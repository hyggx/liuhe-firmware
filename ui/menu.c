/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>
#include <stdlib.h>

#include "../app/dtmf.h"
#include "../app/menu.h"
#include "../bitmaps.h"
#include "../board.h"
#include "../dcs.h"
#include "../driver/backlight.h"
#include "../driver/bk4819.h"
#include "../driver/eeprom.h"
#include "../driver/st7565.h"
#include "../external/printf/printf.h"
#include "../frequencies.h"
#include "../helper/battery.h"
#include "../misc.h"
#include "../settings.h"
#include "helper.h"
#include "inputbox.h"
#include "menu.h"
#include "ui.h"
#ifdef ENABLE_CHINESE
	#include "menu_lang.h"
	#include "menu_sub_values_cn.h"
#endif

/* Localised menu title: returns CN string in CN mode, else ASCII .name */
#ifdef ENABLE_CHINESE
	#define _MENU_TITLE(item)  UI_MENU_GetMenuTitle(item)
#else
	#define _MENU_TITLE(item)  ((item)->name)
#endif


const t_menu_item MenuList[] =
{
//   text,     voice ID,                               menu ID
	{"Step",   VOICE_ID_FREQUENCY_STEP,                MENU_STEP          },
	{"TxPower", VOICE_ID_POWER,                        MENU_TXP           },
	{"RxDCS",  VOICE_ID_DCS,                           MENU_R_DCS         },
	{"RxCTCSS", VOICE_ID_CTCSS,                        MENU_R_CTCS        },
	{"TxDCS",  VOICE_ID_DCS,                           MENU_T_DCS         },
	{"TxCTCSS", VOICE_ID_CTCSS,                        MENU_T_CTCS        },
	{"TxOffDir", VOICE_ID_TX_OFFSET_FREQUENCY_DIRECTION, MENU_SFT_D       }, // was "TxODir"
	{"TxOffset", VOICE_ID_TX_OFFSET_FREQUENCY,         MENU_OFFSET        }, // was "TxOffs"
	{"BandWdth", VOICE_ID_CHANNEL_BANDWIDTH,           MENU_W_N           }, // was "W/N"
	{"Scramble", VOICE_ID_SCRAMBLER_ON,                MENU_SCR           }, // was "Scramb"
	{"BusyLock", VOICE_ID_BUSY_LOCKOUT,                MENU_BCL           }, // was "BusyCL"
	{"Compand", VOICE_ID_INVALID,                      MENU_COMPAND       }, // was "Compnd"
	{"RxDemod", VOICE_ID_INVALID,                      MENU_AM            }, // was "Demodu"
	{"ScanAdd1", VOICE_ID_INVALID,                     MENU_S_ADD1        }, // was "ScAdd1"
	{"ScanAdd2", VOICE_ID_INVALID,                     MENU_S_ADD2        }, // was "ScAdd2"
	{"SaveChan", VOICE_ID_MEMORY_CHANNEL,              MENU_MEM_CH        }, // was "ChSave"
	{"ChanName", VOICE_ID_INVALID,                     MENU_MEM_NAME      }, // was "ChName"

	{"ScanList", VOICE_ID_INVALID,                     MENU_S_LIST        }, // was "SList"
	{"ScanLst1", VOICE_ID_INVALID,                     MENU_SLIST1        }, // was "SList1"
	{"ScanLst2", VOICE_ID_INVALID,                     MENU_SLIST2        }, // was "SList2"
	{"ScanRsum", VOICE_ID_INVALID,                     MENU_SC_REV        }, // was "ScnRev"
#ifdef ENABLE_NOAA
	{"NOAA-S", VOICE_ID_INVALID,                       MENU_NOAA_S        },
#endif
	{"F1 Short",  VOICE_ID_INVALID,                   MENU_F1SHRT        }, // was "F1Shrt"
	{"F1 Long",   VOICE_ID_INVALID,                   MENU_F1LONG        }, // was "F1Long"
	{"F2 Short",  VOICE_ID_INVALID,                   MENU_F2SHRT        }, // was "F2Shrt"
	{"F2 Long",   VOICE_ID_INVALID,                   MENU_F2LONG        }, // was "F2Long"
	{"MenuLong",  VOICE_ID_INVALID,                   MENU_MLONG         },

	{"KeyLock", VOICE_ID_INVALID,                     MENU_AUTOLK        }, // was "KeyLck"
	{"TxTimer", VOICE_ID_TRANSMIT_OVER_TIME,          MENU_TOT           }, // was "TxTOut"
	{"BatSave", VOICE_ID_SAVE_MODE,                   MENU_SAVE          }, // was "BatSav"
	{"Mic",    VOICE_ID_INVALID,                      MENU_MIC           },
#ifdef ENABLE_AUDIO_BAR
	{"MicBar", VOICE_ID_INVALID,                      MENU_MIC_BAR       },
#endif
	{"ChanDisp", VOICE_ID_INVALID,                    MENU_MDF           }, // was "ChDisp"
	{"PwrOnMsg", VOICE_ID_INVALID,                    MENU_PONMSG        }, // was "POnMsg"
	{"BatText", VOICE_ID_INVALID,                     MENU_BAT_TXT       }, // was "BatTxt"
	{"Backlit",  VOICE_ID_INVALID,                    MENU_ABR           }, // was "BackLt"
	{"BL Min",  VOICE_ID_INVALID,                     MENU_ABR_MIN       }, // was "BLMin"
	{"BL Max",  VOICE_ID_INVALID,                     MENU_ABR_MAX       }, // was "BLMax"
	{"BL TX/RX", VOICE_ID_INVALID,                   MENU_ABR_ON_TX_RX  }, // was "BltTRX"
	{"Beep",   VOICE_ID_BEEP_PROMPT,                  MENU_BEEP          },
#ifdef ENABLE_VOICE
	{"Voice",  VOICE_ID_VOICE_PROMPT,                 MENU_VOICE         },
#endif
	{"Roger",  VOICE_ID_INVALID,                      MENU_ROGER         },
	{"TailElim", VOICE_ID_INVALID,                    MENU_STE           }, // was "STE"
	{"Rpt Tail", VOICE_ID_INVALID,                    MENU_RP_STE        }, // was "RP STE"
	{"1 Call", VOICE_ID_INVALID,                      MENU_1_CALL        },
#ifdef ENABLE_ALARM
	{"AlarmTyp", VOICE_ID_INVALID,                    MENU_AL_MOD        }, // was "AlarmT"
#endif
#ifdef ENABLE_DTMF_CALLING
	{"ANI ID", VOICE_ID_ANI_CODE,                     MENU_ANI_ID        },
#endif
	{"DTMF Up", VOICE_ID_INVALID,                     MENU_UPCODE        }, // was "UPCode"
	{"DTMF Dwn", VOICE_ID_INVALID,                    MENU_DWCODE        }, // was "DWCode"
	{"PTT ID", VOICE_ID_INVALID,                      MENU_PTT_ID        },
	{"DTMF ST", VOICE_ID_INVALID,                     MENU_D_ST          }, // was "D ST"
#ifdef ENABLE_DTMF_CALLING
	{"DTMFResp", VOICE_ID_INVALID,                    MENU_D_RSP         }, // was "D Resp"
	{"DTMFHold", VOICE_ID_INVALID,                    MENU_D_HOLD        }, // was "D Hold"
#endif
	{"DTMF Pre", VOICE_ID_INVALID,                    MENU_D_PRE         }, // was "D Prel"
#ifdef ENABLE_DTMF_CALLING
	{"DTMF Dec", VOICE_ID_INVALID,                    MENU_D_DCD         }, // was "D Decd"
	{"DTMFList", VOICE_ID_INVALID,                    MENU_D_LIST        }, // was "D List"
#endif
	{"DTMFLive", VOICE_ID_INVALID,                    MENU_D_LIVE_DEC    }, // was "D Live"
#ifdef ENABLE_AM_FIX
	{"AM Fix", VOICE_ID_INVALID,                       MENU_AM_FIX        },
#endif
#ifdef ENABLE_VOX
	{"VOX",    VOICE_ID_VOX,                           MENU_VOX           },
#endif
	{"BatVolt", VOICE_ID_INVALID,                      MENU_VOL           }, // was "BatVol"
	{"RxMode", VOICE_ID_DUAL_STANDBY,                  MENU_TDR           },
	{"Squelch", VOICE_ID_SQUELCH,                      MENU_SQL           }, // was "Sql"
#ifdef ENABLE_CHINESE
	{"Language", VOICE_ID_INVALID,                     MENU_LANGUAGE      }, // was "Lang"
#endif
	{"DelChan", VOICE_ID_DELETE_CHANNEL,               MENU_DEL_CH        }, // was "ChDele"

	// hidden menu items from here on
	// enabled if pressing both the PTT and upper side button at power-on
	{"FreqLock", VOICE_ID_INVALID,                     MENU_F_LOCK        }, // was "F Lock"
	{"Tx 200", VOICE_ID_INVALID,                       MENU_200TX         },
	{"Tx 350", VOICE_ID_INVALID,                       MENU_350TX         },
	{"Tx 500", VOICE_ID_INVALID,                       MENU_500TX         },
	{"350 Enbl", VOICE_ID_INVALID,                    MENU_350EN         },
	{"Scram En", VOICE_ID_INVALID,                    MENU_SCREN         }, // was "ScraEn"
#ifdef ENABLE_F_CAL_MENU
	{"FreqCal", VOICE_ID_INVALID,                      MENU_F_CALI        }, // was "FrCali"
#endif
	{"BatCal", VOICE_ID_INVALID,                       MENU_BATCAL        },
	{"AutoOff", VOICE_ID_INVALID,                      MENU_APO           },
	{"BatType", VOICE_ID_INVALID,                      MENU_BATTYP        }, // was "BatTyp"
	{"Reset",  VOICE_ID_INITIALISATION,                MENU_RESET         },

	{"",       VOICE_ID_INVALID,                       0xff               }  // end of list - DO NOT delete or move this this
};

