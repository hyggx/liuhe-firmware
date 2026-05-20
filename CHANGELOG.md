# Changelog

All notable changes to this project are documented in this file.

Format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).  
Version scheme: `MAJOR.MINOR.PATCH[-label]` — `0.x` series is pre-release.

---

## [Unreleased]

### Feature — T9 multi-tap channel name input method (2026-05-20, `dev`)

Flash budget: `text 60 420 B / 61 440 B` (`ENABLE_CHINESE=1`, default config).

- **`app/menu.c` — T9 multi-tap IME for channel name editing** —
  Channel names can now be entered with a full T9 multi-tap input method,
  matching the ergonomics of classic mobile phone text entry.

  **Key mapping:**

  | Key | Characters |
  |-----|-----------|
  | `2` | 2 A B C |
  | `3` | 3 D E F |
  | `4` | 4 G H I |
  | `5` | 5 J K L |
  | `6` | 6 M N O |
  | `7` | 7 P Q R S |
  | `8` | 8 T U V |
  | `9` | 9 W X Y Z |
  | `1` | 1 - . |
  | `0` | 0 (space) |

  **Operation:**
  - Press the same digit repeatedly to cycle through its characters.
  - After 1.5 s idle the character is locked; press again to start a new one.
  - **Short press MENU** — confirm current character and advance cursor.
  - **Long press MENU** — save immediately from any cursor position → shows SURE? confirmation.
  - **EXIT (pos > 0)** — backspace: erase current position and step back.
  - **EXIT (pos = 0)** — cancel edit and restore the original channel name.
  - **`F` key** — toggle uppercase / lowercase (default: uppercase on entry).
  - **`*` key** — insert `-` and advance cursor.

### Fixed — FM radio audio pop/crack on channel switch (2026-05-18, `main`)

- **`audio.c` `AUDIO_PlayBeep` — BK1080 unmute sequencing corrected** —
  After each key-beep in FM radio mode a loud pop/crack was audible,
  independent of the volume knob. Root cause: BK1080's audio output feeds
  the speaker amplifier directly, bypassing BK4819's volume control; the
  prior code turned the amplifier on first and unmuted BK1080 second, so
  the amp opened onto an unstable (transitioning) analog signal and the
  transient was amplified at full hardware level.
  Fix: in FM mode the sequence is now **mute → beep → BK1080_Mute(false)
  (amp still off) → 20 ms settle → AudioPathOn**, ensuring the amplifier
  always opens onto a clean, stable signal. Non-FM code path unchanged.
  Also: `app/fm.c` `Key_UP_DOWN` mutes BK1080 and turns off the audio path
  before calling `BK1080_SetFrequency`, preventing PLL-relock noise from
  reaching the speaker during channel hops.

### UI — Settings menu overhaul (2026-05-18, `main`)

Flash budget: `text 59 912 B / 61 440 B` (`ENABLE_CHINESE=1`, default config).

- **`ui/menu.c` — top-bottom layout replaces two-column design** —
  The original left-name / right-value two-column layout is replaced by a
  top-bottom design: title bar (pages 0–1) shows the setting name in large
  font; value area (pages 2–7) shows the current value centred below.
  Eliminates Chinese label overflow and value truncation on the right edge.

- **`ui/menu.c` — title bar: name left, counter top-right** —
  Setting name is left-aligned; the `N/Total` index counter is right-aligned;
  both share page 0.  Page 1 retains the dotted separator line at its bottom
  pixel.  Prevents the counter from crossing the separator line and appearing
  on the wrong page.

- **`ui/menu.c` / `ui/helper.c` — CJK text centring in `UI_PrintStringMixed`** —
  Added a measurement pre-pass that computes the total pixel width of a
  mixed CJK/ASCII string (CJK = 13 px, ASCII = 8 px) and derives a centred
  start offset within `Start..End`.  Previously the `End` parameter was
  silently ignored in CN mode, causing all titles and values to be
  left-aligned.

- **`ui/menu.c` — edit indicator changed to `>`** —
  Replaced the filled solid-arrow bitmap (`BITMAP_CurrentIndicator`) with a
  plain `>` character rendered using the large font, shown at the right side
  of the value area (pages 2–3).  Clearer and does not obscure content.

- **`ui/menu.c` — power values: remove `~` approximation prefix** —
  `~0.5W / ~2W / ~5W` → `0.5W / 2W / 5W`.

