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
lv_style_t style_btn_warning;
lv_style_t style_btn_danger;
lv_style_t style_switch_bg;
lv_style_t style_switch_indic;
lv_style_t style_switch_knob;
lv_style_t style_bar_bg;
lv_style_t style_bar_indic;
lv_style_t style_bar_bg_vert;
lv_style_t style_bar_indic_vert;

void ui_theme_init(void) {
  // Screen Background (#f5f5f5)
  lv_style_init(&style_screen_bg);
  lv_style_set_bg_color(&style_screen_bg, COLOR_BG);
  lv_style_set_bg_opa(&style_screen_bg, LV_OPA_COVER);

  // Card (White bg, square, border + shadow)
  lv_style_init(&style_card);
  lv_style_set_bg_color(&style_card, COLOR_CARD);
  lv_style_set_bg_opa(&style_card, LV_OPA_COVER);
  lv_style_set_radius(&style_card, 0);       // Back to square
  lv_style_set_border_width(&style_card, 2); // Black border
  lv_style_set_border_color(&style_card, COLOR_BORDER);
  lv_style_set_pad_all(&style_card, 16);

  // Header (no border, clean alignment)
  lv_style_init(&style_header);
  lv_style_set_radius(&style_header, 0);
  lv_style_set_bg_opa(&style_header, LV_OPA_TRANSP);
  lv_style_set_pad_bottom(&style_header, 8);

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

  // Base Button (Square, border)
  lv_style_init(&style_btn);
  lv_style_set_bg_color(&style_btn, COLOR_CARD);
  lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
  lv_style_set_border_width(&style_btn, 2);
  lv_style_set_border_color(&style_btn, COLOR_BORDER);
  lv_style_set_radius(&style_btn, 0); // Square
  lv_style_set_text_color(&style_btn, COLOR_TEXT);
  lv_style_set_text_font(&style_btn, &lv_font_montserrat_16);
  lv_style_set_shadow_width(&style_btn, 4);
  lv_style_set_shadow_opa(&style_btn, 10);
  lv_style_set_shadow_ofs_y(&style_btn, 2);

  // Primary Button (Emerald Green, square)
  lv_style_init(&style_btn_primary);
  lv_style_set_bg_color(&style_btn_primary, COLOR_PRIMARY);
  lv_style_set_bg_opa(&style_btn_primary, LV_OPA_COVER);
  lv_style_set_border_width(&style_btn_primary, 2);
  lv_style_set_border_color(&style_btn_primary, COLOR_BORDER);
  lv_style_set_radius(&style_btn_primary, 0); // Square
  lv_style_set_text_color(&style_btn_primary, lv_color_hex(0xffffff));
  lv_style_set_text_font(&style_btn_primary, &lv_font_montserrat_16);
  lv_style_set_shadow_width(&style_btn_primary, 10);
  lv_style_set_shadow_color(&style_btn_primary, COLOR_PRIMARY);
  lv_style_set_shadow_opa(&style_btn_primary, 40);
  lv_style_set_shadow_ofs_y(&style_btn_primary, 4);

  // Warning Button (Orange bg)
  lv_style_init(&style_btn_warning);
  lv_style_set_bg_color(&style_btn_warning, COLOR_WARNING);
  lv_style_set_bg_opa(&style_btn_warning, LV_OPA_COVER);
  lv_style_set_border_width(&style_btn_warning, 2);
  lv_style_set_border_color(&style_btn_warning, COLOR_BORDER);
  lv_style_set_radius(&style_btn_warning, 0);
  lv_style_set_text_color(&style_btn_warning, lv_color_hex(0xffffff));
  lv_style_set_text_font(&style_btn_warning, &lv_font_montserrat_16);

  // Danger Button (Red bg)
  lv_style_init(&style_btn_danger);
  lv_style_set_bg_color(&style_btn_danger, COLOR_DANGER);
  lv_style_set_bg_opa(&style_btn_danger, LV_OPA_COVER);
  lv_style_set_border_width(&style_btn_danger, 2);
  lv_style_set_border_color(&style_btn_danger, COLOR_BORDER);
  lv_style_set_radius(&style_btn_danger, 0);
  lv_style_set_text_color(&style_btn_danger, lv_color_hex(0xffffff));
  lv_style_set_text_font(&style_btn_danger, &lv_font_montserrat_16);

  // Switch Background (Red when OFF)
  lv_style_init(&style_switch_bg);
  lv_style_set_bg_color(&style_switch_bg, COLOR_DANGER);
  lv_style_set_bg_opa(&style_switch_bg, LV_OPA_COVER);
  lv_style_set_border_width(&style_switch_bg, 2);
  lv_style_set_border_color(&style_switch_bg, COLOR_BORDER);
  lv_style_set_radius(&style_switch_bg, 0);
  lv_style_set_pad_all(&style_switch_bg, 3); // Inset for the knob

  // Switch Indicator (Primary color when ON)
  lv_style_init(&style_switch_indic);
  lv_style_set_bg_color(&style_switch_indic, COLOR_PRIMARY);
  lv_style_set_bg_opa(&style_switch_indic, LV_OPA_COVER);
  lv_style_set_radius(&style_switch_indic, 0);

  // Switch Knob (Always White)
  lv_style_init(&style_switch_knob);
  lv_style_set_bg_color(&style_switch_knob, COLOR_CARD);
  lv_style_set_bg_opa(&style_switch_knob, LV_OPA_COVER);
  lv_style_set_border_width(&style_switch_knob, 2);
  lv_style_set_border_color(&style_switch_knob, COLOR_BORDER);
  lv_style_set_radius(&style_switch_knob, 0);

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

  // Vertical Bar BG
  lv_style_init(&style_bar_bg_vert);
  lv_style_set_bg_color(&style_bar_bg_vert, lv_color_hex(0xE2E8F0));
  lv_style_set_bg_opa(&style_bar_bg_vert, LV_OPA_COVER);
  lv_style_set_border_width(&style_bar_bg_vert, 2);
  lv_style_set_border_color(&style_bar_bg_vert, COLOR_BORDER);
  lv_style_set_radius(&style_bar_bg_vert, 0); // Square
  lv_style_set_pad_all(&style_bar_bg_vert, 2);

  // Vertical Bar Indicator
  lv_style_init(&style_bar_indic_vert);
  lv_style_set_bg_color(&style_bar_indic_vert, COLOR_PRIMARY);
  lv_style_set_bg_opa(&style_bar_indic_vert, LV_OPA_COVER);
  lv_style_set_radius(&style_bar_indic_vert, 0); // Square
}

