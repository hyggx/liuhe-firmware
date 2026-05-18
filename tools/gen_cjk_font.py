#!/usr/bin/env python3
"""
gen_cjk_font.py  —  Generate a CJK bitmap font binary for UV-K6 EEPROM.

Usage:
    python tools/gen_cjk_font.py [OPTIONS]

Options:
    --font PATH       TrueType/OTF font file to use for rendering.
                      Default: looks for system fonts (WenQuanYi, Noto CJK, etc.)
    --out  PATH       Output binary file path.  Default: tools/cjk_font.bin
    --size PX         Font render size in pixels.  Default: 12
    --chars FILE      Text file containing unique characters to include.
                      Default: auto-extracted from ui/menu_lang.c,
                      ui/menu_sub_values_cn.c and ui/menu.c in the
                      parent directory.
    --preview         Render an ASCII preview of each glyph to stdout.

Output binary layout (AT24C512 EEPROM at offset 0x2000):
    Offset 0x0000:  uint16_t magic         = 0x4B36 ('K','6' little-endian)
    Offset 0x0002:  uint16_t glyph_count   (little-endian)
    Offset 0x0004:  uint8_t  version       = 1
    Offset 0x0005:  uint8_t  reserved      = 0
    Offset 0x0006:  uint16_t header_crc16  CRC-16/CCITT over bytes 0-5
    Offset 0x0008:  uint16_t codepoints[N] (sorted, little-endian)
                    ... padded to next 8-byte boundary ...
    Offset 0x0008 + pad(2N, 8):
                    uint8_t  glyphs[N][24] (column-major, 12 cols x 2 bytes)

Glyph format  (12 wide × 16 tall pixels, column-major, LSB = top pixel):
    glyph[col*2 + 0] = rows  0-7  of column col
    glyph[col*2 + 1] = rows 8-15  of column col

Flash-free: all CJK glyph data lives in AT24C512 EEPROM; firmware only stores
the small index/codepoint table in EEPROM and reads glyphs on demand.
"""

import argparse
import os
import re
import struct
import sys

GLYPH_WIDTH = 12
GLYPH_HEIGHT = 16
BYTES_PER_GLYPH = GLYPH_WIDTH * 2  # column-major, 2 bytes per column (16 rows)

# Font header constants — must match ui/helper.c
CJK_FONT_MAGIC = 0x4B36  # 'K','6' little-endian
CJK_FONT_VERSION = 1


# ---------------------------------------------------------------------------
# CRC-16/CCITT  (poly 0x1021, init 0, no reflection)
# Matches the DP32G030 hardware CRC peripheral used by the firmware.
# ---------------------------------------------------------------------------


def _crc16(data: bytes) -> int:
    crc = 0
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) if (crc & 0x8000) else (crc << 1)
        crc &= 0xFFFF
    return crc


# ---------------------------------------------------------------------------
# Codepoint extraction from source files
# ---------------------------------------------------------------------------


def extract_codepoints_from_c_source(path: str) -> set:
    """Return the set of Unicode codepoints present in UTF-8 string literals
    inside the given C source file."""
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            text = f.read()
    except FileNotFoundError:
        return set()

    codepoints = set()
    # Match C string literals (simple heuristic — not a full C parser)
    for m in re.finditer(r'"((?:[^"\\]|\\.)*)"', text):
        s = m.group(1)
        # Decode escape sequences  \\xHH
        try:
            raw = bytes(s, "utf-8").decode("unicode_escape").encode("latin1")
            decoded = raw.decode("utf-8", errors="replace")
        except Exception:
            try:
                decoded = s
            except Exception:
                continue
        for ch in decoded:
            cp = ord(ch)
            if 0x4E00 <= cp <= 0x9FFF:  # CJK Unified Ideographs
                codepoints.add(cp)
    return codepoints


