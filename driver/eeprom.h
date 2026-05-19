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

#ifndef DRIVER_EEPROM_H
#define DRIVER_EEPROM_H

#include <stdint.h>

/* Physical chip: 512 KB external I2C EEPROM (community "V1" UV-K6 big-memory PCB).
 *
 * WARNING – the current driver uses uint16_t addresses and a fixed I2C device
 * byte (0xA0), so it can only reach the first 64 KB (page 0, 0x0000-0xFFFF).
 * EEPROM_SIZE reflects this accessible window, not the full chip capacity.
 * To unlock the remaining 448 KB the driver must be extended to encode the
 * three page-select bits into the I2C device byte and widen addresses to
 * uint32_t.
 *
 * Current layout (64 KB window):
 *   0x0000 – 0x1FFF  radio settings / channels
 *   0x2000 – 0xFFFF  CJK font data
 */
#define EEPROM_SIZE       0x10000u   /* currently accessible: 64 KB (page 0) */
#define EEPROM_FULL_SIZE  0x80000u   /* physical chip capacity: 512 KB        */
#define EEPROM_FONT_BASE  0x2000u

void EEPROM_ReadBuffer(uint16_t Address, void *pBuffer, uint8_t Size);
void EEPROM_WriteBuffer(uint16_t Address, const void *pBuffer);

#endif

