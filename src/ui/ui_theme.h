#pragma once

#include <lvgl.h>

#define COLOR_BG lv_color_hex(0xFFFFFF)       // White
#define COLOR_CARD lv_color_hex(0xFFFFFF)     // White
#define COLOR_BORDER lv_color_hex(0x000000)   // Black
#define COLOR_PRIMARY lv_color_hex(0x10B981)  // Emerald 500
#define COLOR_TEXT lv_color_hex(0x000000)     // Black
#define COLOR_TEXT_DIM lv_color_hex(0x64748B) // Slate 500
#define COLOR_WARNING lv_color_hex(0xF59E0B)  // Amber 500
#define COLOR_DANGER lv_color_hex(0xEF4444)   // Red 500
#define COLOR_ACCENT lv_color_hex(0x6366F1)   // Indigo 500

// Style holders
extern lv_style_t style_screen_bg;
extern lv_style_t style_card;
extern lv_style_t style_header;
extern lv_style_t style_title;
extern lv_style_t style_label;
extern lv_style_t style_label_dim;
extern lv_style_t style_btn;
extern lv_style_t style_btn_primary;
extern lv_style_t style_btn_warning;
extern lv_style_t style_btn_danger;
extern lv_style_t style_switch_bg;
extern lv_style_t style_switch_indic;
extern lv_style_t style_switch_knob;
extern lv_style_t style_bar_bg;
extern lv_style_t style_bar_indic;
extern lv_style_t style_bar_bg_vert;
extern lv_style_t style_bar_indic_vert;

// Initialization routine
void ui_theme_init(void);

// Reusable components
lv_obj_t *create_card(lv_obj_t *parent);
lv_obj_t *create_button(lv_obj_t *parent, const char *text,
                        int type); // 0=Normal, 1=Primary, 2=Warning, 3=Danger
lv_obj_t *create_switch(lv_obj_t *parent);