def collect_codepoints(src_dir: str, chars_file: str | None) -> list:
    """Collect all needed CJK codepoints, sorted."""
    cps = set()
    if chars_file:
        with open(chars_file, encoding="utf-8") as f:
            for ch in f.read():
                cp = ord(ch)
                if 0x4E00 <= cp <= 0x9FFF:
                    cps.add(cp)
    else:
        for fname in ["ui/menu_lang.c", "ui/menu_sub_values_cn.c", "ui/menu.c"]:
            full = os.path.join(src_dir, fname)
            found = extract_codepoints_from_c_source(full)
            cps |= found
            print(f"  {fname}: {len(found)} CJK codepoints", file=sys.stderr)

    result = sorted(cps)
    print(f"Total unique CJK codepoints: {len(result)}", file=sys.stderr)
    return result


# ---------------------------------------------------------------------------
# Glyph rendering
# ---------------------------------------------------------------------------


def find_system_font() -> str | None:
    """Try to locate a suitable CJK TrueType font on the system."""
    candidates = [
        # Linux
        "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
        "/usr/share/fonts/truetype/wqy/wqy-zenhei.ttc",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/noto-cjk/NotoSansCJKsc-Regular.otf",
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
        # macOS
        "/System/Library/Fonts/PingFang.ttc",
        "/Library/Fonts/Arial Unicode MS.ttf",
        # Windows
        r"C:\Windows\Fonts\simsun.ttc",
        r"C:\Windows\Fonts\msyh.ttc",
    ]
    for p in candidates:
        if os.path.exists(p):
            return p
    return None


def render_glyph_pillow(font_path: str, codepoint: int, size: int) -> bytes | None:
    """Render a single glyph using Pillow + freetype-py / ImageFont.
    Returns 24 bytes in column-major format, or None on failure."""
    try:
        from PIL import Image, ImageDraw, ImageFont
    except ImportError:
        print("ERROR: Pillow not installed.  Run: pip install Pillow", file=sys.stderr)
        sys.exit(1)

    char = chr(codepoint)
    try:
        pil_font = ImageFont.truetype(font_path, size)
    except Exception as e:
        print(f"ERROR loading font {font_path}: {e}", file=sys.stderr)
        sys.exit(1)

    # Render onto a small bitmap
    img = Image.new("1", (GLYPH_WIDTH, GLYPH_HEIGHT), 0)
    draw = ImageDraw.Draw(img)

    # Get bounding box to centre the glyph
    try:
        bbox = pil_font.getbbox(char)
        gw = bbox[2] - bbox[0]
        gh = bbox[3] - bbox[1]
        x_off = (GLYPH_WIDTH - gw) // 2 - bbox[0]
        y_off = (GLYPH_HEIGHT - gh) // 2 - bbox[1]
    except Exception:
        x_off, y_off = 0, 0

    draw.text((x_off, y_off), char, font=pil_font, fill=1)

    # Convert to column-major bytes
    result = bytearray(BYTES_PER_GLYPH)
    for col in range(GLYPH_WIDTH):
        lo = 0
        hi = 0
        for row in range(8):
            if img.getpixel((col, row)):
                lo |= 1 << row
        for row in range(8, GLYPH_HEIGHT):
            if img.getpixel((col, row)):
                hi |= 1 << (row - 8)
        result[col * 2 + 0] = lo
        result[col * 2 + 1] = hi
    return bytes(result)


