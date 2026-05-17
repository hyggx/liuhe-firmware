# Changelog

All notable changes to this project are documented in this file.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).  
Version scheme: `MAJOR.MINOR.PATCH[-label]` — `0.x` series is pre-release.

---

## [Unreleased]

### Fixed — Squelch hysteresis inversion (branch: `k6-hardening`)

- **[P1] `radio.c` `RADIO_ConfigureSquelchAndOutputPower`** —
  Fixed two squelch Schmitt-trigger invariant violations in the
  `ENABLE_SQUELCH_MORE_SENSITIVE` path.

  **Root cause:** The upstream code scaled only the *open* thresholds
  (`rssi_open ÷ 2`, `noise_open × 2`) while leaving the corresponding
  *close* thresholds at their original EEPROM values.  After scaling:

  | Threshold | Before fix | After fix |
  |-----------|-----------|-----------|
  | RSSI open | 50 → **25** | 50 → **25** |
  | RSSI close | 40 (unchanged) → **close > open** ✗ | 40 → **20** ✓ |
  | Noise open | 65 → **127** (sat) | 65 → **127** (sat) |
  | Noise close | 70 (unchanged) → **close < open** ✗ | 70 → **127** (sat) ✓ |

  With `rssi_close(40) > rssi_open(25)` the BK4819 hardware squelch
  would open at RSSI=25 but immediately satisfy the close condition
  (RSSI ≤ 40), producing chatter on weak signals (RSSI 25–40).
  The symptom was masked in practice only by the REG_4E time-domain
  delays (`open_delay=5`, `close_delay=6`).

  **Fix:** Scale both open and close thresholds by the same factor.
  The hysteresis window shrinks proportionally but the Schmitt direction
  is preserved.  Post-scale invariant guards cover any residual edge cases.

  *Example VHF Sql=5 (EEPROM default values):*
  - RSSI: open 50→25, close 40→20 — hysteresis 10→5 (proportional) ✓
  - Noise: both saturate at 127 — noise criterion disabled in hi-sens mode ✓
  - Glitch: open 100→200 (×2), close 90 (unchanged) — wide window ✓

### Security — Phase 1 (branch: `k6-hardening`)

Confirmed fixes applied.  Flash budget: `text 61396 B / 61440 B` (0 B free after
this patch set — any future addition requires disabling an `ENABLE_` feature first).

- **[P0] `driver/eeprom.c`** — `EEPROM_WriteBuffer`: corrected upper-bound check from
  `Address >= 0x2000` to `Address > (0x2000 - 8)` so the 8-byte page write cannot
  walk past the end of the 8 KB EEPROM address space.
- **[P0] `app/uart.c` CMD_051B** — Added runtime guard: reject UART read requests
  where `pCmd->Size > 128` (the reply data buffer size); previously a malformed frame
  could cause `EEPROM_ReadBuffer` to overflow `Reply.Data.Data[]` and leak stack data.
- **[P0] `app/uart.c` CMD_051D** — Added runtime guard: reject UART write requests
  where `Offset + Size > 0x2000`; previously an adversary could supply a large offset
  to walk I2C writes past the end of the EEPROM.
- **[P1] `app/uart.c` SendVersion** — Added comment documenting that `strcpy` into
  `Version[16]` is safe for the current `AUTHOR_STRING` + `VERSION_STRING` length
  (15 chars + null = exactly 16 bytes); flagged for `strncpy` conversion if string grows.
- **[P1] `radio.c` `RADIO_ApplyOffset`** — Fixed dead condition in aviation-band check:
  `frequency >= .upper && frequency < .upper` is always false; corrected to
  `frequency >= .lower && frequency < .upper` so TX offset is properly disabled for
  the 108–137 MHz AM aviation band.
- **[P1] `driver/bk4819.c` `BK4819_SetAGC`** — Added explanatory comment clarifying
  that the `!(regVal & (1 << 15)) == enable` expression is logically correct as
  written (C precedence applies `!` before `==` for this expression with parentheses).

### Deferred — requires design / more analysis

- `app/dtmf.c` `DTMF_FindContact` — unbounded `pResult` write; all current callers
  pass adequately-sized buffers; API change is invasive, deferred to Phase 2.
- `am_fix.c` `memmove` size error — the buggy path is under `#if !LOOKUP_TABLE`
  which is compile-time dead (`#define LOOKUP_TABLE 1`); deferred.
- `driver/uart.c` interrupt-disable scope — architectural change, needs careful design.
- `audio.c` voice queue — only active when `ENABLE_VOICE` is set (off by default).
- `app/spectrum.c` RSSI busy-wait / `driver/bk4819.c` IRQ loop — behavioural change
  risk, deferred.

---

## [0.0.0-baseline] — 2026-05-16

### Added

- Forked from
  [egzumer/uv-k5-firmware-custom](https://github.com/egzumer/uv-k5-firmware-custom)
  at commit `7607f0a` (egzumer v0.22 baseline).
- Baseline build verified on WSL2 Ubuntu 24.04 with `arm-none-eabi-gcc 13.2.1`:
  - `text = 61 292 B`, `data = 44 B`, `bss = 3 172 B`
  - Flash usage: 11.7 % of 512 KB
  - Clean build time: ~6 s

### Upstream features at baseline

- Spectrum analyzer (`F` + `5`) — fagci/Mikhail implementation
- AM fix — dynamic front-end gain control (OneOfEleven)
- FM radio receiver — BK1080 WBFM
- DTMF calling — contacts, group calls, ANI
- Configurable button functions (long-press assignments)
- RSSI bar (dBm/S-meter) and TX audio bar
- Fast channel and frequency-range scanning
- Big font frequencies, small bold channel names
- Battery voltage calibration, percentage display (1600/2200 mAh)
- Backlight dimming with configurable timeout
- SSB demodulation (adopted from fagci)
- Copy channel → VFO (`1 BAND` long press)
- Scan ranges for frequency scanning
- No CTCSS/DCS scan timeout option
- Wide RX range (18 MHz – 1300 MHz) option
- LCD interference crash fix