- **`ui/menu.c` / `ui/menu_lang.c` — CTCS → CTCSS (correct abbreviation)** —
  EN menu names `RxCTCS / TxCTCS` → `RxCTCSS / TxCTCSS`;
  CN labels updated to match.
  `t_menu_item.name[]` widened from 7 to 9 bytes to accommodate longer names.

- **`ui/menu_lang.c` — Chinese menu label improvements** —

  | Before | After |
  |--------|-------|
  | 存信道 (save ch.) | 保存信道 (save channel) |
  | 删信道 (del ch.) | 删除信道 (delete channel) |
  | 名+频 (name+freq) | 名称+频率 (name+frequency) |
  | 显示语言 (display language) | 语言 (language) |
  | 偏移向 (offset dir.) | 频差方向 (freq. offset direction) |
  | 偏移频 (offset freq.) | 频差频率 (freq. offset) |
  | 忙锁 (busy lock) | 繁忙锁定 (busy lockout) |
  | 省电 (power save) | 省电模式 (power-save mode) |

- **`tools/gen_cjk_font.py` / `tools/cjk_font.bin` — font expanded to 143 glyphs** —
  Added glyphs required by updated menu labels (e.g. 保, 除, 率, 名称, …);
  binary regenerated: 4 014 bytes, 26 bytes/glyph.

- **`ui/menu.c` — 3–4 line menu items no longer overflow** —
  Multi-line items (save/delete/name channel, PTT-ID) now fit within
  pages 2–7 by reducing line spacing and adjusting the starting page.

### Chinese localisation — language audit fixes (branch: `main`)

Flash budget (2026-05-18, `ENABLE_CHINESE=1`, default config):
`text 60 860 B / 61 440 B` — **580 B free**
(`ENABLE_FLASHLIGHT` and `ENABLE_VOX` auto-disabled to fit CJK renderer).

- **[P1] `board.c` / `Makefile` — CRC peripheral not initialised for CN-only builds** —
  `CRC_Init()` in `BOARD_Init()` was guarded by
  `#if defined(ENABLE_UART) || defined(ENABLED_AIRCOPY)`.
  Two bugs: (1) `ENABLED_AIRCOPY` is a typo — the correct macro is `ENABLE_AIRCOPY`;
  (2) `ENABLE_CHINESE` was absent, so a build with
  `ENABLE_CHINESE=1 ENABLE_UART=0` would call `CRC_Calculate()` from
  `UI_PrintStringMixed` without the CRC peripheral being clocked, causing
  a fault.
  Fix: typo corrected to `ENABLE_AIRCOPY`; `defined(ENABLE_CHINESE)` added to
  the `#if` guard.  Matching fix in `Makefile`: `ENABLE_CHINESE` added to the
  `$(filter ...)` condition that selects `driver/crc.o`, ensuring the object is
  always linked for CN builds regardless of `ENABLE_UART`.

- **[P1] `ui/menu.c` — non-custom menu branch rendered Chinese titles with ASCII font** —
  `#ifndef ENABLE_CUSTOM_MENU_LAYOUT` path called `UI_PrintString(title, …, 8)`
  directly, which uses the built-in ASCII 8×8 font.  With `ENABLE_CHINESE=1` and
  `ENABLE_CUSTOM_MENU_LAYOUT=0`, all three visible menu-title rows were rendered as
  garbled ASCII glyphs.
  Fix: replaced with `SUBV_PRINT(title, …)`, which expands to
  `UI_PrintStringMixed` in CN builds (EEPROM CJK font lookup) or
  `UI_PrintString(…, 8)` in non-CN builds (existing behaviour unchanged).

- **[P2] CJK font binary — 8-byte header with magic, version, CRC-16/CCITT** —
  The font binary written to EEPROM at `0x2000` previously had no integrity
  metadata.  A blank or stale EEPROM caused `UI_PrintStringMixed` to read
  arbitrary data as a glyph table, producing garbage or out-of-bounds reads.
  New header layout (8 bytes, little-endian):

  | Offset | Field      | Value  | Notes                         |
  |--------|------------|--------|-------------------------------|
  | 0–1    | `magic`    | 0x4B36 | 'K','6' LE                    |
  | 2–3    | `count`    | n      | glyph count                   |
  | 4      | `version`  | 1      |                               |
  | 5      | `pad`      | 0      |                               |
  | 6–7    | `crc16`    | –      | CRC-16/CCITT over bytes 0–5   |

  `ui/helper.c` `UI_PrintStringMixed`: reads header, validates magic, version,
  and CRC (using `CRC_Calculate`); silently skips CJK rendering on failure.
  `tools/gen_cjk_font.py`: emits the header; software CRC matches the
  DP32G030 hardware peripheral (poly 0x1021, init 0, no reflection).
  Also updated to scan `ui/menu.c` for codepoints in addition to
  `ui/menu_lang.c` and `ui/menu_sub_values_cn.c`.