// Component Factory: Create standard Card
lv_obj_t *create_card(lv_obj_t *parent) {
  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_remove_style_all(card); // Remove default theme padding/borders
  lv_obj_add_style(card, &style_card, 0);
  lv_obj_clear_flag(card,
                    LV_OBJ_FLAG_SCROLLABLE); // Typically cards don't scroll
  return card;
}

// Component Factory: Create standard Button
lv_obj_t *create_button(lv_obj_t *parent, const char *text, int type) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_remove_style_all(btn);

  lv_style_t *base_style;
  lv_color_t text_color = COLOR_TEXT;

  if (type == 1) {
    base_style = &style_btn_primary;
    text_color = lv_color_hex(0xffffff);
  } else if (type == 2) {
    base_style = &style_btn_warning;
    text_color = lv_color_hex(0xffffff);
  } else if (type == 3) {
    base_style = &style_btn_danger;
    text_color = lv_color_hex(0xffffff);
  } else {
    base_style = &style_btn;
    text_color = COLOR_TEXT;
  }

  lv_obj_add_style(btn, base_style, 0);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_color(label, text_color, 0);
  lv_obj_center(label);

  return btn;
}

// Component Factory: Create Toggle Switch matching Tailwind
lv_obj_t *create_switch(lv_obj_t *parent) {
  lv_obj_t *sw = lv_switch_create(parent);
  lv_obj_remove_style_all(sw);

  // Basic switch structural styles
  lv_obj_add_style(sw, &style_switch_bg, LV_PART_MAIN);
  // When checked, background turns Brewery Green
  lv_obj_add_style(sw, &style_switch_indic,
                   LV_PART_INDICATOR | LV_STATE_CHECKED);
  // Clean White Knob
  lv_obj_add_style(sw, &style_switch_knob, LV_PART_KNOB);

  lv_obj_set_size(sw, 58, 28);
  return sw;
}
