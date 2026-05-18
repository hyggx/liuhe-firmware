#!/usr/bin/env python3
"""flash_font.py — Write the CJK font binary to UV-K6 V1 EEPROM (AT24C512).

The font is written starting at EEPROM address 0x2000.
The radio firmware must be compiled with ENABLE_CHINESE=1.

Requirements:
    pip install pyserial

Usage:
    python tools/flash_font.py --port COM3  --font tools/cjk_font.bin
    python tools/flash_font.py --port COM3  --font tools/cjk_font.bin --no-verify
    python tools/flash_font.py --port /dev/ttyUSB0 --font tools/cjk_font.bin

UART protocol summary (UV-K5/K6 proprietary):
    Frame  (host → radio):
        0xAB 0xCD               outer start (2 B)
        OuterSize  uint16_t LE  = InnerSize + 4
        ID         uint16_t LE  command ID
        InnerSize  uint16_t LE  payload size (bytes after the two header fields)
        Payload    InnerSize B  (XOR-obfuscated when encrypted; unencrypted here)
        CRC        uint16_t LE  CRC-16/CCITT over (ID + InnerSize + Payload)
        0xDC 0xBA               outer end   (2 B)

    After the 0x0514 handshake the firmware disables XOR obfuscation for the
    rest of the session, so all frames sent by this tool are plaintext.
"""

import argparse
import struct
import sys
import time

try:
    import serial
except ImportError:
    sys.exit("ERROR: pyserial not installed.  Run:  pip install pyserial")

# ---------------------------------------------------------------------------
# Protocol constants
# ---------------------------------------------------------------------------

_OBFUSCATION = bytes(
    [
        0x16,
        0x6C,
        0x14,
        0xE6,
        0x2E,
        0x91,
        0x0D,
        0x40,
        0x21,
        0x35,
        0xD5,
        0x40,
        0x13,
        0x03,
        0xE9,
        0x80,
    ]
)

EEPROM_FONT_BASE = 0x2000  # start of CJK font region in AT24C512
EEPROM_SIZE = 0x10000  # total EEPROM size (64 KB)

WRITE_CHUNK = 128  # bytes per 0x051D write (must be multiple of 8)
READ_CHUNK = 128  # bytes per 0x051B read  (max 128; firmware buffer limit)
BAUD_RATE = 38400
TIMEOUT_S = 5.0  # generous: 128-byte write triggers 16 × 8-ms EEPROM burns


# ---------------------------------------------------------------------------
# CRC-16/CCITT  (poly 0x1021, init 0x0000, no input/output reflection)
# Matches the DP32G030 hardware CRC peripheral configuration in driver/crc.c
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
# Frame assembly / parsing
# ---------------------------------------------------------------------------


def _build_frame(cmd_id: int, inner_data: bytes, encrypted: bool = False) -> bytes:
    """Assemble a complete host-to-radio UART frame."""
    inner_size = len(inner_data)
    outer_size = inner_size + 4  # ID(2) + InnerSize(2) + Payload(inner_size)
    header = struct.pack("<HH", cmd_id, inner_size)
    payload = header + inner_data  # outer_size bytes
    crc = struct.pack("<H", _crc16(payload))
    full = payload + crc  # outer_size + 2 bytes
    if encrypted:
        full = bytes(b ^ _OBFUSCATION[i % 16] for i, b in enumerate(full))
    return b"\xab\xcd" + struct.pack("<H", outer_size) + full + b"\xdc\xba"