- **[P2] `app/uart.c` CMD_051B — EEPROM read boundary extended to 64 KB** —
  EEPROM constants updated: `EEPROM_SIZE = 0x10000u`, `EEPROM_FONT_BASE = 0x2000u`
  in `driver/eeprom.h`.  CMD_051B guard now checks
  `Offset + Size <= EEPROM_SIZE` (was `Offset + Size <= 0x2000`), allowing
  UART reads of the full AT24C512 address space including the font region.

- **[P2] `ui/menu_sub_values_cn.c` / `ui/menu.c` — missing Chinese sub-menu values** —
  Three sub-menu cases in `MENU_DrawValues` always displayed English strings
  regardless of the active UI language:
  - `MENU_AL_MOD`: now uses existing `gSubMenu_AL_MOD_CN[]` via `SUBV()`.
  - `MENU_F_LOCK`: added `gSubMenu_F_LOCK_CN[F_LOCK_LEN]` with translated
    band-plan labels; the "READ MANUAL" unlock-confirmation string mapped to
    "阅读\n手册".
  - `MENU_F1SHRT` / `MENU_F1LONG` / `MENU_F2SHRT` / `MENU_F2LONG` / `MENU_MLONG`
    (side-key function names): added `gSidefuncCN[ACTION_OPT_LEN]` indexed by
    `t_sidefunction.id` (`ACTION_OPT_t`), covering all 15 `ACTION_OPT_*` values.
  Extern declarations added to `ui/menu_sub_values_cn.h`.

- **[P2] `ui/helper.h` — `SUBV_PRINT` dispatch macro** —
  Added `SUBV_PRINT(str, x1, x2, line)`:
  expands to `UI_PrintStringMixed(…)` in CN builds,
  `UI_PrintString(…, 8)` in non-CN builds.
  Used throughout `ui/menu.c` to display the currently-selected menu title
  in the appropriate font without call-site `#ifdef` chains.

- **[P2] `ui/menu.c` — adjacent menu items: compile-time → runtime language check** —
  The two `while` loops that draw neighbouring (smaller-font) menu items in the
  custom layout were wrapped in `#ifndef ENABLE_CHINESE`, causing them to be
  compiled out entirely in CN builds.  Consequence: with `ENABLE_CHINESE=1` but
  `gUiLanguage == UI_LANGUAGE_EN` (English selected at runtime), the neighbouring
  items were never drawn.
  Fix: guards replaced with `#ifdef ENABLE_CHINESE if (gUiLanguage != UI_LANGUAGE_CN)`
  so adjacent items are drawn whenever the active language is not Chinese, regardless
  of how the firmware was compiled.

- **[build] `Makefile` — auto-disable `FLASHLIGHT` and `VOX` for CN builds** —
  `ENABLE_CHINESE=1` now automatically sets `ENABLE_FLASHLIGHT := 0` and
  `ENABLE_VOX := 0` to reclaim the ~1.4 KB needed by the CJK renderer and
  string tables without exceeding the 61 440 B flash limit.

- **[build] `Makefile` — link map output and clean target** —
  Added `-Wl,-Map=firmware.map` to `LDFLAGS`; `firmware.map` added to `clean` target.
  Enables precise per-symbol flash analysis via `grep` on the map file.

- **[tool] `tools/flash_font.py` — write-back verification enabled by default** —
  Verify pass (read-back and compare every written block) is now performed
  automatically after writing.  Pass `--no-verify` to skip.  Previously
  verification required the explicit `--verify` flag.

### Fixed — AM Fix bugs (branch: `k6-hardening`)

