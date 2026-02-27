#pragma once

// LVGL 8 Configuration Options
#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0
#define LV_TICK_CUSTOM 1
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (128U * 1024U)

// Enable default fonts
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_32 1

#define LV_USE_USER_DATA 1

// Enable Logging
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

// Use Arduino core functions for tick if LV_TICK_CUSTOM is 1
#if LV_TICK_CUSTOM
    #include <Arduino.h>
    #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif
