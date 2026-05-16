# Hygg 鈥?UV-K6 V1 Custom Firmware

Custom firmware for the **Quansheng UV-K6 V1** (DP32G030 / ARM Cortex-M0), forked from
[egzumer/uv-k5-firmware-custom](https://github.com/egzumer/uv-k5-firmware-custom).

> [!WARNING]
> Use at your own risk. This firmware may brick your radio.
> **Always back up your EEPROM before flashing.**

## Hardware Target

| Item     | Details                          |
| -------- | -------------------------------- |
| Radio    | Quansheng UV-K6 V1               |
| MCU      | DP32G030 (ARM Cortex-M0, 48 MHz) |
| Flash    | 512 KB internal                  |
| RAM      | 16 KB                            |
| Radio IC | BK4819 (VHF/UHF transceiver)     |
| FM IC    | BK1080 (WBFM broadcast RX)       |
| Display  | ST7565 128脳64 LCD                |

## Project Status

See [CHANGELOG.md](./CHANGELOG.md) for detailed progress.

| Phase   | Description                    | Status        |
| ------- | ------------------------------ | ------------- |
| Phase 0 | Baseline build verification    | 鉁?Done        |
| Phase 1 | P0 security hardening          | 馃敡 In progress |
| Phase 2 | Regression test suite          | 鈴?Planned     |
| Phase 3 | Ham-core features for UV-K6 V1 | 鈴?Planned     |

## Features

All features from egzumer v0.22 baseline are preserved:

- **Spectrum analyzer** (`F` + `5`) 鈥?fagci implementation
- **AM fix** 鈥?dynamic front-end gain control (OneOfEleven)
- **FM radio** 鈥?WBFM VHF broadcast receiver
- **DTMF calling** 鈥?contacts, group calls, ANI
- **Configurable buttons** 鈥?long-press assignments from menu
- **RSSI / audio bar** 鈥?dBm S-meter and TX audio level
- **Fast scan** 鈥?channel and frequency range scanning
- **Big font** frequencies, small bold channel names
- Battery voltage calibration, percentage display
- Backlight dimming, longer backlight timers

For the full feature list, see the
[upstream README](https://github.com/egzumer/uv-k5-firmware-custom/blob/main/README.md).

## Building

### Prerequisites 鈥?WSL2 / Ubuntu 24.04 (recommended)

```bash
sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi make python3-crcmod
```

### Build

```bash
# In WSL2, from the repo root:
make

# Output:
#   firmware.bin        鈥?raw binary
#   firmware.packed.bin 鈥?flashable image (with CRC)
```

Full clean build: ~6 seconds. Incremental (single file changed): ~1鈥? seconds.

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

Edit the `ENABLE_XXX` flags at the top of `Makefile` (`0` = disable, `1` = enable).

| Option                        | Description                                  |
| ----------------------------- | -------------------------------------------- |
| `ENABLE_UART`                 | PC configuration via UART 鈥?**keep enabled** |
| `ENABLE_FMRADIO`              | WBFM broadcast receiver                      |
| `ENABLE_VOX`                  | Voice-operated TX                            |
| `ENABLE_DTMF_CALLING`         | DTMF call system                             |
| `ENABLE_FLASHLIGHT`           | Top LED (on / blink / SOS)                   |
| `ENABLE_SPECTRUM`             | Spectrum analyzer (`F`+`5`)                  |
| `ENABLE_AM_FIX`               | Dynamic AM front-end gain                    |
| `ENABLE_RSSI_BAR`             | dBm/S-meter bar                              |
| `ENABLE_AUDIO_BAR`            | TX audio level bar                           |
| `ENABLE_BIG_FREQ`             | Large frequency font                         |
| `ENABLE_WIDE_RX`              | 18 MHz 鈥?1300 MHz RX range                   |
| `ENABLE_SCAN_RANGES`          | Frequency range scan mode                    |
| `ENABLE_NO_CODE_SCAN_TIMEOUT` | Disable 32s CTCSS/DCS scan timeout           |
| `ENABLE_LTO`                  | Link-time optimization (reduces size)        |

Full list: see comments in [Makefile](./Makefile).

## Repository Structure

```
uv-k6-dp32-firmware/
鈹溾攢鈹€ app/          # Application layer (menus, DTMF, scanner, spectrum)
鈹溾攢鈹€ driver/       # Hardware drivers (BK4819, BK1080, ST7565, EEPROM, UART)
鈹溾攢鈹€ ui/           # Display rendering
鈹溾攢鈹€ helper/       # Battery, boot
鈹溾攢鈹€ external/     # CMSIS_5 (Cortex-M CMSIS), printf
鈹溾攢鈹€ bsp/          # Board support (DP32G030 register map)
鈹溾攢鈹€ Makefile      # Build system with ENABLE_XXX feature flags
鈹溾攢鈹€ firmware.ld   # Linker script (512KB Flash, 16KB RAM)
鈹斺攢鈹€ fw-pack.py    # CRC packing for flashable .packed.bin
```

## Branch Structure

| Branch                 | Purpose                                     |
| ---------------------- | ------------------------------------------- |
| `main`                 | Mirrors upstream egzumer                    |
| `vendor/egzumer-v0.22` | Locked upstream baseline (commit `7607f0a`) |
| `main-dp32-k6`         | Primary development branch                  |
| `k6-hardening`         | Phase 1 security fixes                      |

## Credits

This firmware stands on the work of:

- [egzumer](https://github.com/egzumer) 鈥?primary upstream, feature integration
- [OneOfEleven](https://github.com/OneOfEleven) 鈥?AM fix, fast scan, configurable buttons
- [fagci / Mikhail](https://github.com/fagci) 鈥?spectrum analyzer
- [DualTachyon](https://github.com/DualTachyon) 鈥?original open-source firmware
- [Andrej](https://github.com/Tunas1337), [Manuel](https://github.com/manujedi), and many others

## License

Apache License 2.0 鈥?inherited from upstream.

Original copyright: Copyright 2023 Dual Tachyon / egzumer.
See [LICENSE](./LICENSE) for the full text.
