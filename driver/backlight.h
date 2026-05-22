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

#ifndef DRIVER_BACKLIGHT_H
#define DRIVER_BACKLIGHT_H

#include <stdint.h>
#include <stdbool.h>

extern uint16_t gBacklightCountdown_500ms;
extern uint8_t gBacklightBrightness;

// Countdown in 10ms ticks for the next brightness-step during fade-to-off.
// 0 = not fading.  Set to BACKLIGHT_FADE_STEP_10MS to begin a fade step.
#define BACKLIGHT_FADE_STEP_10MS  5   // 50 ms between steps → ~500 ms total fade
extern uint8_t gBacklightFadeCountdown10ms;

#ifdef ENABLE_BLMIN_TMP_OFF
typedef enum {
    BLMIN_STAT_ON,
    BLMIN_STAT_OFF,
    BLMIN_STAT_UNKNOWN
} BLMIN_STAT_t;
#endif

void BACKLIGHT_InitHardware();
void BACKLIGHT_TurnOn();
void BACKLIGHT_TurnOff();
void BACKLIGHT_StartFade(void);
bool BACKLIGHT_IsOn();
void BACKLIGHT_SetBrightness(uint8_t brigtness);
uint8_t BACKLIGHT_GetBrightness(void);

#endif