def _parse_reply(raw: bytes, expected_id: int, encrypted: bool = False) -> bytes:
    """Validate a radio-to-host reply frame; return inner payload bytes."""
    idx = raw.find(b"\xab\xcd")
    if idx < 0:
        raise ValueError("No 0xABCD start marker in reply")
    idx += 2
    if idx + 2 > len(raw):
        raise ValueError("Reply too short (no outer-size field)")
    outer_size = struct.unpack_from("<H", raw, idx)[0]
    idx += 2  # now pointing at the inner ID field
    full = raw[idx : idx + outer_size + 2]
    if len(full) < outer_size + 2:
        raise ValueError(f"Reply truncated ({len(full)} B, need {outer_size + 2} B)")
    if encrypted:
        full = bytes(b ^ _OBFUSCATION[i % 16] for i, b in enumerate(full))
    crc_computed = _crc16(full[:outer_size])
    crc_received = struct.unpack_from("<H", full, outer_size)[0]
    if crc_computed != crc_received:
        raise ValueError(
            f"CRC mismatch: computed {crc_computed:#06x}, received {crc_received:#06x}"
        )
    reply_id, inner_size = struct.unpack_from("<HH", full, 0)
    if reply_id != expected_id:
        raise ValueError(
            f"Unexpected reply ID {reply_id:#06x} (expected {expected_id:#06x})"
        )
    return full[4 : 4 + inner_size]


def _send_recv(
    ser: "serial.Serial", frame: bytes, expected_id: int, encrypted: bool = False
) -> bytes:
    """Send a frame and collect the reply, waiting up to TIMEOUT_S."""
    ser.reset_input_buffer()
    ser.write(frame)
    ser.flush()
    raw = b""
    deadline = time.monotonic() + TIMEOUT_S
    while time.monotonic() < deadline:
        chunk = ser.read(ser.in_waiting or 1)
        if chunk:
            raw += chunk
        if b"\xdc\xba" in raw:
            break
        time.sleep(0.005)
    return _parse_reply(raw, expected_id, encrypted)


# ---------------------------------------------------------------------------
# Protocol operations
# ---------------------------------------------------------------------------


def _handshake(ser: "serial.Serial") -> tuple:
    """Send CMD_0514 (session init); returns (version_str, timestamp).

    0x0514 must always be sent unencrypted.  The firmware disables XOR
    obfuscation for the rest of the session upon receiving it.
    """
    ts = int(time.time()) & 0xFFFFFFFF
    frame = _build_frame(0x0514, struct.pack("<I", ts), encrypted=False)
    data = _send_recv(ser, frame, expected_id=0x0515, encrypted=False)
    # REPLY_0514_t.Data layout: Version[16], bHasCustomAesKey, bIsInLockScreen,
    #                            Padding[2], Challenge[16]
    version = data[:16].rstrip(b"\x00").decode("ascii", errors="replace")
    return version, ts


def _write_block(
    ser: "serial.Serial", eeprom_addr: int, chunk: bytes, timestamp: int
) -> None:
    """Send CMD_051D and wait for CMD_051E ACK.

    Inner payload layout (CMD_051D_t minus Header_t):
        Offset uint16_t, Size uint8_t, bAllowPassword uint8_t,
        Timestamp uint32_t, Data[Size]
    """
    inner = struct.pack("<HBBI", eeprom_addr, len(chunk), 0, timestamp) + chunk
    frame = _build_frame(0x051D, inner, encrypted=False)
    rep = _send_recv(ser, frame, expected_id=0x051E, encrypted=False)
    # REPLY_051D_t.Data: Offset uint16_t
    rep_offset = struct.unpack_from("<H", rep, 0)[0]
    if rep_offset != eeprom_addr:
        raise ValueError(
            f"ACK offset {rep_offset:#06x} does not match sent {eeprom_addr:#06x}"
        )


def _read_block(
    ser: "serial.Serial", eeprom_addr: int, size: int, timestamp: int
) -> bytes:
    """Send CMD_051B and return the read bytes.

    Inner payload layout (CMD_051B_t minus Header_t):
        Offset uint16_t, Size uint8_t, Padding uint8_t, Timestamp uint32_t

    Reply inner data layout (REPLY_051B_t.Data):
        Offset uint16_t, Size uint8_t, Padding uint8_t, Data[Size]
    """
    inner = struct.pack("<HBBI", eeprom_addr, size, 0, timestamp)
    frame = _build_frame(0x051B, inner, encrypted=False)
    rep = _send_recv(ser, frame, expected_id=0x051C, encrypted=False)
    return rep[4 : 4 + size]  # skip Offset(2)+Size(1)+Padding(1)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------


