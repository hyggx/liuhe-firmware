# profiles/debug.mk — development / lab defaults
# Included by Makefile when PROFILE=debug (make PROFILE=debug).
# Uses := (immediate assignment) so these values take precedence over the
# ?= defaults in the main Makefile.

# --- TX lock: permissive for bench testing ---
# Reverts to F_LOCK_DEF (stock default bands) on blank EEPROM.
DEFAULT_F_LOCK          := F_LOCK_DEF

# --- Debug output: on ---
ENABLE_AM_FIX_SHOW_DATA := 1
ENABLE_AGC_SHOW_DATA    := 1

# --- Debug hardware interfaces: on ---
ENABLE_UART_RW_BK_REGS  := 1
ENABLE_SWD              := 0
