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

#ifndef UI_UI_H
#define UI_UI_H

#include <stdbool.h>
#include <stdint.h>

void UI_GenerateChannelString(char *pString, const uint8_t Channel);
void UI_GenerateChannelStringEx(char *pString, const bool bShowPrefix, const uint8_t ChannelNumber);
void UI_PrintString(const char *pString, uint8_t Start, uint8_t End, uint8_t Line, uint8_t Width);
void UI_PrintStringSmallNormal(const char *pString, uint8_t Start, uint8_t End, uint8_t Line);
void UI_PrintStringSmallBold(const char *pString, uint8_t Start, uint8_t End, uint8_t Line);
void UI_PrintStringSmallBufferNormal(const char *pString, uint8_t *buffer);
void UI_PrintStringSmallBufferBold(const char *pString, uint8_t * buffer);
void UI_DisplayFrequency(const char *string, uint8_t X, uint8_t Y, bool center);

void UI_DisplayPopup(const char *string);

void UI_DrawPixelBuffer(uint8_t (*buffer)[128], uint8_t x, uint8_t y, bool black);
void UI_DrawLineBuffer(uint8_t (*buffer)[128], int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool black);
void UI_DrawRectangleBuffer(uint8_t (*buffer)[128], int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool black);

void UI_DisplayClear();

#ifdef ENABLE_CHINESE
/* Render a UTF-8 string that may contain 3-byte CJK codepoints.
 * ASCII chars use gFontBig (8px wide, 2 pages).
 * CJK chars use 12x16 glyphs read from AT24C512 EEPROM at EEPROM_CJK_FONT_BASE.
 * When the UI language is not CN the function falls back to UI_PrintString,
 * preserving the original Start/End centering behaviour for English menus.
 * Parameters: pString = null-terminated UTF-8 string.
 *             Start   = left pixel column.
 *             End     = right pixel column (0 = no right bound / centering).
 *             Line    = framebuffer page (same convention as UI_PrintString). */
void UI_PrintStringMixed(const char *pString, uint8_t Start, uint8_t End, uint8_t Line);
#define SUBV_PRINT(str, x1, x2, line)  UI_PrintStringMixed((str), (x1), (x2), (line))
#else
#define SUBV_PRINT(str, x1, x2, line)  UI_PrintString((str), (x1), (x2), (line), 8)
#endif

#endif
