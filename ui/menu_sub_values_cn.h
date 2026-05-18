/* Copyright 2024 egzumer / Hygg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Simplified Chinese submenu value string arrays.
 * Enabled by compiling with -DENABLE_CHINESE.
 */

#ifndef UI_MENU_SUB_VALUES_CN_H
#define UI_MENU_SUB_VALUES_CN_H

#ifdef ENABLE_CHINESE

#include <stdint.h>

/* Maximum bytes per submenu-value string (UTF-8, up to 4 CJK × 3 bytes + NUL) */
#define CN_MAX  13

extern const char gSubMenu_TXP_CN[3][CN_MAX];
extern const char gSubMenu_SFT_D_CN[3][CN_MAX];
extern const char gSubMenu_W_N_CN[3][CN_MAX];
extern const char gSubMenu_OFF_ON_CN[2][CN_MAX];
extern const char gSubMenu_SAVE_CN[5][CN_MAX];
extern const char gSubMenu_TOT_CN[11][CN_MAX];
extern const char gSubMenu_RXMode_CN[4][10];
extern const char gSubMenu_SC_REV_CN[3][CN_MAX];
extern const char gSubMenu_MDF_CN[4][10];
extern const char gSubMenu_PONMSG_CN[4][CN_MAX];
extern const char gSubMenu_ROGER_CN[3][CN_MAX];
extern const char gSubMenu_RESET_CN[2][CN_MAX];
extern const char gSubMenu_BACKLIGHT_CN[8][CN_MAX];
extern const char gSubMenu_RX_TX_CN[4][CN_MAX];
extern const char gSubMenu_BAT_TXT_CN[3][CN_MAX];
extern const char gSubMenu_BATTYP_CN[2][CN_MAX];
extern const char gSubMenu_LANGUAGE[2][8];

#ifdef ENABLE_ALARM
extern const char gSubMenu_AL_MOD_CN[2][CN_MAX];
#endif

/* ---- TX frequency lock ---- */
extern const char * const gSubMenu_F_LOCK_CN[];

/* ---- Side-key function names, indexed by ACTION_OPT_t value ---- */
extern const char * const gSidefuncCN[];

#endif  /* ENABLE_CHINESE */
#endif  /* UI_MENU_SUB_VALUES_CN_H */