- **[P1] `radio.c` `RADIO_SetupAGC`** —
  Both `listeningAM` and `disable` parameters were shifted left by 1,
  placing them in the same bit position of `newSettings`.  The change-detect
  guard (`if lastSettings == newSettings return`) therefore ignored any
  change in `disable` when `listeningAM=1`, preventing the AGC from being
  updated when the Spectrum app toggled `lockAGC`.  Fix: `disable` now
  occupies bit 0.

- **[P1] `am_fix.c` `desired_rssi`** —
  Target RF level changed from −89 dBm to −95 dBm.  100% AM modulation
  produces sideband peaks at carrier +6 dB; targeting −89 dBm left no
  headroom for heavily-modulated broadcast audio, causing occasional
  clipping artefacts on music/full-modulation speech.  −95 dBm provides
  exactly the 6 dB headroom required.

- **[P2] `am_fix.c` `AM_fix_init`** —
  Gain table index initialised to `gain_table_size − 1` (0 dB, max gain)
  instead of 0 (the isolated "original" −7 dB entry).  Previously, on a
  signal below −95 dBm at boot, the control loop would step from index 0
  to index 1 = −93 dB (an 86 dB gain drop) before ramping back up over
  ~420 ms.  Starting at max gain means weak signals are immediately
  receivable; strong signals are attenuated on the very first 10 ms tick.

- **[P3] `am_fix.c` `AM_fix_10ms` RSSI filter** —
  Replaced the 2-tap FIR average with a first-order IIR low-pass filter
  (α = 0.25, time constant ≈ 30 ms).  The old code stored the raw RSSI
  reading as `prev_rssi` and averaged raw + raw; the new code stores the
  *smoothed* value and updates it each tick via
  `filtered += (raw − filtered) >> 2`.  Noise-spike rejection improved
  from 50% attenuation (FIR) to 75% (IIR), reducing spurious gain hunting
  on impulsive interference.

### Fixed — RSSI measurement bugs (branch: `k6-hardening`)

- **[P2] `driver/bk4819.c` `BK4819_GetRSSI_dBm`** —
  Integer division `raw / 2` always truncates toward zero, causing a
  systematic −0.5 dBm bias on every odd raw value (visible as a 1 dBm
  "staircase" where two adjacent raw readings map to the same displayed
  value).  Fix: `(raw + 1) / 2` rounds to nearest 1 dBm instead.
  Same fix applied to `Rssi2DBm()` in `app/spectrum.c`.

- **[P2] `am_fix.c` `AM_fix_10ms` `currentGainDiff` reference point** —
  The gain-compensation term added to the RSSI display was computed as
  `gain_table[0].gain_dB − current_gain_dB`, where `gain_table[0].gain_dB`
  is the "original register" entry at −7 dB.  Using −7 dB as the reference
  instead of 0 dB introduced a constant −7 dBm error in the S-meter
  whenever AM Fix was at maximum gain (no attenuation).  Fix: reference is
  now 0 dB: `currentGainDiff = −current_gain_dB`.  Result: displayed level
  is accurate at all gain steps, with zero correction error at full gain.

- **[P3] `ui/main.c` `DisplayRSSIBar` — single-sample display instability** —
  The 500 ms display update took a single point-sample of REG_67 with no
  temporal averaging.  On marginal signals or in the presence of impulse
  noise, the displayed dBm value could jump ±5–10 dBm between ticks,
  making the S-meter hard to read.  Added a first-order IIR low-pass filter
  (α = ½, τ ≈ 500 ms) on the final corrected dBm value.  Each 500 ms
  sample contributes 50% weight; the smoother reseeds immediately on VFO
  change so stale readings from the previous channel are never displayed.

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

  **Field verification (2026-05-17):**
  Tested on UV-K6 V1 with Huahong S518+ 8 cm stub antenna (lower gain than
  stock antenna; signal more likely to dwell in the squelch boundary zone).
  At ~−100 dBm (S4–S6) on a UHF simplex frequency, stock V1 firmware
  exhibited audible background noise bursts consistent with squelch chatter
  in the RSSI 25–40 hysteresis-inversion window.  Hygg firmware (this fix)
  produced clean, stable audio under identical conditions.  Hardware and
  antenna unchanged between tests; improvement confirmed as firmware-only.

### Ham Core — Phase 4a (branch: `k6-hardening`, commit: `46eca1c`)

Flash budget (2026-05-17 measured, current config):
`text 57528 B / 61440 B` — **3912 B free**.

