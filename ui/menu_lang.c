/* Copyright 2024 egzumer / Hygg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Localised menu-title strings for Simplified Chinese (UTF-8).
 * All CJK strings reference characters that must be present in the
 * CJK bitmap font written to AT24C512 EEPROM at 0x2000.
 * Run  tools/gen_cjk_font.py  to generate the font binary.
 */

#ifdef ENABLE_CHINESE

#include "settings.h"   /* gUiLanguage, UI_LANGUAGE_CN */
#include "ui/menu.h"    /* t_menu_item, MENU_* enum */

/* ---------------------------------------------------------------------------
 * Chinese menu title strings (2-4 characters, UTF-8 encoded).
 *
 * Character format:  "\\xHH\\xHH\\xHH"  per CJK character (3 bytes each).
 * ---------------------------------------------------------------------------
 */

/* Visible items */
/* 静噪  */ static const char cn_SQL[]       = "\xe9\x9d\x99\xe5\x99\xaa";
/* 步进频率 */static const char cn_STEP[]      = "\xe6\xad\xa5\xe8\xbf\x9b\xe9\xa2\x91\xe7\x8e\x87";
/* 发射功率 */static const char cn_TXP[]       = "\xe5\x8f\x91\xe5\xb0\x84\xe5\x8a\x9f\xe7\x8e\x87";
/* 接收数字亚音 */static const char cn_R_DCS[]  = "\xe6\x8e\xa5\xe6\x94\xb6\xe6\x95\xb0\xe5\xad\x97\xe4\xba\x9a\xe9\x9f\xb3";
/* 接收模拟亚音 */static const char cn_R_CTCS[] = "\xe6\x8e\xa5\xe6\x94\xb6\xe6\xa8\xa1\xe6\x8b\x9f\xe4\xba\x9a\xe9\x9f\xb3";
/* 发射数字亚音 */static const char cn_T_DCS[]  = "\xe5\x8f\x91\xe5\xb0\x84\xe6\x95\xb0\xe5\xad\x97\xe4\xba\x9a\xe9\x9f\xb3";
/* 发射模拟亚音 */static const char cn_T_CTCS[] = "\xe5\x8f\x91\xe5\xb0\x84\xe6\xa8\xa1\xe6\x8b\x9f\xe4\xba\x9a\xe9\x9f\xb3";
/* 频差方向 */static const char cn_SFT_D[]   = "\xe9\xa2\x91\xe5\xb7\xae\xe6\x96\xb9\xe5\x90\x91";
/* 频差频率 */static const char cn_OFFSET[]  = "\xe9\xa2\x91\xe5\xb7\xae\xe9\xa2\x91\xe7\x8e\x87";
/* 发射超时 */static const char cn_TOT[]      = "\xe5\x8f\x91\xe5\xb0\x84\xe8\xb6\x85\xe6\x97\xb6";
/* 信道带宽 */static const char cn_W_N[]      = "\xe4\xbf\xa1\xe9\x81\x93\xe5\xb8\xa6\xe5\xae\xbd";
/* 加密通话 */static const char cn_SCR[]      = "\xe5\x8a\xa0\xe5\xaf\x86\xe9\x80\x9a\xe8\xaf\x9d";
/* 遇忙禁发 */static const char cn_BCL[]      = "\xe9\x81\x87\xe5\xbf\x99\xe7\xa6\x81\xe5\x8f\x91";
/* 存储信道 */static const char cn_MEM_CH[]  = "\xe5\xad\x98\xe5\x82\xa8\xe4\xbf\xa1\xe9\x81\x93";
/* 删除信道 */static const char cn_DEL_CH[]  = "\xe5\x88\xa0\xe9\x99\xa4\xe4\xbf\xa1\xe9\x81\x93";
/* 编辑名称 */static const char cn_MEM_NAME[]= "\xe7\xbc\x96\xe8\xbe\x91\xe5\x90\x8d\xe7\xa7\xb0";
/* 信道显示 */static const char cn_MDF[]     = "\xe4\xbf\xa1\xe9\x81\x93\xe6\x98\xbe\xe7\xa4\xba";
/* 省电模式 */static const char cn_SAVE[]     = "\xe7\x9c\x81\xe7\x94\xb5\xe6\xa8\xa1\xe5\xbc\x8f";
/* VOX   */
#ifdef ENABLE_VOX
static const char cn_VOX[]      = "VOX";
#endif
/* 自动背光 */static const char cn_ABR[]      = "\xe8\x87\xaa\xe5\x8a\xa8\xe8\x83\x8c\xe5\x85\x89";
/* 收发背光 */static const char cn_ABR_TRX[] = "\xe6\x94\xb6\xe5\x8f\x91\xe8\x83\x8c\xe5\x85\x89";
/* 背光最暗 */static const char cn_ABR_MIN[] = "\xe8\x83\x8c\xe5\x85\x89\xe6\x9c\x80\xe6\x9a\x97";
/* 背光最亮 */static const char cn_ABR_MAX[] = "\xe8\x83\x8c\xe5\x85\x89\xe6\x9c\x80\xe4\xba\xae";
/* 双守模式 */static const char cn_TDR[]      = "\xe5\x8f\x8c\xe5\xae\x88\xe6\xa8\xa1\xe5\xbc\x8f";
/* 按键音  */static const char cn_BEEP[]    = "\xe6\x8c\x89\xe9\x94\xae\xe9\x9f\xb3";
/* 语音  */ 
#ifdef ENABLE_VOICE
static const char cn_VOICE[]    = "\xe8\xaf\xad\xe9\x9f\xb3\xe6\x8f\x90\xe7\xa4\xba";
#endif
/* 扫描恢复 */static const char cn_SC_REV[]  = "\xe6\x89\xab\xe6\x8f\x8f\xe6\x81\xa2\xe5\xa4\x8d";
/* 自动锁键 */static const char cn_AUTOLK[]  = "\xe8\x87\xaa\xe5\x8a\xa8\xe9\x94\x81\xe9\x94\xae";
/* 加入扫描1 */static const char cn_S_ADD1[]  = "\xe5\x8a\xa0\xe5\x85\xa5\xe6\x89\xab\xe6\x8f\x8f\x31";
/* 加入扫描2 */static const char cn_S_ADD2[]  = "\xe5\x8a\xa0\xe5\x85\xa5\xe6\x89\xab\xe6\x8f\x8f\x32";
/* 尾音消除 */static const char cn_STE[]     = "\xe5\xb0\xbe\xe9\x9f\xb3\xe6\xb6\x88\xe9\x99\xa4";
/* 中继尾音 */static const char cn_RP_STE[]  = "\xe4\xb8\xad\xe7\xbb\xa7\xe5\xb0\xbe\xe9\x9f\xb3";
/* 麦克增益 */static const char cn_MIC[]     = "\xe9\xba\xa6\xe5\x85\x8b\xe5\xa2\x9e\xe7\x9b\x8a";
/* 话音电平 */static const char cn_MIC_BAR[] = "\xe8\xaf\x9d\xe9\x9f\xb3\xe7\x94\xb5\xe5\xb9\xb3";
/* 语音压扩 */static const char cn_COMPAND[]  = "\xe8\xaf\xad\xe9\x9f\xb3\xe5\x8e\x8b\xe6\x89\xa9";
/* 一键即呼 */static const char cn_1_CALL[]  = "\xe4\xb8\x80\xe9\x94\xae\xe5\x8d\xb3\xe5\x91\xbc";
/* 扫描列表 */static const char cn_S_LIST[]  = "\xe6\x89\xab\xe6\x8f\x8f\xe5\x88\x97\xe8\xa1\xa8";
/* 扫描列表1 */static const char cn_SLIST1[]  = "\xe6\x89\xab\xe6\x8f\x8f\xe5\x88\x97\xe8\xa1\xa8\x31";
/* 扫描列表2 */static const char cn_SLIST2[]  = "\xe6\x89\xab\xe6\x8f\x8f\xe5\x88\x97\xe8\xa1\xa8\x32";
/* 调制模式 */static const char cn_AM[]       = "\xe8\xb0\x83\xe5\x88\xb6\xe6\xa8\xa1\xe5\xbc\x8f";
/* AM修正 */ static const char cn_AM_FIX[]  = "AM\xe4\xbf\xae\xe6\xad\xa3";
/* NOAA  */
#ifdef ENABLE_NOAA
static const char cn_NOAA_S[]   = "NOAA";
#endif
/* DTMF上线码 */static const char cn_UPCODE[]  = "DTMF\xe4\xb8\x8a\xe7\xba\xbf\xe7\xa0\x81";
/* DTMF下线码 */static const char cn_DWCODE[]  = "DTMF\xe4\xb8\x8b\xe7\xba\xbf\xe7\xa0\x81";
/* PTT ID*/ static const char cn_PTT_ID[]   = "PTT ID";
/* DTMF侧音 */static const char cn_D_ST[]   = "DTMF\xe4\xbe\xa7\xe9\x9f\xb3";
/* DTMF响应 */
#ifdef ENABLE_DTMF_CALLING
static const char cn_D_RSP[]  = "DTMF\xe5\x93\x8d\xe5\xba\x94";
#endif
/* DTMF保持 */
#ifdef ENABLE_DTMF_CALLING
static const char cn_D_HOLD[] = "DTMF\xe4\xbf\x9d\xe6\x8c\x81";
#endif
/* DTMF前导 */static const char cn_D_PRE[]  = "DTMF\xe5\x89\x8d\xe5\xaf\xbc";
/* DTMF解码 */
#ifdef ENABLE_DTMF_CALLING
static const char cn_D_DCD[]  = "DTMF\xe8\xa7\xa3\xe7\xa0\x81";
#endif
/* DTMF联系 */
#ifdef ENABLE_DTMF_CALLING
static const char cn_D_LIST[] = "DTMF\xe8\x81\x94\xe7\xb3\xbb";
#endif
/* DTMF直解 */static const char cn_D_LIVE[] = "DTMF\xe7\x9b\xb4\xe8\xa7\xa3";
/* 开机信息 */static const char cn_PONMSG[]  = "\xe5\xbc\x80\xe6\x9c\xba\xe4\xbf\xa1\xe6\x81\xaf";
/* 发送结束音 */static const char cn_ROGER[]   = "\xe5\x8f\x91\xe9\x80\x81\xe7\xbb\x93\xe6\x9d\x9f\xe9\x9f\xb3";
/* 电池电压 */static const char cn_VOL[]      = "\xe7\x94\xb5\xe6\xb1\xa0\xe7\x94\xb5\xe5\x8e\x8b";
/* 电池显示 */static const char cn_BAT_TXT[] = "\xe7\x94\xb5\xe6\xb1\xa0\xe6\x98\xbe\xe7\xa4\xba";
/* 报警  */
#ifdef ENABLE_ALARM
static const char cn_AL_MOD[]   = "\xe6\x8a\xa5\xe8\xad\xa6";
#endif
/* 语言     */static const char cn_LANGUAGE[] = "\xe8\xaf\xad\xe8\xa8\x80";
/* 恢复出厂 */static const char cn_RESET[]    = "\xe6\x81\xa2\xe5\xa4\x8d\xe5\x87\xba\xe5\x8e\x82";
/* Hidden items */
/* 锁定频段 */static const char cn_F_LOCK[]  = "\xe9\x94\x81\xe5\xae\x9a\xe9\xa2\x91\xe6\xae\xb5";
/* Tx200 */ static const char cn_200TX[]    = "Tx200";
/* Tx350 */ static const char cn_350TX[]    = "Tx350";
/* Tx500 */ static const char cn_500TX[]    = "Tx500";
/* 350启用 */static const char cn_350EN[]    = "\x33\x35\x30\xe5\x90\x81\xe7\x94\xa8";
/* 加密启用 */static const char cn_SCREN[]   = "\xe5\x8a\xa0\xe5\xaf\x86\xe5\x90\x81\xe7\x94\xa8";
/* 频率校准 */
#ifdef ENABLE_F_CAL_MENU
static const char cn_F_CALI[]  = "\xe9\xa2\x91\xe7\x8e\x87\xe6\xa0\xa1\xe5\x87\x86";
#endif
/* 电池校准 */static const char cn_BATCAL[]  = "\xe7\x94\xb5\xe6\xb1\xa0\xe6\xa0\xa1\xe5\x87\x86";
/* 键1短按 */static const char cn_F1SHRT[]  = "\xe9\x94\xae\x31\xe7\x9f\xad\xe6\x8c\x89";
/* 键1长按 */static const char cn_F1LONG[]  = "\xe9\x94\xae\x31\xe9\x95\xbf\xe6\x8c\x89";
/* 键2短按 */static const char cn_F2SHRT[]  = "\xe9\x94\xae\x32\xe7\x9f\xad\xe6\x8c\x89";
/* 键2长按 */static const char cn_F2LONG[]  = "\xe9\x94\xae\x32\xe9\x95\xbf\xe6\x8c\x89";
/* 菜单长按 */static const char cn_MLONG[]   = "\xe8\x8f\x9c\xe5\x8d\x95\xe9\x95\xbf\xe6\x8c\x89";
/* 电池类型 */static const char cn_BATTYP[]  = "\xe7\x94\xb5\xe6\xb1\xa0\xe7\xb1\xbb\xe5\x9e\x8b";

