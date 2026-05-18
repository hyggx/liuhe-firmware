# Hygg - UV-K6 V1 Custom Firmware

Custom firmware for the **Quansheng UV-K6 V1** (DP32G030 / ARM Cortex-M0), forked from
[egzumer/uv-k5-firmware-custom](https://github.com/egzumer/uv-k5-firmware-custom).

> [!WARNING]
> Use at your own risk. This firmware may brick your radio.
> **Always back up your EEPROM before flashing.**

## Hardware Target

> **Supported hardware:** UV-K6 V1 **final production version only** — the variant with the
> 512 KB (4 Mbit) external EEPROM. V1 is now end-of-life; only dealer stock remains.
> This firmware is **not compatible** with UV-K6 V3 or UV-K1 (PY32F071 MCU).

| Item      | Details                                                   |
| --------- | --------------------------------------------------------- |
| Radio     | Quansheng UV-K6 V1 (final production, EOL)                |
| MCU       | DP32G030 (ARM Cortex-M0, 48 MHz)                          |
| MCU Flash | 64 KB internal — firmware code lives here                 |
| MCU RAM   | 16 KB                                                     |
| EEPROM    | 512 KB external I2C — channel data, settings, calibration |
| Radio IC  | BK4819 (VHF/UHF transceiver)                              |
| FM IC     | BK1080 (WBFM broadcast RX)                                |
| Display   | ST7565 128x64 LCD                                         |

## Project Status

Current release: **v0.1.0** (2026-05-18).  
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

Use the [UV-K5 Web Flasher](https://egzumer.github.io/uvtools) with `firmware.packed.bin`.

## Build Options

Edit the `ENABLE_XXX` flags at the top of `Makefile` (`0` = disable, `1` = enable),
or select a build profile with `make PROFILE=release` (default) / `make PROFILE=debug`.

| Option                        | Default | Description                              |
| ----------------------------- | ------- | ---------------------------------------- |
| `ENABLE_UART`                 | 1       | PC configuration via UART — keep enabled |
| `ENABLE_FMRADIO`              | 1       | WBFM broadcast receiver                  |
| `ENABLE_VOX`                  | 1       | Voice-operated TX                        |
| `ENABLE_DTMF_CALLING`         | **0**   | DTMF call system (disabled to fit flash) |
| `ENABLE_FLASHLIGHT`           | 1       | Top LED (on / blink / SOS)               |
| `ENABLE_SPECTRUM`             | 1       | Spectrum analyzer (`F`+`5`)              |
| `ENABLE_AM_FIX`               | 1       | Dynamic AM front-end gain                |
| `ENABLE_RSSI_BAR`             | 1       | dBm/S-meter bar                          |
| `ENABLE_AUDIO_BAR`            | 1       | TX audio level bar                       |
| `ENABLE_BIG_FREQ`             | 1       | Large frequency font                     |
| `ENABLE_WIDE_RX`              | 1       | 18 MHz – 1300 MHz RX range               |
| `ENABLE_SCAN_RANGES`          | 1       | Frequency range scan mode                |
| `ENABLE_NO_CODE_SCAN_TIMEOUT` | 1       | Disable 32s CTCSS/DCS scan timeout       |
| `ENABLE_LTO`                  | 1       | Link-time optimization (reduces size)    |

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
