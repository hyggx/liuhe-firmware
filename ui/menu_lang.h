/* Copyright 2024 egzumer / Hygg
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Runtime English / Simplified-Chinese language support.
 * Enabled by compiling with -DENABLE_CHINESE.
 */

#ifndef UI_MENU_LANG_H
#define UI_MENU_LANG_H

#ifdef ENABLE_CHINESE

#include "ui/menu.h"    // t_menu_item

/* Returns a pointer to the localised menu-title string for `item`.
 * In CN mode:  returns a Chinese UTF-8 string literal.
 * In EN mode:  returns item->name (the 6-char ASCII short name). */
const char *UI_MENU_GetMenuTitle(const t_menu_item *item);

#endif  /* ENABLE_CHINESE */
#endif  /* UI_MENU_LANG_H */
