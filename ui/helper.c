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

#include "driver/st7565.h"
#include "external/printf/printf.h"
#include "font.h"
#include "ui/helper.h"
#include "ui/inputbox.h"
#include "misc.h"

#ifndef ARRAY_SIZE
	#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

void UI_GenerateChannelString(char *pString, const uint8_t Channel)
{
	unsigned int i;

	if (gInputBoxIndex == 0)
	{
		sprintf(pString, "CH-%02u", Channel + 1);
		return;
	}

	pString[0] = 'C';
	pString[1] = 'H';
	pString[2] = '-';
	for (i = 0; i < 2; i++)
		pString[i + 3] = (gInputBox[i] == 10) ? '-' : gInputBox[i] + '0';
}

void UI_GenerateChannelStringEx(char *pString, const bool bShowPrefix, const uint8_t ChannelNumber)
{
	if (gInputBoxIndex > 0) {
		for (unsigned int i = 0; i < 3; i++) {
			pString[i] = (gInputBox[i] == 10) ? '-' : gInputBox[i] + '0';
		}

		pString[3] = 0;
		return;
	}

	if (bShowPrefix) {
		// BUG here? Prefixed NULLs are allowed
		sprintf(pString, "CH-%03u", ChannelNumber + 1);
	} else if (ChannelNumber == 0xFF) {
		strcpy(pString, "NULL");
	} else {
		sprintf(pString, "%03u", ChannelNumber + 1);
	}
}

void UI_PrintStringBuffer(const char *pString, uint8_t * buffer, uint32_t char_width, const uint8_t *font)
{
	const size_t Length = strlen(pString);
	const unsigned int char_spacing = char_width + 1;
	for (size_t i = 0; i < Length; i++) {
		const unsigned int index = pString[i] - ' ' - 1;
		if (pString[i] > ' ' && pString[i] < 127) {
			const uint32_t offset = i * char_spacing + 1;
			memcpy(buffer + offset, font + index * char_width, char_width);
		}
	}
}

void UI_PrintString(const char *pString, uint8_t Start, uint8_t End, uint8_t Line, uint8_t Width)
{
	size_t i;
	size_t Length = strlen(pString);

	if (End > Start)
		Start += (((End - Start) - (Length * Width)) + 1) / 2;

	for (i = 0; i < Length; i++)
	{
		const unsigned int ofs   = (unsigned int)Start + (i * Width);
		if (pString[i] > ' ' && pString[i] < 127)
		{
			const unsigned int index = pString[i] - ' ' - 1;
			memcpy(gFrameBuffer[Line + 0] + ofs, &gFontBig[index][0], 7);
			memcpy(gFrameBuffer[Line + 1] + ofs, &gFontBig[index][7], 7);
		}
	}
}

void UI_PrintStringSmall(const char *pString, uint8_t Start, uint8_t End, uint8_t Line, uint8_t char_width, const uint8_t *font)
{
	const size_t Length = strlen(pString);
	const unsigned int char_spacing = char_width + 1;

	if (End > Start) {
		Start += (((End - Start) - Length * char_spacing) + 1) / 2;
	}

	UI_PrintStringBuffer(pString, gFrameBuffer[Line] + Start, char_width, font);
}

void UI_PrintStringSmallNormal(const char *pString, uint8_t Start, uint8_t End, uint8_t Line)
{
	UI_PrintStringSmall(pString, Start, End, Line, ARRAY_SIZE(gFontSmall[0]), (const uint8_t *)gFontSmall);
}

void UI_PrintStringSmallBold(const char *pString, uint8_t Start, uint8_t End, uint8_t Line)
{
#ifdef ENABLE_SMALL_BOLD
	const uint8_t *font = (uint8_t *)gFontSmallBold;
	const uint8_t char_width = ARRAY_SIZE(gFontSmallBold[0]);
#else
	const uint8_t *font = (uint8_t *)gFontSmall;
	const uint8_t char_width = ARRAY_SIZE(gFontSmall[0]);
#endif

	UI_PrintStringSmall(pString, Start, End, Line, char_width, font);
}

void UI_PrintStringSmallBufferNormal(const char *pString, uint8_t * buffer)
{
	UI_PrintStringBuffer(pString, buffer, ARRAY_SIZE(gFontSmall[0]), (uint8_t *)gFontSmall);
}