const uint8_t FIRST_HIDDEN_MENU_ITEM = MENU_F_LOCK;

const char gSubMenu_TXP[][5] =
{
	"LOW",
	"MID",
	"HIGH"
};

const char gSubMenu_SFT_D[][4] =
{
	"OFF",
	"+",
	"-"
};

const char gSubMenu_W_N[][7] =
{
	"WIDE",
	"NARROW",
	"NARRW"
};

const char gSubMenu_OFF_ON[][4] =
{
	"OFF",
	"ON"
};

const char gSubMenu_SAVE[][4] =
{
	"OFF",
	"1:1",
	"1:2",
	"1:3",
	"1:4"
};

const char gSubMenu_TOT[][7] =
{
	"30 sec",
	"1 min",
	"2 min",
	"3 min",
	"4 min",
	"5 min",
	"6 min",
	"7 min",
	"8 min",
	"9 min",
	"15 min"
};

const char* const gSubMenu_RXMode[] =
{
	"MAIN\nONLY", 		// TX and RX on main only
	"DUAL RX\nRESPOND", // Watch both and respond
	"CROSS\nBAND", 		// TX on main, RX on secondary
	"MAIN TX\nDUAL RX" 	// always TX on main, but RX on both
};

#ifdef ENABLE_VOICE
	const char gSubMenu_VOICE[][4] =
	{
		"OFF",
		"CHI",
		"ENG"
	};
