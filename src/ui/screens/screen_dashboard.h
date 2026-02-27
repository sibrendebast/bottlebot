#pragma once

#include <lvgl.h>

void ui_screen_dashboard_init(void);
void ui_update_fermenter_data(uint8_t index, float current_temp, float target_temp, bool is_active, bool is_cooling, bool is_heating, float pid_value);