void UI_PrintStringSmallBufferBold(const char *pString, uint8_t * buffer)
{
#ifdef ENABLE_SMALL_BOLD
	const uint8_t *font = (uint8_t *)gFontSmallBold;
	const uint8_t char_width = ARRAY_SIZE(gFontSmallBold[0]);
#else
	const uint8_t *font = (uint8_t *)gFontSmall;
	const uint8_t char_width = ARRAY_SIZE(gFontSmall[0]);
#endif
	UI_PrintStringBuffer(pString, buffer, char_width, font);
}

void UI_DisplayFrequency(const char *string, uint8_t X, uint8_t Y, bool center)
{
	const unsigned int char_width  = 13;
	uint8_t           *pFb0        = gFrameBuffer[Y] + X;
	uint8_t           *pFb1        = pFb0 + 128;
	bool               bCanDisplay = false;

	uint8_t len = strlen(string);
	for(int i = 0; i < len; i++) {
		char c = string[i];
		if(c=='-') c = '9' + 1;
		if (bCanDisplay || c != ' ')
		{
			bCanDisplay = true;
			if(c>='0' && c<='9' + 1) {
				memcpy(pFb0 + 2, gFontBigDigits[c-'0'],                  char_width - 3);
				memcpy(pFb1 + 2, gFontBigDigits[c-'0'] + char_width - 3, char_width - 3);
			}
			else if(c=='.') {
				*pFb1 = 0x60; pFb0++; pFb1++;
				*pFb1 = 0x60; pFb0++; pFb1++;
				*pFb1 = 0x60; pFb0++; pFb1++;
				continue;
			}

		}
		else if (center) {
			pFb0 -= 6;
			pFb1 -= 6;
		}
		pFb0 += char_width;
		pFb1 += char_width;
	}
}

void UI_DrawPixelBuffer(uint8_t (*buffer)[128], uint8_t x, uint8_t y, bool black)
{
	const uint8_t pattern = 1 << (y % 8);
	if(black)
		buffer[y/8][x] |= pattern;
	else
		buffer[y/8][x] &= ~pattern;
}

static void sort(int16_t *a, int16_t *b)
{
	if(*a > *b) {
		int16_t t = *a;
		*a = *b;
		*b = t;
	}
}

void UI_DrawLineBuffer(uint8_t (*buffer)[128], int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool black)
{
	if(x2==x1) {
		sort(&y1, &y2);
		for(int16_t i = y1; i <= y2; i++) {
			UI_DrawPixelBuffer(buffer, x1, i, black);
		}
	} else {
		const int multipl = 1000;
		int a = (y2-y1)*multipl / (x2-x1);
		int b = y1 - a * x1 / multipl;

		sort(&x1, &x2);
		for(int i = x1; i<= x2; i++)
		{
			UI_DrawPixelBuffer(buffer, i, i*a/multipl +b, black);
		}
	}
}

void UI_DrawRectangleBuffer(uint8_t (*buffer)[128], int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool black)
{
	UI_DrawLineBuffer(buffer, x1,y1, x1,y2, black);
	UI_DrawLineBuffer(buffer, x1,y1, x2,y1, black);
	UI_DrawLineBuffer(buffer, x2,y1, x2,y2, black);
	UI_DrawLineBuffer(buffer, x1,y2, x2,y2, black);
}


void UI_DisplayPopup(const char *string)
{
	UI_DisplayClear();

	// for(uint8_t i = 1; i < 5; i++) {
	// 	memset(gFrameBuffer[i]+8, 0x00, 111);
	// }

	// for(uint8_t x = 10; x < 118; x++) {
	// 	UI_DrawPixelBuffer(x, 10, true);
	// 	UI_DrawPixelBuffer(x, 46-9, true);
	// }

	// for(uint8_t y = 11; y < 37; y++) {
	// 	UI_DrawPixelBuffer(10, y, true);
	// 	UI_DrawPixelBuffer(117, y, true);
	// }
	// DrawRectangle(9,9, 118,38, true);
	UI_PrintString(string, 9, 118, 2, 8);
	UI_PrintStringSmallNormal("Press EXIT", 9, 118, 6);
}

void UI_DisplayClear()
{
	memset(gFrameBuffer, 0, sizeof(gFrameBuffer));
}