#endif

const char gSubMenu_SC_REV[][8] =
{
	"TIMEOUT",
	"CARRIER",
	"STOP"
};

const char* const gSubMenu_MDF[] =
{
	"FREQ",
	"CHANNEL\nNUMBER",
	"NAME",
	"NAME\n+\nFREQ"
};

#ifdef ENABLE_ALARM
	const char gSubMenu_AL_MOD[][5] =
	{
		"SITE",
		"TONE"
	};
#endif

#ifdef ENABLE_DTMF_CALLING
const char gSubMenu_D_RSP[][11] =
{
	"DO\nNOTHING",
	"RING",
	"REPLY",
	"BOTH"
};
#endif

const char* const gSubMenu_PTT_ID[] =
{
	"OFF",
	"UP CODE",
	"DOWN CODE",
	"UP+DOWN\nCODE",
	"APOLLO\nQUINDAR"
};

const char gSubMenu_PONMSG[][8] =
{
	"FULL",
	"MESSAGE",
	"VOLTAGE",
	"NONE"
};

const char gSubMenu_ROGER[][6] =
{
	"OFF",
	"ROGER",
	"MDC"
};

const char gSubMenu_RESET[][4] =
{
	"VFO",
	"ALL"
};

const char * const gSubMenu_F_LOCK[] =
{
	"DEFAULT+\n137-174\n400-470",
	"FCC HAM\n144-148\n420-450",
	"CE HAM\n144-146\n430-440",
	"GB HAM\n144-148\n430-440",
	"137-174\n400-430",
	"137-174\n400-438",
	"DISABLE\nALL",
	"UNLOCK\nALL",
};

const char gSubMenu_BACKLIGHT[][7] =
{
	"OFF",
	"5 sec",
	"10 sec",
	"20 sec",
	"1 min",
	"2 min",
	"4 min",
	"ON"
};

const char gSubMenu_RX_TX[][6] =
{
	"OFF",
	"TX",
	"RX",
	"TX/RX"
};

const char gSubMenu_BAT_TXT[][8] =
{
	"NONE",
	"VOLTAGE",
	"PERCENT"
};

const char gSubMenu_BATTYP[][9] =
{
	"1600mAh",
	"2200mAh"
};

const char gSubMenu_SCRAMBLER[][7] =
{
	"OFF",
	"2600Hz",
	"2700Hz",
	"2800Hz",
	"2900Hz",
	"3000Hz",
	"3100Hz",
	"3200Hz",
	"3300Hz",
	"3400Hz",
	"3500Hz"
};

const t_sidefunction gSubMenu_SIDEFUNCTIONS[] =
{
	{"NONE",			ACTION_OPT_NONE},
#ifdef ENABLE_FLASHLIGHT
	{"FLASH\nLIGHT",	ACTION_OPT_FLASHLIGHT},
#endif
	{"POWER",			ACTION_OPT_POWER},
	{"MONITOR",			ACTION_OPT_MONITOR},
	{"SCAN",			ACTION_OPT_SCAN},
#ifdef ENABLE_VOX
	{"VOX",				ACTION_OPT_VOX},
#endif
#ifdef ENABLE_ALARM
	{"ALARM",			ACTION_OPT_ALARM},
#endif
#ifdef ENABLE_FMRADIO
	{"FM RADIO",		ACTION_OPT_FM},
#endif
#ifdef ENABLE_TX1750
	{"1750HZ",			ACTION_OPT_1750},
#endif
	{"LOCK\nKEYPAD",	ACTION_OPT_KEYLOCK},
	{"SWITCH\nVFO",		ACTION_OPT_A_B},
	{"VFO/MR",			ACTION_OPT_VFO_MR},
	{"SWITCH\nDEMODUL",	ACTION_OPT_SWITCH_DEMODUL},
#ifdef ENABLE_BLMIN_TMP_OFF
	{"BLMIN\nTMP OFF",  ACTION_OPT_BLMIN_TMP_OFF}, 		//BackLight Minimum Temporay OFF
#endif
#ifdef ENABLE_SPECTRUM
	{"SPECTRUM",         ACTION_OPT_SPECTRUM}
#endif
};