- **[ham] `settings.c` — S-meter IARU R1 default calibration** —
  Changed default S0/S9 levels from 130/76 to **127/73** to comply with IARU
  Region 1 Technical Recommendation R.1: S9 = −73 dBm, 6 dB per S-unit,
  giving S0 = −127 dBm.  The 54 dBm / 9 S-unit span is unchanged; only
  the absolute S9 reference is corrected by 3 dBm.  Takes effect on factory
  reset or first boot with uninitialised EEPROM.

- **[fix] `ui/main.c` — `ENABLE_RSSI_BAR=0` build failure** —
  Static IIR state variables (`rssi_disp_dBm`, `rssi_disp_seeded`,
  `rssi_disp_vfo`) were declared outside the `#ifdef ENABLE_RSSI_BAR` guard,
  causing `-Werror=unused-variable` when the feature was disabled.  Wrapped
  in the appropriate `#ifdef`; all build configurations now compile cleanly.

- **[security] `app/uart.c` `SendVersion` — bounded version string copy** —
  Replaced `strcpy(Reply.Data.Version, Version)` with
  `strncpy(..., sizeof − 1)` plus explicit null termination.  Eliminates
  the latent buffer overflow if `AUTHOR_STRING` or `VERSION_STRING` is ever
  lengthened beyond 14 characters.

### Config — FM radio enabled, DTMF calling removed (branch: `k6-hardening`, commit: `f4293a0`)

Flash budget (2026-05-17 measured, field-verified):
`text 58960 B / 61440 B` — **2480 B free**.

- **[config] `Makefile` — `ENABLE_FMRADIO=1`** —
  FM WBFM broadcast receiver re-enabled (was disabled at project start).
  Adds `driver/bk1080.o`, `app/fm.o`, `ui/fmradio.o` — measured cost
  ~4.7 KB, not the originally estimated 1 KB.

- **[config] `Makefile` — `ENABLE_DTMF_CALLING=0`** —
  DTMF calling protocol disabled to reclaim 3300 B needed to fit FM radio.
  This removes the ANI-ID / contact-list / ring-reply call protocol only.
  The following DTMF features are **unaffected**: manual code entry before
  PTT (repeater access), PTT-ID auto UP/DOWN code, DTMF side-tone, preload
  time, and live DTMF decoder display.  Menu item count: 60 → **55** items
  (removed: `ANI ID`, `D Resp`, `D Hold`, `D Decd`, `D List`).

  **Field verification (2026-05-17):** FM reception, all remaining menu
  items, VFO/MR operation, and PTT-ID confirmed working on UV-K6 V1 hardware.

### TX lock default + build profiles (branch: `k6-hardening`)

- **[hardening] `settings.c` `SETTINGS_InitEEPROM`** —
  TX-frequency lock fallback on invalid/blank EEPROM changed from
  `F_LOCK_DEF` (permissive stock bands) to `DEFAULT_F_LOCK`, a
  compile-time constant.  This value is `F_LOCK_ALL` (all TX disabled)
  for the release profile and `F_LOCK_DEF` for the debug profile.
  Existing user EEPROM values are **never overwritten** on normal boot;
  the fallback only applies when `Data[0] >= F_LOCK_LEN` (0xFF on
  brand-new or blank EEPROM).  TX lock can still be changed via the
  hidden menu (PTT + upper side-key at power-on → `F LOCK`).

- **[build] `Makefile` — `PROFILE` variable** —
  Added profile-based build system.  `PROFILE ?= release` selects
  `profiles/release.mk` by default; override with `make PROFILE=debug`.

  | Profile | `DEFAULT_F_LOCK` | debug flags | UART BK regs |
  |---------|-----------------|-------------|--------------|
  | release | `F_LOCK_ALL`    | off         | off          |
  | debug   | `F_LOCK_DEF`    | on          | on           |

  Profile files use `:=` so they take precedence over any subsequent
  `?=` defaults in the main Makefile.  Flash budget is identical for
  both profiles (`text 58960 B`); the debug flags add negligible code
  with LTO.

### Security — Phase 1 (branch: `k6-hardening`)

Confirmed fixes applied.  Flash budget note: the `text 61396 B` figure below
was measured with `ENABLE_FMRADIO=1`; current config (`FMRADIO=0`) measures
`text 57428 B / 61440 B` (3912 B free before the Phase 4a commit above).

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

### Fixed — AM Fix follow-up bugs (self-audit, branch: `k6-hardening`)