#ifdef ENABLE_CHINESE
/* ---------------------------------------------------------------------------
 * CJK mixed-script renderer
 *
 * Font layout in AT24C512 EEPROM starting at EEPROM_CJK_FONT_BASE (0x2000):
 *
 *   Header (4 bytes, 8-byte aligned block at 0x2000):
 *     uint16_t glyph_count   — number of glyphs in the table
 *     uint16_t reserved      — 0xFFFF
 *
 *   Index table (sorted, 8-byte aligned, immediately after header):
 *     uint16_t codepoints[glyph_count]   — Unicode codepoints, sorted ascending
 *
 *   Glyph data (8-byte aligned, immediately after index table):
 *     uint8_t  glyphs[glyph_count][CJK_BYTES_PER_GLYPH]
 *
 * Glyph format: 12-pixel-wide × 16-pixel-tall, column-major, LSB = top pixel.
 *   CJK_BYTES_PER_GLYPH = 12 × 2 = 24 bytes.
 *   glyph[col*2 + 0] = rows 0-7  of column col  (byte 0 = top 8 rows)
 *   glyph[col*2 + 1] = rows 8-15 of column col
 *
 * Rendering occupies 2 framebuffer pages (Line and Line+1), each glyph 12px wide.
 * ASCII characters use gFontBig (7px wide, 2 pages) to match vertical height.
 * ---------------------------------------------------------------------------
 */

#include "driver/crc.h"
#include "driver/eeprom.h"
#include "settings.h"

/* Use the canonical constant from driver/eeprom.h */
#define EEPROM_CJK_FONT_BASE   EEPROM_FONT_BASE
#define CJK_GLYPH_WIDTH        13u
#define CJK_BYTES_PER_GLYPH    (CJK_GLYPH_WIDTH * 2u)
#define ASCII_GLYPH_WIDTH      8u   /* gFontBig renders into 8 columns (7 data + 1 spacer) */

/* Font binary header constants (must match tools/gen_cjk_font.py) */
#define CJK_FONT_MAGIC    0x4B36u   /* 'K','6' little-endian */
#define CJK_FONT_VERSION  1u

/* Decode one UTF-8 sequence starting at *p.
 * Returns the Unicode codepoint and advances *p past the consumed bytes.
 * Unknown/invalid sequences return the byte value as-is and advance by 1. */
static uint32_t utf8_next(const char **p)
{
	const uint8_t *s = (const uint8_t *)(*p);
	uint32_t cp;

	if (s[0] < 0x80u) {
		cp = s[0];
		*p += 1;
	} else if ((s[0] & 0xE0u) == 0xC0u && (s[1] & 0xC0u) == 0x80u) {
		cp = ((uint32_t)(s[0] & 0x1Fu) << 6) | (s[1] & 0x3Fu);
		*p += 2;
	} else if ((s[0] & 0xF0u) == 0xE0u && (s[1] & 0xC0u) == 0x80u && (s[2] & 0xC0u) == 0x80u) {
		cp = ((uint32_t)(s[0] & 0x0Fu) << 12) | ((uint32_t)(s[1] & 0x3Fu) << 6) | (s[2] & 0x3Fu);
		*p += 3;
	} else {
		cp = s[0];
		*p += 1;
	}
	return cp;
}

/* Binary search for codepoint in sorted index table.
 * index_base = EEPROM address of first uint16_t codepoint entry.
 * count      = number of entries.
 * Returns glyph index, or 0xFFFFu if not found. */
static uint16_t cjk_find_glyph(uint16_t index_base, uint16_t count, uint16_t codepoint)
{
	uint16_t lo = 0, hi = count;
	while (lo < hi) {
		uint16_t mid  = (lo + hi) >> 1;
		uint16_t val;
		EEPROM_ReadBuffer(index_base + mid * 2u, &val, 2);
		if (val == codepoint)
			return mid;
		if (val < codepoint)
			lo = mid + 1;
		else
			hi = mid;
	}
	return 0xFFFFu;
}

/* Render one CJK glyph into the framebuffer at (col, Line).
 * Line occupies 2 pages (Line+0 = top 8 rows, Line+1 = bottom 8 rows). */
static void cjk_render_glyph(uint8_t col, uint8_t line, uint16_t glyph_data_addr)
{
	uint8_t glyph[CJK_BYTES_PER_GLYPH];
	EEPROM_ReadBuffer(glyph_data_addr, glyph, CJK_BYTES_PER_GLYPH);

	if (col + CJK_GLYPH_WIDTH > 128u)
		return;

	for (uint8_t c = 0; c < CJK_GLYPH_WIDTH; c++) {
		gFrameBuffer[line + 0][col + c] = glyph[c * 2 + 0];
		gFrameBuffer[line + 1][col + c] = glyph[c * 2 + 1];
	}
}