const uint8_t gSubMenu_SIDEFUNCTIONS_size = ARRAY_SIZE(gSubMenu_SIDEFUNCTIONS);

bool    gIsInSubMenu;
uint8_t gMenuCursor;
int UI_MENU_GetCurrentMenuId() {
	if(gMenuCursor < ARRAY_SIZE(MenuList))
		return MenuList[gMenuCursor].menu_id;

	return MenuList[ARRAY_SIZE(MenuList)-1].menu_id;
}

uint8_t UI_MENU_GetMenuIdx(uint8_t id)
{
	for(uint8_t i = 0; i < ARRAY_SIZE(MenuList); i++)
		if(MenuList[i].menu_id == id)
			return i;
	return 0;
}

int32_t gSubMenuSelection;

// edit box
char    edit_original[17]; // a copy of the text before editing so that we can easily test for changes/difference
char    edit[17];
int     edit_index;

void UI_DisplayMenu(void)
{
	// Full-width value area: x1=0, x2=LCD_WIDTH-1
	const unsigned int menu_item_x1 = 0;
	const unsigned int menu_item_x2 = LCD_WIDTH - 1;
	unsigned int       i;
	char               String[64];  // bigger cuz we can now do multi-line in one string (use '\n' char)

#ifdef ENABLE_DTMF_CALLING
	char               Contact[16];
#endif

	UI_DisplayClear();

	// --- Top-bottom layout: title (pages 0-1) + value area (pages 2-7) ---
	{
		const int menu_index = gMenuCursor;

		// Counter: "N/Total" small font, right-aligned, vertically centred in 16px title bar.
		// Render into page 0 first, then shift each column byte down 4px so the 8px-tall
		// small font sits visually centred between pages 0 and 1: (16-8)/2 = 4px offset.
		sprintf(String, "%u/%u", 1 + gMenuCursor, gMenuListCount);
		{
			const uint8_t idx_len = (uint8_t)strlen(String);
			const uint8_t idx_x   = (uint8_t)(LCD_WIDTH - idx_len * 7u);
			UI_PrintStringSmallNormal(String, idx_x, 0, 0);
			for (uint8_t col = idx_x; col < LCD_WIDTH; col++) {
				const uint8_t b = gFrameBuffer[0][col];
				if (b) {
					gFrameBuffer[0][col] = (uint8_t)(b << 4);
					gFrameBuffer[1][col] |= (uint8_t)(b >> 4);
				}
			}
		}

		// Title: left-aligned (no centering), big font, page 0
		if (menu_index >= 0 && menu_index < (int)gMenuListCount)
			SUBV_PRINT(_MENU_TITLE(&MenuList[menu_index]), 0, 0, 0);

		// Dotted horizontal separator at bottom pixel of page 1
		for (i = 0; i < LCD_WIDTH; i++)
			gFrameBuffer[1][i] |= 0x80;
	}

	// **************

	memset(String, 0, sizeof(String));

	bool already_printed = false;

	/* Brightness is set to max in some entries of this menu. Return it to the configured brightness
	   level the "next" time we enter here.I.e., when we move from one menu to another.
	   It also has to be set back to max when pressing the Exit key. */

	BACKLIGHT_TurnOn();

	switch (UI_MENU_GetCurrentMenuId())
	{
		case MENU_SQL:
			sprintf(String, "%d", gSubMenuSelection);
			break;

		case MENU_MIC:
			{	// display the mic gain in actual dB rather than just an index number
				const uint8_t mic = gMicGain_dB2[gSubMenuSelection];
				sprintf(String, "+%u.%01udB", mic / 2, mic % 2);
			}
			break;

		#ifdef ENABLE_AUDIO_BAR
			case MENU_MIC_BAR:
				strcpy(String, SUBV(gSubMenu_OFF_ON[gSubMenuSelection], gSubMenu_OFF_ON_CN[gSubMenuSelection]));
				break;
		#endif

		case MENU_STEP: {
			uint16_t step = gStepFrequencyTable[FREQUENCY_GetStepIdxFromSortedIdx(gSubMenuSelection)];
			sprintf(String, "%d.%02ukHz", step / 100, step % 100);
			break;
		}

		case MENU_TXP: {
			static const char * const txp_watts[] = {"0.5W", "2W", "5W"};
			sprintf(String, "%s\n%s", SUBV(gSubMenu_TXP[gSubMenuSelection], gSubMenu_TXP_CN[gSubMenuSelection]), txp_watts[gSubMenuSelection]);
			break;
		}

		case MENU_R_DCS:
		case MENU_T_DCS:
			if (gSubMenuSelection == 0)
				strcpy(String, "OFF");
			else if (gSubMenuSelection < 105)
				sprintf(String, "D%03oN", DCS_Options[gSubMenuSelection -   1]);
			else
				sprintf(String, "D%03oI", DCS_Options[gSubMenuSelection - 105]);
			break;

		case MENU_R_CTCS:
		case MENU_T_CTCS:
		{
			if (gSubMenuSelection == 0)
				strcpy(String, "OFF");
			else
				sprintf(String, "%u.%uHz", CTCSS_Options[gSubMenuSelection - 1] / 10, CTCSS_Options[gSubMenuSelection - 1] % 10);
			break;
		}

		case MENU_SFT_D:
			strcpy(String, SUBV(gSubMenu_SFT_D[gSubMenuSelection], gSubMenu_SFT_D_CN[gSubMenuSelection]));
			break;

		case MENU_OFFSET:
			if (!gIsInSubMenu || gInputBoxIndex == 0)
				sprintf(String, "%3d.%05u MHz", gSubMenuSelection / 100000, abs(gSubMenuSelection) % 100000);
			else
			{
				const char * ascii = INPUTBOX_GetAscii();
				sprintf(String, "%.3s.%.3s MHz", ascii, ascii + 3);
			}
			UI_PrintString(String, menu_item_x1, menu_item_x2, 4, 8);
			already_printed = true;
			break;

		case MENU_W_N:
			strcpy(String, SUBV(gSubMenu_W_N[gSubMenuSelection], gSubMenu_W_N_CN[gSubMenuSelection]));
			break;

		case MENU_SCR:
			strcpy(String, gSubMenu_SCRAMBLER[gSubMenuSelection]);
			#if 1
				if (gSubMenuSelection > 0 && gSetting_ScrambleEnable)
					BK4819_EnableScramble(gSubMenuSelection - 1);
				else
					BK4819_DisableScramble();
			#endif
			break;

		#ifdef ENABLE_VOX
			case MENU_VOX:
				if (gSubMenuSelection == 0)
					strcpy(String, "OFF");
				else
					sprintf(String, "%d", gSubMenuSelection);
				break;
		#endif

		case MENU_ABR:
			strcpy(String, SUBV(gSubMenu_BACKLIGHT[gSubMenuSelection], gSubMenu_BACKLIGHT_CN[gSubMenuSelection]));
			if(BACKLIGHT_GetBrightness() < 4)
				BACKLIGHT_SetBrightness(4);
			break;

		case MENU_ABR_MIN:
		case MENU_ABR_MAX:
			sprintf(String, "%d", gSubMenuSelection);
			if(gIsInSubMenu)
				BACKLIGHT_SetBrightness(gSubMenuSelection);
			else if(BACKLIGHT_GetBrightness() < 4)
				BACKLIGHT_SetBrightness(4);
			break;

		case MENU_AM:
			strcpy(String, gModulationStr[gSubMenuSelection]);
			break;

		case MENU_AUTOLK:
			strcpy(String, SUBV((gSubMenuSelection == 0) ? "OFF" : "AUTO", (gSubMenuSelection == 0) ? "\xe5\x85\xb3" : "\xe8\x87\xaa\xe5\x8a\xa8"));
			break;

		case MENU_COMPAND:
		case MENU_ABR_ON_TX_RX:
			strcpy(String, SUBV(gSubMenu_RX_TX[gSubMenuSelection], gSubMenu_RX_TX_CN[gSubMenuSelection]));
			break;

		#ifdef ENABLE_AM_FIX
			case MENU_AM_FIX:
		#endif
		case MENU_BCL:
		case MENU_BEEP:
		case MENU_S_ADD1:
		case MENU_S_ADD2:
		case MENU_STE:
		case MENU_D_ST:
#ifdef ENABLE_DTMF_CALLING
		case MENU_D_DCD:
#endif
		case MENU_D_LIVE_DEC:
		#ifdef ENABLE_NOAA
			case MENU_NOAA_S:
		#endif
		case MENU_350TX:
		case MENU_200TX:
		case MENU_500TX:
		case MENU_350EN:
		case MENU_SCREN:
			strcpy(String, SUBV(gSubMenu_OFF_ON[gSubMenuSelection], gSubMenu_OFF_ON_CN[gSubMenuSelection]));
			break;

		case MENU_MEM_CH:
		case MENU_1_CALL:
		case MENU_DEL_CH:
		{
			const bool valid = RADIO_CheckValidChannel(gSubMenuSelection, false, 0);

			// Line 1: channel number (pages 2-3)
			UI_GenerateChannelStringEx(String, valid, gSubMenuSelection);
			UI_PrintString(String, menu_item_x1, menu_item_x2, 2, 8);

			if (!gAskForConfirmation)
			{
				// Line 2: channel name (pages 4-5)
				SETTINGS_FetchChannelName(String, gSubMenuSelection);
				UI_PrintString(String[0] ? String : "--", menu_item_x1, menu_item_x2, 4, 8);

				// Line 3: frequency small font (page 6 only — avoids page-7 cutoff)
				if (valid)
				{
					const uint32_t frequency = SETTINGS_FetchChannelFrequency(gSubMenuSelection);
					sprintf(String, "%u.%05u MHz", frequency / 100000, frequency % 100000);
					UI_PrintStringSmallNormal(String, menu_item_x1, menu_item_x2, 6);
				}
			}
			// When gAskForConfirmation: only CH# shown; SURE?/WAIT! drawn below
			already_printed = true;
			break;
		}

		case MENU_MEM_NAME:
		{
			const bool valid = RADIO_CheckValidChannel(gSubMenuSelection, false, 0);

			// Line 1: channel number (pages 2-3)
			UI_GenerateChannelStringEx(String, valid, gSubMenuSelection);
			UI_PrintString(String, menu_item_x1, menu_item_x2, 2, 8);

			if (valid && !gAskForConfirmation)
			{
				const uint32_t frequency = SETTINGS_FetchChannelFrequency(gSubMenuSelection);

				// Line 2: channel name or edit buffer (pages 4-5)
				if (!gIsInSubMenu || edit_index < 0)
				{
					SETTINGS_FetchChannelName(String, gSubMenuSelection);
					UI_PrintString(String[0] ? String : "--", menu_item_x1, menu_item_x2, 4, 8);
				}
				else
				{
					UI_PrintString(edit, menu_item_x1, menu_item_x2, 4, 8);
					// Underline cursor: 1px at bottom of page 5 (adjusted for centred text)
					if (edit_index < 10)
					{
						const uint8_t cx = (uint8_t)((LCD_WIDTH - 10 * 8) / 2 + edit_index * 8);
						for (uint8_t dx = 0; dx < 7 && (cx + dx) < LCD_WIDTH; dx++)
							gFrameBuffer[5][cx + dx] |= 0x80;
					}
				}

				// Line 3: frequency small font (page 6) — hidden during name editing
				if (!gIsInSubMenu || edit_index < 0)
				{
					sprintf(String, "%u.%05u MHz", frequency / 100000, frequency % 100000);
					UI_PrintStringSmallNormal(String, menu_item_x1, menu_item_x2, 6);
				}
			}
			// When gAskForConfirmation: only CH# shown; SURE?/WAIT! drawn below

			already_printed = true;
			break;
		}

		case MENU_SAVE:
			strcpy(String, SUBV(gSubMenu_SAVE[gSubMenuSelection], gSubMenu_SAVE_CN[gSubMenuSelection]));
			break;

		case MENU_TDR:
			strcpy(String, SUBV(gSubMenu_RXMode[gSubMenuSelection], gSubMenu_RXMode_CN[gSubMenuSelection]));
			break;

		case MENU_TOT:
			strcpy(String, SUBV(gSubMenu_TOT[gSubMenuSelection], gSubMenu_TOT_CN[gSubMenuSelection]));
			break;

		#ifdef ENABLE_VOICE
			case MENU_VOICE:
				strcpy(String, gSubMenu_VOICE[gSubMenuSelection]);
				break;
		#endif

		case MENU_SC_REV:
			strcpy(String, SUBV(gSubMenu_SC_REV[gSubMenuSelection], gSubMenu_SC_REV_CN[gSubMenuSelection]));
			break;

		case MENU_MDF:
			strcpy(String, SUBV(gSubMenu_MDF[gSubMenuSelection], gSubMenu_MDF_CN[gSubMenuSelection]));
			break;

		case MENU_RP_STE:
			if (gSubMenuSelection == 0)
				strcpy(String, SUBV("OFF", "\xe5\x85\xb3"));
			else
				sprintf(String, "%d*100ms", gSubMenuSelection);
			break;

		case MENU_S_LIST:
			if (gSubMenuSelection < 2)
				sprintf(String, SUBV("LIST%u", "\xe5\x88\x97\xe8\xa1\xa8%u"), 1 + gSubMenuSelection);
			else
				strcpy(String, SUBV("ALL", "\xe5\x85\xa8\xe9\x83\xa8"));
			break;

		#ifdef ENABLE_ALARM
			case MENU_AL_MOD:
				strcpy(String, SUBV(gSubMenu_AL_MOD[gSubMenuSelection], gSubMenu_AL_MOD_CN[gSubMenuSelection]));
				break;
		#endif

#ifdef ENABLE_DTMF_CALLING
		case MENU_ANI_ID:
			strcpy(String, gEeprom.ANI_DTMF_ID);
			break;
#endif
		case MENU_UPCODE:
			sprintf(String, "%.8s\n%.8s", gEeprom.DTMF_UP_CODE, gEeprom.DTMF_UP_CODE + 8);
			break;

		case MENU_DWCODE:
			sprintf(String, "%.8s\n%.8s", gEeprom.DTMF_DOWN_CODE, gEeprom.DTMF_DOWN_CODE + 8);
			break;

#ifdef ENABLE_DTMF_CALLING
		case MENU_D_RSP:
			strcpy(String, gSubMenu_D_RSP[gSubMenuSelection]);
			break;

		case MENU_D_HOLD:
			sprintf(String, "%ds", gSubMenuSelection);
			break;
#endif
		case MENU_D_PRE:
			sprintf(String, "%d*10ms", gSubMenuSelection);
			break;

		case MENU_PTT_ID:
			strcpy(String, gSubMenu_PTT_ID[gSubMenuSelection]);
			break;

		case MENU_BAT_TXT:
			strcpy(String, SUBV(gSubMenu_BAT_TXT[gSubMenuSelection], gSubMenu_BAT_TXT_CN[gSubMenuSelection]));
			break;

#ifdef ENABLE_DTMF_CALLING
		case MENU_D_LIST:
			gIsDtmfContactValid = DTMF_GetContact((int)gSubMenuSelection - 1, Contact);
			if (!gIsDtmfContactValid)
				strcpy(String, "NULL");
			else
				memcpy(String, Contact, 8);
			break;
#endif

		case MENU_PONMSG:
			strcpy(String, SUBV(gSubMenu_PONMSG[gSubMenuSelection], gSubMenu_PONMSG_CN[gSubMenuSelection]));
			break;

		case MENU_ROGER:
			strcpy(String, SUBV(gSubMenu_ROGER[gSubMenuSelection], gSubMenu_ROGER_CN[gSubMenuSelection]));
			break;

		case MENU_VOL:
			sprintf(String, "%u.%02uV\n%u%%",
				gBatteryVoltageAverage / 100, gBatteryVoltageAverage % 100,
				BATTERY_VoltsToPercent(gBatteryVoltageAverage));
			break;

		case MENU_RESET:
			strcpy(String, SUBV(gSubMenu_RESET[gSubMenuSelection], gSubMenu_RESET_CN[gSubMenuSelection]));
			break;

#ifdef ENABLE_CHINESE
		case MENU_LANGUAGE:
			if (gSubMenuSelection == UI_LANGUAGE_CN)
				strcpy(String, SUBV("Chinese", gSubMenu_LANGUAGE[UI_LANGUAGE_CN]));
			else
				strcpy(String, gSubMenu_LANGUAGE[gSubMenuSelection]);
			break;
#endif

		case MENU_F_LOCK:
			if(!gIsInSubMenu && gUnlockAllTxConfCnt>0 && gUnlockAllTxConfCnt<10)
				strcpy(String, SUBV("READ\nMANUAL", "\xe9\x98\x85\xe8\xaf\xbb\n\xe6\x89\x8b\xe5\x86\x8c"));
			else
				strcpy(String, SUBV(gSubMenu_F_LOCK[gSubMenuSelection], gSubMenu_F_LOCK_CN[gSubMenuSelection]));
			break;

		#ifdef ENABLE_F_CAL_MENU
			case MENU_F_CALI:
				{
					const uint32_t value   = 22656 + gSubMenuSelection;
					const uint32_t xtal_Hz = (0x4f0000u + value) * 5;

					writeXtalFreqCal(gSubMenuSelection, false);

					sprintf(String, "%d\n%u.%06u\nMHz",
						gSubMenuSelection,
						xtal_Hz / 1000000, xtal_Hz % 1000000);
				}
				break;
		#endif

		case MENU_BATCAL:
		{
			const uint16_t vol = (uint32_t)gBatteryVoltageAverage * gBatteryCalibration[3] / gSubMenuSelection;
			sprintf(String, "%u.%02uV\n%u", vol / 100, vol % 100, gSubMenuSelection);
			break;
		}

		case MENU_APO:
			if (gSubMenuSelection == 0)
				strcpy(String, SUBV("Off", "\xe5\x85\xb3"));
			else
				sprintf(String, SUBV("%umin", "%u\xe5\x88\x86"), (unsigned)gSubMenuSelection);
			break;

		case MENU_BATTYP:
			strcpy(String, SUBV(gSubMenu_BATTYP[gSubMenuSelection], gSubMenu_BATTYP_CN[gSubMenuSelection]));
			break;

		case MENU_F1SHRT:
		case MENU_F1LONG:
		case MENU_F2SHRT:
		case MENU_F2LONG:
		case MENU_MLONG:
			strcpy(String, SUBV(gSubMenu_SIDEFUNCTIONS[gSubMenuSelection].name,
			                    gSidefuncCN[gSubMenu_SIDEFUNCTIONS[gSubMenuSelection].id]));
			break;

	}

	if (!already_printed)
	{	// we now do multi-line text in a single string

		unsigned int y;
		unsigned int lines = 1;
		unsigned int len   = strlen(String);
		bool         small = false;

		if (len > 0)
		{
			// count number of lines
			for (i = 0; i < len; i++)
			{
				if (String[i] == '\n' && i < (len - 1))
				{	// found new line char
					lines++;
					String[i] = 0;  // null terminate the line
				}
			}

			if (lines > 3)
			{	// use small text
				small = true;
				if (lines > 6)
					lines = 6;
			}

			// Center vertically in the value area (pages 2-7 = 6 pages tall).
			// Big font (2 pages/line): y = 5 - lines  (1 line→4, 2→3, 3→2)
			// Small font (1 page/line): y = 2 + (6 - lines) / 2
			if (small)
				y = 2 + (6 - lines) / 2;
			else
				y = (lines <= 3) ? (5 - lines) : 2;

			// draw the text lines
			for (i = 0; i < len && lines > 0; lines--)
			{
				if (small)
					UI_PrintStringSmallNormal(String + i, menu_item_x1, menu_item_x2, y);
				else
					SUBV_PRINT(String + i, menu_item_x1, menu_item_x2, y);

				// look for start of next line
				while (i < len && String[i] >= 32)
					i++;

				// hop over the null term char(s)
				while (i < len && String[i] < 32)
					i++;

				y += small ? 1 : 2;
			}
		}
	}

	if (UI_MENU_GetCurrentMenuId() == MENU_SLIST1 || UI_MENU_GetCurrentMenuId() == MENU_SLIST2)
	{
		i = (UI_MENU_GetCurrentMenuId() == MENU_SLIST1) ? 0 : 1;
		char *pPrintStr = String;

		if (gSubMenuSelection < 0) {
			pPrintStr = "NULL";
		} else {
			UI_GenerateChannelStringEx(String, true, gSubMenuSelection);
			pPrintStr = String;
		}

		// channel number
		UI_PrintString(pPrintStr, menu_item_x1, menu_item_x2, 2, 8);

		SETTINGS_FetchChannelName(String, gSubMenuSelection);
		pPrintStr = String[0] ? String : "--";

		// channel name and scan-list
		if (gSubMenuSelection < 0 || !gEeprom.SCAN_LIST_ENABLED[i]) {
			UI_PrintString(pPrintStr, menu_item_x1, menu_item_x2, 4, 8);
		} else {
			UI_PrintStringSmallNormal(pPrintStr, menu_item_x1, menu_item_x2, 4);

			if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH1[i])) {
				sprintf(String, "PRI%d:%u", 1, gEeprom.SCANLIST_PRIORITY_CH1[i] + 1);
				UI_PrintStringSmallNormal(String, menu_item_x1, menu_item_x2, 5);
			}

			if (IS_MR_CHANNEL(gEeprom.SCANLIST_PRIORITY_CH2[i])) {
				sprintf(String, "PRI%d:%u", 2, gEeprom.SCANLIST_PRIORITY_CH2[i] + 1);
				UI_PrintStringSmallNormal(String, menu_item_x1, menu_item_x2, 6);
			}
		}
	}

	if ((UI_MENU_GetCurrentMenuId() == MENU_R_CTCS || UI_MENU_GetCurrentMenuId() == MENU_R_DCS) && gCssBackgroundScan)
		UI_PrintString("SCAN", menu_item_x1, menu_item_x2, 5, 8);