def preview_glyph(glyph: bytes, char: str) -> None:
    """Print an ASCII-art preview of a 12×16 glyph."""
    print(f"U+{ord(char):04X} '{char}'")
    for row in range(GLYPH_HEIGHT):
        line = ""
        for col in range(GLYPH_WIDTH):
            byte_idx = col * 2 + (row // 8)
            bit = row % 8
            if glyph[byte_idx] & (1 << bit):
                line += "##"
            else:
                line += ".."
        print(line)
    print()


# ---------------------------------------------------------------------------
# Binary layout builder
# ---------------------------------------------------------------------------


def build_font_binary(codepoints: list, glyphs: list[bytes]) -> bytes:
    """Assemble the complete binary blob to write to EEPROM at 0x2000."""
    n = len(codepoints)
    assert len(glyphs) == n

    out = bytearray()

    # 8-byte header block:
    #   [0-1] magic 0x4B36  [2-3] glyph_count  [4] version  [5] reserved  [6-7] CRC16
    hdr_body = struct.pack("<HHBBxx", CJK_FONT_MAGIC, n, CJK_FONT_VERSION, 0)
    hdr_crc = _crc16(hdr_body[:6])
    out += hdr_body[:6] + struct.pack("<H", hdr_crc)

    # Codepoint index table (sorted uint16_t array), padded to 8 bytes
    idx_bytes = struct.pack(f"<{n}H", *codepoints)
    out += idx_bytes
    # Pad to next 8-byte boundary
    pad = (8 - len(idx_bytes) % 8) % 8
    out += b"\xff" * pad

    # Glyph data (packed, no alignment between glyphs)
    for g in glyphs:
        out += g

    return bytes(out)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main():
    # Locate script directory and repo root
    script_dir = os.path.dirname(os.path.abspath(__file__))
    src_dir = os.path.dirname(script_dir)  # parent = firmware root

    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument("--font", default=None, help="TrueType/OTF font file path")
    parser.add_argument(
        "--out",
        default=os.path.join(script_dir, "cjk_font.bin"),
        help="Output binary file (default: tools/cjk_font.bin)",
    )
    parser.add_argument(
        "--size", type=int, default=12, help="Render size in pixels (default: 12)"
    )
    parser.add_argument(
        "--chars", default=None, help="Text file with characters to include"
    )
    parser.add_argument(
        "--preview", action="store_true", help="Print ASCII preview of each glyph"
    )
    args = parser.parse_args()

    # Find font
    font_path = args.font or find_system_font()
    if not font_path:
        print("ERROR: No CJK font found.  Specify one with --font.", file=sys.stderr)
        print(
            "  Suggested: apt install fonts-wqy-microhei  (Debian/Ubuntu)",
            file=sys.stderr,
        )
        sys.exit(1)
    print(f"Using font: {font_path}", file=sys.stderr)

    # Collect codepoints
    codepoints = collect_codepoints(src_dir, args.chars)
    if not codepoints:
        print("WARNING: No CJK codepoints found.  Check source paths.", file=sys.stderr)
        sys.exit(0)

    # Render glyphs
    glyphs = []
    failed = 0
    for cp in codepoints:
        g = render_glyph_pillow(font_path, cp, args.size)
        if g is None:
            g = bytes(BYTES_PER_GLYPH)  # blank glyph
            failed += 1
        glyphs.append(g)
        if args.preview:
            preview_glyph(g, chr(cp))

    if failed:
        print(
            f"WARNING: {failed} glyph(s) could not be rendered (blank substituted).",
            file=sys.stderr,
        )

    # Build and write binary
    binary = build_font_binary(codepoints, glyphs)
    with open(args.out, "wb") as f:
        f.write(binary)

    total_eeprom = (
        8 + ((len(codepoints) * 2 + 7) & ~7) + len(codepoints) * BYTES_PER_GLYPH
    )
    print(f"Written {len(binary)} bytes  →  {args.out}", file=sys.stderr)
    print(
        f"EEPROM usage: 0x2000 .. 0x{0x2000 + total_eeprom:04X}  ({total_eeprom} bytes)",
        file=sys.stderr,
    )
    print(f"Glyphs: {len(codepoints)}  (each {BYTES_PER_GLYPH} bytes)", file=sys.stderr)
    print(
        f"Remaining EEPROM: {65536 - 0x2000 - total_eeprom} bytes free", file=sys.stderr
    )


if __name__ == "__main__":
    main()
