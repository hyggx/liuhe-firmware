/* Copyright 2024 egzumer / Hygg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Simplified Chinese submenu value strings.
 * All CJK characters here must be present in the font stored at EEPROM 0x2000.
 * Run  tools/gen_cjk_font.py  to regenerate the font binary.
 */

#ifdef ENABLE_CHINESE

#include "ui/menu_sub_values_cn.h"
#include "settings.h"  /* F_LOCK_LEN, ACTION_OPT_LEN */

/* ---- TX power ---- */
/* 低 中 高 */
const char gSubMenu_TXP_CN[3][CN_MAX] = {
    "\xe4\xbd\x8e",            /* 低 */
    "\xe4\xb8\xad",            /* 中 */
    "\xe9\xab\x98",            /* 高 */
};

/* ---- TX offset direction ---- */
/* 关  +方向  -方向 */
const char gSubMenu_SFT_D_CN[3][CN_MAX] = {
    "\xe5\x85\xb3",            /* 关 */
    "+\xe6\x96\xb9\xe5\x90\x91", /* +方向 */
    "-\xe6\x96\xb9\xe5\x90\x91", /* -方向 */
};

/* ---- Bandwidth ---- */
/* 宽  窄  超窄 */
const char gSubMenu_W_N_CN[3][CN_MAX] = {
    "\xe5\xae\xbd",            /* 宽 */
    "\xe7\xaa\x84",            /* 窄 */
    "\xe8\xb6\x85\xe7\xaa\x84", /* 超窄 */
};

/* ---- Generic OFF/ON ---- */
/* 关  开 */
const char gSubMenu_OFF_ON_CN[2][CN_MAX] = {
    "\xe5\x85\xb3",            /* 关 */
    "\xe5\xbc\x80",            /* 开 */
};

/* ---- Battery save ratios ---- */
/* 关  1:1  1:2  1:3  1:4 */
const char gSubMenu_SAVE_CN[5][CN_MAX] = {
    "\xe5\x85\xb3",            /* 关 */
    "1:1",
    "1:2",
    "1:3",
    "1:4",
};

/* ---- TX timeout ---- */
/* 30秒 … 15分 */
const char gSubMenu_TOT_CN[11][CN_MAX] = {
    "30\xe7\xa7\x92",          /* 30秒 */
    "1\xe5\x88\x86",           /* 1分  */
    "2\xe5\x88\x86",           /* 2分  */
    "3\xe5\x88\x86",           /* 3分  */
    "4\xe5\x88\x86",           /* 4分  */
    "5\xe5\x88\x86",           /* 5分  */
    "6\xe5\x88\x86",           /* 6分  */
    "7\xe5\x88\x86",           /* 7分  */
    "8\xe5\x88\x86",           /* 8分  */
    "9\xe5\x88\x86",           /* 9分  */
    "15\xe5\x88\x86",          /* 15分 */
};

/* ---- RX mode (dual-watch) ---- */
/* 仅主频  双守  跨频段  主发收 */
const char gSubMenu_RXMode_CN[4][10] = {
    "\xe4\xbb\x85\xe4\xb8\xbb\xe9\xa2\x91", /* 仅主频 */
    "\xe5\x8f\x8c\xe5\xae\x88",            /* 双守   */
    "\xe8\xb7\xa8\xe9\xa2\x91\xe6\xae\xb5", /* 跨频段 */
    "\xe4\xb8\xbb\xe5\x8f\x91\xe6\x94\xb6", /* 主发收 */
};

/* ---- Scan resume ---- */
/* 超时  载波  停止 */
const char gSubMenu_SC_REV_CN[3][CN_MAX] = {
    "\xe8\xb6\x85\xe6\x97\xb6", /* 超时 */
    "\xe8\xbd\xbd\xe6\xb3\xa2", /* 载波 */
    "\xe5\x81\x9c\xe6\xad\xa2", /* 停止 */
};

/* ---- Channel display mode ---- */
/* 频率  信道号  名称  名+频 */
const char gSubMenu_MDF_CN[4][16] = {
    "\xe9\xa2\x91\xe7\x8e\x87",                              /* 频率     */
    "\xe4\xbf\xa1\xe9\x81\x93\xe5\x8f\xb7",                  /* 信道号   */
    "\xe5\x90\x8d\xe7\xa7\xb0",                              /* 名称     */
    "\xe5\x90\x8d\xe7\xa7\xb0\x2b\xe9\xa2\x91\xe7\x8e\x87", /* 名称+频率 */
};

/* ---- Power-on message ---- */
/* 全屏  消息  电压  无 */
const char gSubMenu_PONMSG_CN[4][CN_MAX] = {
    "\xe5\x85\xa8\xe5\xb1\x8f", /* 全屏 */
    "\xe6\xb6\x88\xe6\x81\xaf", /* 消息 */
    "\xe7\x94\xb5\xe5\x8e\x8b", /* 电压 */
    "\xe6\x97\xa0",             /* 无   */
};

/* ---- Roger beep ---- */
/* 关  Roger  MDC */
const char gSubMenu_ROGER_CN[3][CN_MAX] = {
    "\xe5\x85\xb3",            /* 关 */
    "Roger",
    "MDC",
};

/* ---- Factory reset ---- */
/* VFO  全部 */
const char gSubMenu_RESET_CN[2][CN_MAX] = {
    "VFO",
    "\xe5\x85\xa8\xe9\x83\xa8", /* 全部 */
};

