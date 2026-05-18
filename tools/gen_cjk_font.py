#!/usr/bin/env python3
"""
gen_cjk_font.py  —  Generate a CJK bitmap font binary for UV-K6 EEPROM.

Usage:
    python tools/gen_cjk_font.py [OPTIONS]

Options:
    --bdf  PATH       BDF bitmap font file to use.
                      Default: tools/wenquanyi_10pt.bdf (in same dir as this script)
    --out  PATH       Output binary file path.  Default: tools/cjk_font.bin
    --chars FILE      Text file containing unique characters to include.
                      Default: auto-extracted from ui/menu_lang.c,
                      ui/menu_sub_values_cn.c and ui/menu.c in the
                      parent directory.
    --preview         Render an ASCII preview of each glyph to stdout.

BDF baseline alignment:
    BASELINE_SCREEN_ROW=11 places CJK glyphs (BBX 12×12, y_off=-1) at screen
    rows 1-12, matching gFontBig ASCII digits which occupy rows 3-12.
    Both CJK and ASCII share baseline at row 12 — no per-glyph misalignment.

Output binary layout (AT24C512 EEPROM at offset 0x2000):
    Offset 0x0000:  uint16_t magic         = 0x4B36 ('K','6' little-endian)
    Offset 0x0002:  uint16_t glyph_count   (little-endian)
    Offset 0x0004:  uint8_t  version       = 1
    Offset 0x0005:  uint8_t  reserved      = 0
    Offset 0x0006:  uint16_t header_crc16  CRC-16/CCITT over bytes 0-5
    Offset 0x0008:  uint16_t codepoints[N] (sorted, little-endian)
                    ... padded to next 8-byte boundary ...
    Offset 0x0008 + pad(2N, 8):
                    uint8_t  glyphs[N][26] (column-major, 13 cols x 2 bytes)

Glyph format  (13 wide × 16 tall pixels, column-major, LSB = top pixel):
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

GLYPH_WIDTH = 13
GLYPH_HEIGHT = 16
BYTES_PER_GLYPH = GLYPH_WIDTH * 2  # column-major, 2 bytes per column (16 rows)

# Screen row of the font baseline.
# With wenquanyi_10pt (FONT_ASCENT=12, CJK BBX 12×12 y_off=-1):
#   CJK glyphs → screen rows 1-12 (aligned to gFontBig ASCII rows 3-12)
BASELINE_SCREEN_ROW = 11

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
            before = len(cps)
            try:
                with open(full, encoding="utf-8", errors="replace") as f:
                    for ch in f.read():
                        cp = ord(ch)
                        if 0x4E00 <= cp <= 0x9FFF:
                            cps.add(cp)
            except FileNotFoundError:
                pass
            print(f"  {fname}: {len(cps) - before} new CJK codepoints", file=sys.stderr)

    result = sorted(cps)
    print(f"Total unique CJK codepoints: {len(result)}", file=sys.stderr)
    return result


# ---------------------------------------------------------------------------
# BDF font parsing and glyph rendering
# ---------------------------------------------------------------------------


def parse_bdf_file(path: str) -> dict:
    """Parse a BDF font file.
    Returns dict: {codepoint: {'bbx': (w,h,x_off,y_off), 'bitmap': [int, ...]}}
    Each bitmap entry is one row (top→bottom), stored as an integer with MSB=left.
    """
    chars = {}
    with open(path, encoding="utf-8", errors="replace") as f:
        lines = f.readlines()

    i = 0
    while i < len(lines):
        line = lines[i].strip()
        if line.startswith("STARTCHAR"):
            encoding = None
            bbx = None
            bitmap: list[int] = []
            i += 1
            while i < len(lines) and lines[i].strip() != "ENDCHAR":
                tok = lines[i].strip()
                if tok.startswith("ENCODING"):
                    encoding = int(tok.split()[1])
                elif tok.startswith("BBX"):
                    parts = tok.split()
                    bbx = (int(parts[1]), int(parts[2]), int(parts[3]), int(parts[4]))
                elif tok.startswith("BITMAP"):
                    i += 1
                    while i < len(lines) and lines[i].strip() != "ENDCHAR":
                        hex_row = lines[i].strip()
                        if hex_row and all(c in "0123456789ABCDEFabcdef" for c in hex_row):
                            bitmap.append(int(hex_row, 16))
                        i += 1
                    break
                i += 1
            if encoding is not None and bbx is not None:
                chars[encoding] = {"bbx": bbx, "bitmap": bitmap}
        i += 1
    return chars


def render_glyph_bdf(bdf_chars: dict, codepoint: int) -> bytes:
    """Render a single CJK glyph from BDF data into column-major format.

    BDF baseline placement:
      screen_row = BASELINE_SCREEN_ROW - bbx_y_off - bbx_h + 1 + bdf_bitmap_row
    With BASELINE_SCREEN_ROW=11 and wenquanyi_10pt CJK (bbx_y=-1, bbx_h=12):
      glyph occupies screen rows 1-12, matching gFontBig ASCII at rows 3-12.
    """
    if codepoint not in bdf_chars:
        return bytes(BYTES_PER_GLYPH)

    entry = bdf_chars[codepoint]
    bbx_w, bbx_h, bbx_x, bbx_y = entry["bbx"]
    bitmap = entry["bitmap"]

    # BDF rows are padded to byte boundaries; total bits per row:
    row_total_bits = ((bbx_w + 7) // 8) * 8

    result = bytearray(BYTES_PER_GLYPH)
    for bdf_row, row_val in enumerate(bitmap):
        screen_row = BASELINE_SCREEN_ROW - bbx_y - bbx_h + 1 + bdf_row
        if not (0 <= screen_row < GLYPH_HEIGHT):
            continue
        for bdf_col in range(bbx_w):
            # MSB of the padded row = leftmost pixel
            bit_pos = row_total_bits - 1 - bdf_col
            if not ((row_val >> bit_pos) & 1):
                continue
            screen_col = bbx_x + bdf_col
            if not (0 <= screen_col < GLYPH_WIDTH):
                continue
            byte_idx = screen_col * 2 + (screen_row // 8)
            result[byte_idx] |= 1 << (screen_row % 8)

    return bytes(result)


def preview_glyph(glyph: bytes, char: str) -> None:
    """Print an ASCII-art preview of a 13×16 glyph."""
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

    default_bdf = os.path.join(script_dir, "wenquanyi_10pt.bdf")

    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument(
        "--bdf",
        default=default_bdf,
        help="BDF bitmap font file (default: tools/wenquanyi_10pt.bdf)",
    )
    parser.add_argument(
        "--out",
        default=os.path.join(script_dir, "cjk_font.bin"),
        help="Output binary file (default: tools/cjk_font.bin)",
    )
    parser.add_argument(
        "--chars", default=None, help="Text file with characters to include"
    )
    parser.add_argument(
        "--preview", action="store_true", help="Print ASCII preview of each glyph"
    )
    args = parser.parse_args()

    if not os.path.exists(args.bdf):
        print(f"ERROR: BDF font not found: {args.bdf}", file=sys.stderr)
        print(
            "  Copy wenquanyi_10pt.bdf into tools/ or specify --bdf PATH",
            file=sys.stderr,
        )
        sys.exit(1)
    print(f"BDF font : {args.bdf}", file=sys.stderr)

    # Parse BDF
    print("Parsing BDF font...", file=sys.stderr)
    bdf_chars = parse_bdf_file(args.bdf)
    print(f"BDF contains {len(bdf_chars)} glyphs", file=sys.stderr)

    # Collect codepoints
    codepoints = collect_codepoints(src_dir, args.chars)
    if not codepoints:
        print("WARNING: No CJK codepoints found.  Check source paths.", file=sys.stderr)
        sys.exit(0)

    # Render glyphs
    glyphs = []
    missing = []
    for cp in codepoints:
        if cp not in bdf_chars:
            missing.append(chr(cp))
        g = render_glyph_bdf(bdf_chars, cp)
        glyphs.append(g)
        if args.preview:
            preview_glyph(g, chr(cp))

    if missing:
        print(
            f"WARNING: {len(missing)} codepoint(s) missing from BDF "
            f"(blank substituted): {''.join(missing)}",
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