- **[P2] `am_fix.c` `AM_fix_reset` — gain index not reset to max gain** —
  `AM_fix_init()` was corrected (prior commit) to start at `gain_table_size − 1`
  (0 dB, maximum gain), but `AM_fix_reset()` — called on every frequency change —
  still left `gain_table_index[vfo]` at whatever the previous channel had set it to.
  Tuning from a strong AM station (where the AGC had reduced the index to a low-gain
  value) to a weak station caused ~400 ms of near-silence while the AGC ramped the
  index back up to maximum.  Fix: `AM_fix_reset()` now also sets
  `gain_table_index[vfo] = gain_table_size − 1`.

- **[P2] `am_fix.c` `currentGainDiff` — single global, not per-VFO** —
  `currentGainDiff` was a single `int8_t` updated by `AM_fix_10ms(vfo)` for whichever
  VFO was last processed.  In dual-watch mode with both VFOs on AM, the S-meter in
  `DisplayRSSIBar` would apply the gain compensation from the *last-processed* VFO
  rather than the *currently-displayed* VFO, producing an incorrect dBm reading.
  Fix: changed to `int8_t currentGainDiff[2]` (indexed by VFO); `AM_fix_get_gain_diff`
  now takes an `unsigned vfo` parameter.  Call sites updated:
  `ui/main.c` passes `gEeprom.RX_VFO`; `app/spectrum.c` passes the module-scope `vfo`.

- **[P3] `driver/bk4819.c` comment arithmetic error** —
  The `BK4819_GetRSSI_dBm` documentation comment stated
  `raw 511 → −104.5 dBm`, but `(511 + 1) / 2 − 160 = +96 dBm`.
  Corrected to `raw 130 → −95 dBm`, which matches the AM Fix AGC target and is
  easily verified: `130 × 0.5 − 160 = −95 dBm`.

### Scanner — Priority-channel insertion rate reduced (commit: `f26854a`)

- **[fix] `app/chFrScanner.c` — `SCAN_PRIORITY_INTERVAL`** —
  Priority-channel check was inserted after every scanned channel
  (`SCAN_PRIORITY_INTERVAL = 1`).  On a busy band this caused the
  scanner to stall on the priority channel entry for every step, making
  forward progress nearly imperceptible.  Changed to
  `SCAN_PRIORITY_INTERVAL = 8`: the priority check fires once every
  8 scanned channels, reducing interruptions by 87.5 % while still
  giving the priority channel roughly one check per second at normal
  scan speeds.

### Fixed — 6.25 kHz bandwidth exposed in W_N menu + weak-signal filter bug (commit: `3402875`)

Flash budget (2026-05-18 measured):
`text 59 000 B / 61 440 B` — **2 440 B free**.

- **[fix] `ui/menu.c` / `ui/menu.h` — `gSubMenu_W_N` missing NARROWER entry** —
  The `BK4819_FILTER_BW_NARROWER` (6.25 kHz) enum value was already
  handled in `bk4819.c` and used in `app/spectrum.c`, but the W_N menu
  array was declared as `[2][7]` and contained only `{"WIDE", "NARROW"}`.
  Any code that wrote `NARROWER` to EEPROM would read back a display of
  `WIDE` (index 0 wrap), silently discarding the user's choice.
  Fix: array extended to `[3][7]`; third entry added as `"NARRW"` (truncated
  to fit the 6-character field width).  Users can now select 6.25 kHz via
  the menu; value is stored and restored correctly across power cycles.

- **[P2] `driver/bk4819.c` `BK4819_SetFilterBandwidth` NARROWER weak-signal path** —
  In the `BK4819_FILTER_BW_NARROWER` case, the `val` initialiser included
  `(3u << 9)` unconditionally.  The `if (weak_no_different)` branch then
  OR'd `(3u << 9)` again (no-op), and the `else` branch OR'd `(0u << 9)`
  (also a no-op).  Both paths therefore produced the identical register
  value `0x3658`, making the "weak signal" variant of NARROWER identical
  to the normal variant — the bits[11:9] field was never cleared.
  Fix: `(3u << 9)` removed from the initialiser; the strong-signal path
  OR's it in to produce `0x3658`; the weak-signal path leaves bits[11:9]
  at zero, producing `0x3058` — the correct lower-bandwidth setting for
  weak-signal AM reception.

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
