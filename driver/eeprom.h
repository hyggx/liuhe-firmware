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

/* AT24C512: 64 KB, addresses 0x0000-0xFFFF.
 * Settings occupy 0x0000-0x1FFF; CJK font starts at 0x2000. */
#define EEPROM_SIZE       0x10000u
#define EEPROM_FONT_BASE  0x2000u

void EEPROM_ReadBuffer(uint16_t Address, void *pBuffer, uint8_t Size);
void EEPROM_WriteBuffer(uint16_t Address, const void *pBuffer);

#endif

