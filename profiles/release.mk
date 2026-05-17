# profiles/release.mk — field-safe shipping defaults
# Included by Makefile when PROFILE=release (the default).
# Uses := (immediate assignment) so these values take precedence over the
# ?= defaults in the main Makefile.

# --- TX lock: all TX disabled on blank/new EEPROM ---
# Users can unlock via the hidden menu (PTT + upper sidekey at power-on).
DEFAULT_F_LOCK          := F_LOCK_ALL

# --- Debug output: off ---
ENABLE_AM_FIX_SHOW_DATA := 0
ENABLE_AGC_SHOW_DATA    := 0

# --- Debug hardware interfaces: off ---
ENABLE_UART_RW_BK_REGS  := 0
ENABLE_SWD              := 0
