# Haige (海歌) — UV-K6 V1 Custom Firmware

Custom firmware for the **Quansheng UV-K6 V1** (DP32G030 / ARM Cortex-M0), forked from
[egzumer/uv-k5-firmware-custom](https://github.com/egzumer/uv-k5-firmware-custom).

[中文文档](README.md) | English

> [!WARNING]
> Use at your own risk. This firmware may brick your radio.
> **Always back up your EEPROM before flashing.**

## Hardware Target

> **Supported hardware:** UV-K6 with DP32G030 MCU + 512 KB EEPROM only.
> In community convention, all DP32G030-based UV-K6 boards (PCB V1 and V2) are
> called "V1", because the true generational break is the V3 which swapped to a
> PY32F071 MCU.  The 512 KB EEPROM variant is the later PCB revision (hardware
> V2); early boards shipped with only 8 KB.
> This firmware is **not compatible** with UV-K6 V3 / UV-K1 (PY32F071 MCU).

| Item      | Details                                                   |
| --------- | --------------------------------------------------------- |
| Radio     | Quansheng UV-K6 (DP32G030, community "V1" — PCB V1/V2, 512 KB EEPROM) |
| MCU       | DP32G030 (ARM Cortex-M0, 48 MHz)                          |
| MCU Flash | 64 KB internal — firmware code lives here                 |
| MCU RAM   | 16 KB                                                     |
| EEPROM    | 512 KB external I2C — channel data, settings, calibration, CJK font |
| Radio IC  | BK4819 (VHF/UHF transceiver)                              |
| FM IC     | BK1080 (WBFM broadcast RX)                                |
| Display   | ST7565 128x64 LCD                                         |

## Project Status

Current release: **v0.6.0** (2026-05-24).  
See [CHANGELOG.md](./CHANGELOG.md) for the full change history.

## Features

Most features from egzumer v0.22 baseline are preserved:

- **Spectrum analyzer** (`F` + `5`) - fagci implementation
- **AM fix** - dynamic front-end gain control (OneOfEleven)
- **FM radio** - WBFM VHF broadcast receiver
- **DTMF calling** - contacts, group calls, ANI *(disabled by default in this fork — reclaims ~3 KB flash)*
- **Configurable buttons** - long-press assignments from menu
- **RSSI / audio bar** - dBm S-meter and TX audio level
- **Fast scan** - channel and frequency range scanning
- **Big font** frequencies, small bold channel names
- Battery voltage calibration, percentage display
- Backlight dimming, longer backlight timers

For the full feature list, see the
[upstream README](https://github.com/egzumer/uv-k5-firmware-custom/blob/main/README.md).

## Building

### Prerequisites - WSL2 / Ubuntu 24.04 (recommended)

```bash
sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi make python3-crcmod
```

### Build

```bash
# In WSL2, from the repo root:
make

# Output:
#   firmware.bin        - raw binary
#   firmware.packed.bin - flashable image (with CRC)
```

Full clean build: ~6 seconds. Incremental (single file changed): ~1-2 seconds.

Compiler: `arm-none-eabi-gcc 13.2.1` (Ubuntu 24.04 package).

### Windows (Docker)

```bat
docker-make.bat
```

Builds toolchain image on first run (Ubuntu 24.04 base), then mounts source as volume.
Output goes to `compiled-firmware\`.

### Flashing

#### Step 1 — Flash the firmware

Use the [UV-K5 Web Flasher](https://egzumer.github.io/uvtools) with `firmware.packed.bin`.

#### Step 2 — Flash the CJK font *(Chinese builds only)*

The default build (`ENABLE_CHINESE=1`) renders menu text using a bitmap font
stored in the radio's external EEPROM.  **Without this step all CJK characters
appear as blank boxes.**  Flash it once after installing the firmware, and
again whenever you rebuild with a new font (i.e. after adding custom labels).

Requires **pyserial** (`pip install pyserial`). Turn on the radio and connect
it via USB before running:

```bash
# macOS / Linux
python3 tools/flash_font.py --port /dev/ttyUSB0 --font tools/cjk_font.bin

# macOS (CP210x USB–serial adapter)
python3 tools/flash_font.py --port /dev/tty.usbserial-XXXX --font tools/cjk_font.bin
```

```bat
:: Windows
python tools\flash_font.py --port COM3 --font tools\cjk_font.bin
```

The tool prints each page written and verifies the data. The font lives at
EEPROM offset `0x2000` and is independent of channel memory and settings.

## Build Options

Edit the `ENABLE_XXX` flags at the top of `Makefile` (`0` = disable, `1` = enable),
or select a build profile with `make PROFILE=release` (default) / `make PROFILE=debug`.

| Option                        | Default   | Description                              |
| ----------------------------- | --------- | ---------------------------------------- |
| `ENABLE_UART`                 | 1         | PC configuration via UART — keep enabled |
| `ENABLE_FMRADIO`              | 1         | WBFM broadcast receiver                  |
| `ENABLE_CHINESE`              | **1**     | CJK font UI — auto-disables VOX to stay within flash budget |
| `ENABLE_VOX`                  | 0 (auto)† | Voice-operated TX                        |
| `ENABLE_DTMF_CALLING`         | **0**     | DTMF call system (disabled to fit flash) |
| `ENABLE_FLASHLIGHT`           | 1         | Top LED (on / blink / SOS)               |
| `ENABLE_SPECTRUM`             | 1         | Spectrum analyzer (`F`+`5`)              |
| `ENABLE_AM_FIX`               | 1         | Dynamic AM front-end gain                |
| `ENABLE_RSSI_BAR`             | 1         | dBm/S-meter bar                          |
| `ENABLE_AUDIO_BAR`            | 1         | TX audio level bar                       |
| `ENABLE_BIG_FREQ`             | 1         | Large frequency font                     |
| `ENABLE_WIDE_RX`              | 1         | 18 MHz – 1300 MHz RX range               |
| `ENABLE_SCAN_RANGES`          | 1         | Frequency range scan mode                |
| `ENABLE_NO_CODE_SCAN_TIMEOUT` | 1         | Disable 32s CTCSS/DCS scan timeout       |
| `ENABLE_LTO`                  | 1         | Link-time optimization (reduces size)    |

† When `ENABLE_CHINESE=1` (the default), the Makefile automatically overrides
`ENABLE_VOX` to `0` to stay within the 60 KB flash budget.  To re-enable VOX,
set `ENABLE_CHINESE=0` or disable other features to free space.

Full list: see comments in [Makefile](./Makefile).

## Repository Structure

```
uv-k6-dp32-firmware/
+-- app/          Application layer (menus, DTMF, scanner, spectrum)
+-- driver/       Hardware drivers (BK4819, BK1080, ST7565, EEPROM, UART)
+-- ui/           Display rendering
+-- helper/       Battery, boot
+-- external/     CMSIS_5 (Cortex-M CMSIS), printf
+-- bsp/          Board support (DP32G030 register map)
+-- Makefile      Build system with ENABLE_XXX feature flags
+-- firmware.ld   Linker script (64 KB internal Flash / 60 KB usable, 16 KB RAM)
+-- fw-pack.py    CRC packing for flashable .packed.bin
```

## Branch Structure

| Branch                 | Purpose                                                  |
| ---------------------- | -------------------------------------------------------- |
| `main`                 | Primary development and release branch                   |
| `vendor/egzumer-v0.22` | Locked upstream baseline (commit `7607f0a`)              |
| `k6-hardening`         | Historical development branch (fully merged into `main`) |
| `main-dp32-k6`         | Old staging branch (superseded)                          |

## Credits

This firmware stands on the work of:

- [egzumer](https://github.com/egzumer) - primary upstream, feature integration
- [OneOfEleven](https://github.com/OneOfEleven) - AM fix, fast scan, configurable buttons
- [fagci / Mikhail](https://github.com/fagci) - spectrum analyzer
- [DualTachyon](https://github.com/DualTachyon) - original open-source firmware
- [Andrej](https://github.com/Tunas1337), [Manuel](https://github.com/manujedi), and many others

## License

Apache License 2.0 - inherited from upstream.

Original copyright: Copyright 2023 Dual Tachyon / egzumer.
See [LICENSE](./LICENSE) for the full text.