/* Public: Render a UTF-8 string (ASCII + CJK) into the framebuffer.
 * When the UI language is not CN, falls back to the standard ASCII renderer
 * (UI_PrintString) which handles Start/End centering correctly.
 * In CN mode, supports centering when End > Start. */
void UI_PrintStringMixed(const char *pString, uint8_t Start, uint8_t End, uint8_t Line)
{
	if (gUiLanguage != UI_LANGUAGE_CN) {
		UI_PrintString(pString, Start, End, Line, 8);
		return;
	}

	/* Read and validate the 8-byte font header from EEPROM:
	 *   [0-1] magic   0x4B36 ('K','6')  — blank EEPROM reads 0xFFFF
	 *   [2-3] count   glyph_count LE
	 *   [4]   version 1
	 *   [5]   reserved
	 *   [6-7] CRC-16/CCITT over bytes 0-5
	 * Any validation failure disables CJK rendering for this call. */
	uint16_t glyph_count;
	uint16_t index_base;
	uint16_t glyph_data_base;
	{
		uint8_t  hdr[8];
		uint16_t magic, crc_stored, idx_size;
		EEPROM_ReadBuffer(EEPROM_CJK_FONT_BASE, hdr, 8);
		magic      = (uint16_t)hdr[0] | ((uint16_t)hdr[1] << 8);
		crc_stored = (uint16_t)hdr[6] | ((uint16_t)hdr[7] << 8);
		if (magic  != CJK_FONT_MAGIC   ||
		    hdr[4] != CJK_FONT_VERSION ||
		    CRC_Calculate(hdr, 6) != crc_stored) {
			glyph_count = 0;
		} else {
			glyph_count = (uint16_t)hdr[2] | ((uint16_t)hdr[3] << 8);
			if (glyph_count > 512u)
				glyph_count = 0;
		}
		/* index table starts at 0x2008 (next 8-byte block after 8-byte header) */
		index_base  = EEPROM_CJK_FONT_BASE + 8u;
		idx_size    = glyph_count * 2u;
		glyph_data_base = index_base + ((idx_size + 7u) & ~7u);
	}

	/* Pre-pass: measure total pixel width for centering within Start..End. */
	uint8_t x = Start;
	if (End > Start) {
		uint16_t total_w = 0;
		const char *q = pString;
		while (*q) {
			uint32_t cp = utf8_next(&q);
			if (cp >= 0x4E00u && cp <= 0x9FFFu)
				total_w += CJK_GLYPH_WIDTH;
			else
				total_w += ASCII_GLYPH_WIDTH; /* ASCII, space, or unknown */
		}
		const uint16_t span = (uint16_t)(End - Start);
		if (total_w < span)
			x = (uint8_t)(Start + (span - total_w) / 2u);
	}

	/* Render pass: from computed x offset. */
	const char *p = pString;
	while (*p && x < 128u) {
		uint32_t cp = utf8_next(&p);

		if (cp >= 0x4E00u && cp <= 0x9FFFu && glyph_count > 0) {
			/* CJK Unified Ideographs */
			uint16_t gi = cjk_find_glyph(index_base, glyph_count, (uint16_t)cp);
			if (gi != 0xFFFFu) {
				uint16_t glyph_addr = glyph_data_base + gi * CJK_BYTES_PER_GLYPH;
				cjk_render_glyph(x, Line, glyph_addr);
			}
			x += CJK_GLYPH_WIDTH;
		} else if (cp > ' ' && cp < 127u) {
			/* ASCII printable using gFontBig (7 bytes per column-slice × 2 pages).
			 * Guard against overflowing the 128-byte framebuffer row: each glyph
			 * writes 7 bytes, so the start column must satisfy x + 7 <= 128. */
			if (x + 7u <= 128u) {
				const unsigned int index = (unsigned int)(cp - ' ' - 1u);
				memcpy(gFrameBuffer[Line + 0] + x, &gFontBig[index][0], 7);
				memcpy(gFrameBuffer[Line + 1] + x, &gFontBig[index][7], 7);
			}
			x += ASCII_GLYPH_WIDTH;
		} else {
			x += ASCII_GLYPH_WIDTH; /* space / unknown */
		}
	}
}

#endif  /* ENABLE_CHINESE */