/* ---- Backlight timeout ---- */
/* 关  5秒  10秒  20秒  1分  2分  4分  常亮 */
const char gSubMenu_BACKLIGHT_CN[8][CN_MAX] = {
    "\xe5\x85\xb3",                  /* 关   */
    "5\xe7\xa7\x92",                 /* 5秒  */
    "10\xe7\xa7\x92",                /* 10秒 */
    "20\xe7\xa7\x92",                /* 20秒 */
    "1\xe5\x88\x86",                 /* 1分  */
    "2\xe5\x88\x86",                 /* 2分  */
    "4\xe5\x88\x86",                 /* 4分  */
    "\xe5\xb8\xb8\xe4\xba\xae",      /* 常亮 */
};

/* ---- ABR on RX/TX ---- */
/* 关  发射  接收  收发 */
const char gSubMenu_RX_TX_CN[4][CN_MAX] = {
    "\xe5\x85\xb3",            /* 关 */
    "\xe5\x8f\x91\xe5\xb0\x84", /* 发射 (TX) */
    "\xe6\x8e\xa5\xe6\x94\xb6", /* 接收 (RX) */
    "\xe6\x94\xb6\xe5\x8f\x91", /* 收发 (TX/RX) */
};

/* ---- Battery text display ---- */
/* 关  电压V  电量% */
const char gSubMenu_BAT_TXT_CN[3][CN_MAX] = {
    "\xe5\x85\xb3",                       /* 关    (NONE)    */
    "\xe7\x94\xb5\xe5\x8e\x8b""V",        /* 电压V (VOLTAGE) */
    "\xe7\x94\xb5\xe9\x87\x8f%",          /* 电量% (PERCENT) */
};

/* ---- Battery type ---- */
/* 1600mAh  2200mAh */
const char gSubMenu_BATTYP_CN[2][CN_MAX] = {
    "1600mAh",
    "2200mAh",
};

/* ---- Language selection (shared with EN build) ---- */
const char gSubMenu_LANGUAGE[2][8] = {
    "English",
    "\xe4\xb8\xad\xe6\x96\x87",  /* 中文 */
};

#ifdef ENABLE_ALARM
/* ---- Alarm mode ---- */
/* 本地  音调 */
const char gSubMenu_AL_MOD_CN[2][CN_MAX] = {
    "\xe6\x9c\xac\xe5\x9c\xb0", /* 本地 */
    "\xe9\x9f\xb3\xe8\xb0\x83", /* 音调 */
};
#endif

/* ---- TX frequency lock ---- */
/* 默认+  业余  禁用  解锁  全部 */
const char * const gSubMenu_F_LOCK_CN[F_LOCK_LEN] = {
    "\xe9\xbb\x98\xe8\xae\xa4+\n137-174\n400-470",    /* 默认+ */
    "FCC\xe4\xb8\x9a\xe4\xbd\x99\n144-148\n420-450",  /* FCC业余 */
    "CE\xe4\xb8\x9a\xe4\xbd\x99\n144-146\n430-440",   /* CE业余 */
    "GB\xe4\xb8\x9a\xe4\xbd\x99\n144-148\n430-440",   /* GB业余 */
    "137-174\n400-430",
    "137-174\n400-438",
    "\xe7\xa6\x81\xe7\x94\xa8\n\xe5\x85\xa8\xe9\x83\xa8",  /* 禁用\n全部 */
    "\xe8\xa7\xa3\xe9\x94\x81\n\xe5\x85\xa8\xe9\x83\xa8",  /* 解锁\n全部 */
};

/* ---- Side-key function names, indexed by ACTION_OPT_t value ----
 * Slots for features disabled in the CN build (FLASHLIGHT, VOX) are never
 * displayed but must occupy the correct index position. */
/* 无 手电 功率 监听 扫描 VOX 报警 调频 锁键 切频 信道 解调 背光 频谱 */
const char * const gSidefuncCN[ACTION_OPT_LEN] = {
    [ACTION_OPT_NONE]           = "\xe6\x97\xa0",              /* 无 */
    [ACTION_OPT_FLASHLIGHT]     = "\xe6\x89\x8b\xe7\x94\xb5",  /* 手电 */
    [ACTION_OPT_POWER]          = "\xe5\x8a\x9f\xe7\x8e\x87",  /* 功率 */
    [ACTION_OPT_MONITOR]        = "\xe7\x9b\x91\xe5\x90\xac",  /* 监听 */
    [ACTION_OPT_SCAN]           = "\xe6\x89\xab\xe6\x8f\x8f",  /* 扫描 */
    [ACTION_OPT_VOX]            = "VOX",
    [ACTION_OPT_ALARM]          = "\xe6\x8a\xa5\xe8\xad\xa6",  /* 报警 */
    [ACTION_OPT_FM]             = "\xe8\xb0\x83\xe9\xa2\x91",  /* 调频 */
    [ACTION_OPT_1750]           = "1750",
    [ACTION_OPT_KEYLOCK]        = "\xe9\x94\x81\xe9\x94\xae",  /* 锁键 */
    [ACTION_OPT_A_B]            = "\xe5\x88\x87\xe9\xa2\x91",  /* 切频 */
    [ACTION_OPT_VFO_MR]         = "\xe4\xbf\xa1\xe9\x81\x93",  /* 信道 */
    [ACTION_OPT_SWITCH_DEMODUL] = "\xe8\xa7\xa3\xe8\xb0\x83",  /* 解调 */
    [ACTION_OPT_BLMIN_TMP_OFF]  = "\xe8\x83\x8c\xe5\x85\x89",  /* 背光 */
    [ACTION_OPT_SPECTRUM]       = "\xe9\xa2\x91\xe8\xb0\xb1",  /* 频谱 */
};

#endif  /* ENABLE_CHINESE */
