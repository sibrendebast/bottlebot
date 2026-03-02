#pragma once

#include <lvgl.h>

// Global UI state references
extern lv_obj_t *lbl_status;
extern lv_obj_t *lbl_lifetime;
extern lv_obj_t *lbl_volume[4];
extern lv_obj_t *bar_progress[4];
extern lv_obj_t *led_bottle[4];

// Input fields for recipes
extern lv_obj_t *ta_fill_target;
extern lv_obj_t *ta_co2_time;

// Switches for manual
extern lv_obj_t *sw_beer[4];
extern lv_obj_t *sw_co2;
extern lv_obj_t *sw_head_lift;
extern lv_obj_t *sw_gate_entry;
extern lv_obj_t *sw_gate_exit;
extern lv_obj_t *sw_conveyor;

void ui_screen_dashboard_init(void);

// Data update functions for the backend to call
void ui_update_status(const char* status);
void ui_update_lifetime(uint32_t count);
void ui_update_head_data(uint8_t index, bool bottle_present, int volume_ml, int target_ml);