def main() -> None:
    ap = argparse.ArgumentParser(
        description="Write CJK font binary to UV-K6 V1 EEPROM at 0x2000."
    )
    ap.add_argument(
        "--port", required=True, help="Serial port (e.g. COM3 or /dev/ttyUSB0)"
    )
    ap.add_argument(
        "--font", required=True, help="Font binary produced by gen_cjk_font.py"
    )
    ap.add_argument(
        "--baud", type=int, default=BAUD_RATE, help=f"Baud rate (default {BAUD_RATE})"
    )
    ap.add_argument(
        "--no-verify",
        dest="verify",
        action="store_false",
        help="Skip read-back verification after writing",
    )
    ap.set_defaults(verify=True)
    args = ap.parse_args()

    # Load and pad font file to 8-byte boundary (EEPROM writes 8 B at a time)
    with open(args.font, "rb") as fh:
        font_data = fh.read()
    if not font_data:
        sys.exit("ERROR: font file is empty")
    remainder = len(font_data) % 8
    if remainder:
        font_data += b"\xff" * (8 - remainder)

    end_addr = EEPROM_FONT_BASE + len(font_data) - 1
    if end_addr >= EEPROM_SIZE:
        sys.exit(
            f"ERROR: font too large — would end at 0x{end_addr:04X} "
            f"(EEPROM limit 0x{EEPROM_SIZE - 1:04X})"
        )

    print(f"Font file : {args.font}  ({len(font_data):,} bytes)")
    print(f"EEPROM    : 0x{EEPROM_FONT_BASE:04X}..0x{end_addr:04X}")
    print(f"Port      : {args.port} @ {args.baud} baud")

    with serial.Serial(args.port, args.baud, timeout=TIMEOUT_S) as ser:
        # --- Handshake ---------------------------------------------------
        try:
            version, ts = _handshake(ser)
        except Exception as exc:
            sys.exit(
                f"ERROR: handshake failed — {exc}\n"
                "  Check: radio powered on, USB connected, correct port,\n"
                "         firmware compiled with ENABLE_CHINESE=1."
            )
        print(f"Firmware  : {version}")

        # --- Write -------------------------------------------------------
        total = len(font_data)
        written = 0
        print("Writing: ", end="", flush=True)
        for off in range(0, total, WRITE_CHUNK):
            chunk = font_data[off : off + WRITE_CHUNK]
            addr = EEPROM_FONT_BASE + off
            try:
                _write_block(ser, addr, chunk, ts)
            except Exception as exc:
                print(f"\nERROR at EEPROM 0x{addr:04X}: {exc}", file=sys.stderr)
                sys.exit(1)
            written += len(chunk)
            # Progress dot every WRITE_CHUNK, newline every 4 KB
            print(
                ".",
                end=("" if written % (WRITE_CHUNK * 32) else "\n         "),
                flush=True,
            )
        print(f"  {written:,} bytes written.")

        # --- Verify (optional) -------------------------------------------
        if args.verify:
            print("Verifying: ", end="", flush=True)
            errors = 0
            for off in range(0, total, READ_CHUNK):
                expected = font_data[off : off + READ_CHUNK]
                addr = EEPROM_FONT_BASE + off
                try:
                    actual = _read_block(ser, addr, len(expected), ts)
                    if actual != expected:
                        print(
                            f"\n  MISMATCH at 0x{addr:04X} "
                            f"(first diff byte: {expected[0]:#04x} vs {actual[0] if actual else '?'})"
                        )
                        errors += 1
                except Exception as exc:
                    print(f"\n  ERROR at 0x{addr:04X}: {exc}")
                    errors += 1
                print(".", end="", flush=True)
            print()
            if errors:
                sys.exit(f"Verify FAILED — {errors} block(s) mismatched.")
            print("Verify OK.")


if __name__ == "__main__":
    main()
