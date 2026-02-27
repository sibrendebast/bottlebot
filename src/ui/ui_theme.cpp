#include "ui_theme.h"

// Define styles
lv_style_t style_screen_bg;
lv_style_t style_card;
lv_style_t style_header;
lv_style_t style_title;
lv_style_t style_label;
lv_style_t style_label_dim;
lv_style_t style_btn;
lv_style_t style_btn_primary;
lv_style_t style_switch_bg;
lv_style_t style_switch_knob;
lv_style_t style_bar_bg;
lv_style_t style_bar_indic;

void ui_theme_init(void) {
    // Screen Background (#f5f5f5)
    lv_style_init(&style_screen_bg);
    lv_style_set_bg_color(&style_screen_bg, COLOR_BG);
    lv_style_set_bg_opa(&style_screen_bg, LV_OPA_COVER);

    // Card (White bg, 2px black border, 0px radius, padding)
    lv_style_init(&style_card);
    lv_style_set_bg_color(&style_card, COLOR_CARD);
    lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
    lv_style_set_border_width(&style_card, 2);
    lv_style_set_border_color(&style_card, COLOR_BORDER);
    lv_style_set_radius(&style_card, 0);
    lv_style_set_pad_all(&style_card, 16); // sm:p-4 equivalent

    // Header (bottom border only)
    lv_style_init(&style_header);
    lv_style_set_border_width(&style_header, 0); // No full border
    lv_style_set_border_side(&style_header, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_width(&style_header, 2);
    lv_style_set_border_color(&style_header, COLOR_BORDER);
    lv_style_set_pad_bottom(&style_header, 16);
    lv_style_set_pad_top(&style_header, 0);
    lv_style_set_pad_left(&style_header, 0);
    lv_style_set_pad_right(&style_header, 0);
    lv_style_set_bg_opa(&style_header, LV_OPA_TRANSP);
    lv_style_set_radius(&style_header, 0);

    // Title (Large, bold text)
    lv_style_init(&style_title);
    lv_style_set_text_color(&style_title, COLOR_TEXT);
    lv_style_set_text_font(&style_title, &lv_font_montserrat_24);

    // Normal Label
    lv_style_init(&style_label);
    lv_style_set_text_color(&style_label, COLOR_TEXT);
    lv_style_set_text_font(&style_label, &lv_font_montserrat_16);

    // Dim Label
    lv_style_init(&style_label_dim);
    lv_style_set_text_color(&style_label_dim, COLOR_TEXT_DIM);
    lv_style_set_text_font(&style_label_dim, &lv_font_montserrat_14);

    // Base Button (White bg, black border, text black)
    lv_style_init(&style_btn);
    lv_style_set_bg_color(&style_btn, COLOR_CARD);
    lv_style_set_border_width(&style_btn, 2);
    lv_style_set_border_color(&style_btn, COLOR_BORDER);
    lv_style_set_radius(&style_btn, 0);
    lv_style_set_text_color(&style_btn, COLOR_TEXT);
    lv_style_set_text_font(&style_btn, &lv_font_montserrat_16);

    // Primary Button (Green bg, black border, white text)
    lv_style_init(&style_btn_primary);
    lv_style_set_bg_color(&style_btn_primary, COLOR_PRIMARY);
    lv_style_set_border_width(&style_btn_primary, 2);
    lv_style_set_border_color(&style_btn_primary, COLOR_BORDER);
    lv_style_set_radius(&style_btn_primary, 0);
    lv_style_set_text_color(&style_btn_primary, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_btn_primary, &lv_font_montserrat_16);

    // Switch Background (Gray, 2px border, 0px radius)
    lv_style_init(&style_switch_bg);
    lv_style_set_bg_color(&style_switch_bg, lv_color_hex(0xd1d5db)); // Gray 300
    lv_style_set_border_width(&style_switch_bg, 2);
    lv_style_set_border_color(&style_switch_bg, COLOR_BORDER);
    lv_style_set_radius(&style_switch_bg, 0);

    // Switch Knob (White inside, 1px black border to simulate the padding box)
    lv_style_init(&style_switch_knob);
    lv_style_set_bg_color(&style_switch_knob, COLOR_CARD);
    lv_style_set_border_width(&style_switch_knob, 1);
    lv_style_set_border_color(&style_switch_knob, COLOR_BORDER);
    lv_style_set_radius(&style_switch_knob, 0);
    lv_style_set_pad_all(&style_switch_knob, -2); // Inset slightly inside

    // PID Bar BG
    lv_style_init(&style_bar_bg);
    lv_style_set_bg_color(&style_bar_bg, lv_color_hex(0xe5e7eb)); // Gray 200
    lv_style_set_border_width(&style_bar_bg, 2);
    lv_style_set_border_color(&style_bar_bg, COLOR_BORDER);
    lv_style_set_radius(&style_bar_bg, 0);

    // PID Bar Indicator (Green)
    lv_style_init(&style_bar_indic);
    lv_style_set_bg_color(&style_bar_indic, COLOR_PRIMARY);
    lv_style_set_radius(&style_bar_indic, 0);
}

// Component Factory: Create standard Card
lv_obj_t* create_card(lv_obj_t *parent) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_remove_style_all(card); // Remove default theme padding/borders
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE); // Typically cards don't scroll
    return card;
}

// Component Factory: Create standard Button
lv_obj_t* create_button(lv_obj_t *parent, const char *text, bool is_primary) {
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_add_style(btn, is_primary ? &style_btn_primary : &style_btn, 0);
    
    // Add pressed state styling
    lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, is_primary ? lv_color_hex(0x45a882) : COLOR_BG); // Darker green or gray on press
    lv_obj_add_style(btn, &style_pr, LV_STATE_PRESSED);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_center(label);

    return btn;
}

// Component Factory: Create Toggle Switch matching Tailwind
lv_obj_t* create_switch(lv_obj_t *parent) {
    lv_obj_t *sw = lv_switch_create(parent);
    lv_obj_remove_style_all(sw);
    
    // Basic switch structural styles
    lv_obj_add_style(sw, &style_switch_bg, LV_PART_MAIN);
    // When checked, background turns Brewery Green
    lv_style_t style_sw_chk;
    lv_style_init(&style_sw_chk);
    lv_style_set_bg_color(&style_sw_chk, COLOR_PRIMARY);
    lv_obj_add_style(sw, &style_sw_chk, LV_PART_MAIN | LV_STATE_CHECKED);
    
    // Switch inner knob Indicator part
    lv_obj_add_style(sw, &style_switch_knob, LV_PART_INDICATOR);
    lv_obj_add_style(sw, &style_switch_knob, LV_PART_KNOB); // Explicitly style the knob too

    lv_obj_set_size(sw, 44, 24); // approx w-11 h-6
    return sw;
}