#ifdef ENABLE_DTMF_CALLING
	if (UI_MENU_GetCurrentMenuId() == MENU_D_LIST && gIsDtmfContactValid) {
		Contact[11] = 0;
		memcpy(&gDTMF_ID, Contact + 8, 4);
		sprintf(String, "ID:%4s", gDTMF_ID);
		UI_PrintString(String, menu_item_x1, menu_item_x2, 5, 8);
	}
#endif

	if (UI_MENU_GetCurrentMenuId() == MENU_R_CTCS ||
	    UI_MENU_GetCurrentMenuId() == MENU_T_CTCS ||
	    UI_MENU_GetCurrentMenuId() == MENU_R_DCS  ||
	    UI_MENU_GetCurrentMenuId() == MENU_T_DCS
#ifdef ENABLE_DTMF_CALLING
	    || UI_MENU_GetCurrentMenuId() == MENU_D_LIST
#endif
	) {	// Show list index at left edge of content area (page 2) — avoids title-bar counter overlap
		sprintf(String, "%2d", gSubMenuSelection);
		UI_PrintStringSmallNormal(String, 0, 0, 2);
		// Shift index down 2px to add a small gap from the separator line (same trick as counter)
		{
			const uint8_t idx_w = (uint8_t)(strlen(String) * 7u); // char_spacing = char_width(6) + 1
			for (uint8_t col = 0; col < idx_w; col++) {
				const uint8_t b = gFrameBuffer[2][col];
				if (b) {
					gFrameBuffer[2][col] = (uint8_t)(b << 2);
					gFrameBuffer[3][col] |= (uint8_t)(b >> 6);
				}
			}
		}
	}

	if ((UI_MENU_GetCurrentMenuId() == MENU_RESET    ||
	     UI_MENU_GetCurrentMenuId() == MENU_MEM_CH   ||
	     UI_MENU_GetCurrentMenuId() == MENU_MEM_NAME ||
	     UI_MENU_GetCurrentMenuId() == MENU_DEL_CH) && gAskForConfirmation)
	{	// display confirmation in name slot (pages 4-5), visible above page-7 cutoff
		char *pPrintStr = (gAskForConfirmation == 1) ? "SURE?" : "WAIT!";
		UI_PrintString(pPrintStr, menu_item_x1, menu_item_x2, 4, 8);
	}

	// Edit mode indicator: big '>' at right side of value area (pages 2-3)
	if (gIsInSubMenu)
		UI_PrintString(">", LCD_WIDTH - 8, 0, 2, 8);

	ST7565_BlitFullScreen();
}