const char *UI_MENU_GetMenuTitle(const t_menu_item *item)
{
	if (gUiLanguage != UI_LANGUAGE_CN)
		return item->name;

	switch (item->menu_id) {
		case MENU_SQL:       return cn_SQL;
		case MENU_STEP:      return cn_STEP;
		case MENU_TXP:       return cn_TXP;
		case MENU_R_DCS:     return cn_R_DCS;
		case MENU_R_CTCS:    return cn_R_CTCS;
		case MENU_T_DCS:     return cn_T_DCS;
		case MENU_T_CTCS:    return cn_T_CTCS;
		case MENU_SFT_D:     return cn_SFT_D;
		case MENU_OFFSET:    return cn_OFFSET;
		case MENU_TOT:       return cn_TOT;
		case MENU_W_N:       return cn_W_N;
		case MENU_SCR:       return cn_SCR;
		case MENU_BCL:       return cn_BCL;
		case MENU_MEM_CH:    return cn_MEM_CH;
		case MENU_DEL_CH:    return cn_DEL_CH;
		case MENU_MEM_NAME:  return cn_MEM_NAME;
		case MENU_MDF:       return cn_MDF;
		case MENU_SAVE:      return cn_SAVE;
#ifdef ENABLE_VOX
		case MENU_VOX:       return cn_VOX;
#endif
		case MENU_ABR:       return cn_ABR;
		case MENU_ABR_ON_TX_RX: return cn_ABR_TRX;
		case MENU_ABR_MIN:   return cn_ABR_MIN;
		case MENU_ABR_MAX:   return cn_ABR_MAX;
		case MENU_TDR:       return cn_TDR;
		case MENU_BEEP:      return cn_BEEP;
#ifdef ENABLE_VOICE
		case MENU_VOICE:     return cn_VOICE;
#endif
		case MENU_SC_REV:    return cn_SC_REV;
		case MENU_AUTOLK:    return cn_AUTOLK;
		case MENU_S_ADD1:    return cn_S_ADD1;
		case MENU_S_ADD2:    return cn_S_ADD2;
		case MENU_STE:       return cn_STE;
		case MENU_RP_STE:    return cn_RP_STE;
		case MENU_MIC:       return cn_MIC;
#ifdef ENABLE_AUDIO_BAR
		case MENU_MIC_BAR:   return cn_MIC_BAR;
#endif
		case MENU_COMPAND:   return cn_COMPAND;
		case MENU_1_CALL:    return cn_1_CALL;
		case MENU_S_LIST:    return cn_S_LIST;
		case MENU_SLIST1:    return cn_SLIST1;
		case MENU_SLIST2:    return cn_SLIST2;
		case MENU_AM:        return cn_AM;
#ifdef ENABLE_AM_FIX
		case MENU_AM_FIX:    return cn_AM_FIX;
#endif
#ifdef ENABLE_NOAA
		case MENU_NOAA_S:    return cn_NOAA_S;
#endif
		case MENU_UPCODE:    return cn_UPCODE;
		case MENU_DWCODE:    return cn_DWCODE;
		case MENU_PTT_ID:    return cn_PTT_ID;
		case MENU_D_ST:      return cn_D_ST;
#ifdef ENABLE_DTMF_CALLING
		case MENU_D_RSP:     return cn_D_RSP;
		case MENU_D_HOLD:    return cn_D_HOLD;
#endif
		case MENU_D_PRE:     return cn_D_PRE;
#ifdef ENABLE_DTMF_CALLING
		case MENU_D_DCD:     return cn_D_DCD;
		case MENU_D_LIST:    return cn_D_LIST;
#endif
		case MENU_D_LIVE_DEC: return cn_D_LIVE;
		case MENU_PONMSG:    return cn_PONMSG;
		case MENU_ROGER:     return cn_ROGER;
		case MENU_VOL:       return cn_VOL;
		case MENU_BAT_TXT:   return cn_BAT_TXT;
#ifdef ENABLE_ALARM
		case MENU_AL_MOD:    return cn_AL_MOD;
#endif
		case MENU_LANGUAGE:  return cn_LANGUAGE;
		case MENU_RESET:     return cn_RESET;
		/* Hidden items */
		case MENU_F_LOCK:    return cn_F_LOCK;
		case MENU_200TX:     return cn_200TX;
		case MENU_350TX:     return cn_350TX;
		case MENU_500TX:     return cn_500TX;
		case MENU_350EN:     return cn_350EN;
		case MENU_SCREN:     return cn_SCREN;
#ifdef ENABLE_F_CAL_MENU
		case MENU_F_CALI:    return cn_F_CALI;
#endif
		case MENU_BATCAL:    return cn_BATCAL;
		case MENU_F1SHRT:    return cn_F1SHRT;
		case MENU_F1LONG:    return cn_F1LONG;
		case MENU_F2SHRT:    return cn_F2SHRT;
		case MENU_F2LONG:    return cn_F2LONG;
		case MENU_MLONG:     return cn_MLONG;
		case MENU_BATTYP:    return cn_BATTYP;
		default:             return item->name;
	}
}

#endif  /* ENABLE_CHINESE */
