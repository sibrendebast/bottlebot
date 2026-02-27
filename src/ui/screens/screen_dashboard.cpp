#include "screen_dashboard.h"
#include "../ui_theme.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_HEADS 4

lv_obj_t *lbl_status;
lv_obj_t *lbl_lifetime;
lv_obj_t *lbl_volume[4];
lv_obj_t *bar_progress[4];
lv_obj_t *led_bottle[4];

lv_obj_t *ta_fill_target;
lv_obj_t *ta_co2_time;

lv_obj_t *ta_head_ticks[4];

lv_obj_t *sw_beer[4];
lv_obj_t *sw_co2;
lv_obj_t *sw_head_lift;
lv_obj_t *sw_gate;
lv_obj_t *sw_conveyor;

static lv_obj_t *panel_run;
static lv_obj_t *panel_manual;
static lv_obj_t *panel_calibrate;
static lv_obj_t *btn_nav_run;
static lv_obj_t *btn_nav_manual;
static lv_obj_t *btn_nav_calibrate;
static lv_obj_t *kb;

static void update_nav_styles(lv_obj_t *active_btn) {
  lv_obj_t *btns[] = {btn_nav_run, btn_nav_manual, btn_nav_calibrate};
  for (int i = 0; i < 3; i++) {
    if (btns[i] == active_btn) {
      lv_obj_set_style_bg_color(btns[i], lv_color_hex(0xECFDF5),
                                0); // Emerald 50
      lv_obj_set_style_border_color(btns[i], COLOR_PRIMARY, 0);
    } else {
      lv_obj_set_style_bg_color(btns[i], COLOR_CARD, 0);
      lv_obj_set_style_border_color(btns[i], COLOR_BORDER, 0);
    }
  }
}

static void nav_btn_event_cb(lv_event_t *e) {
  lv_obj_t *btn = lv_event_get_target(e);
  lv_obj_t *target_panel = (lv_obj_t *)lv_event_get_user_data(e);

  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    lv_obj_add_flag(panel_run, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(panel_manual, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(panel_calibrate, LV_OBJ_FLAG_HIDDEN);

    lv_obj_clear_flag(target_panel, LV_OBJ_FLAG_HIDDEN);
    update_nav_styles(btn);
  }
}

static void ta_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);
  if (code == LV_EVENT_FOCUSED) {
    if (kb != NULL) {
      lv_keyboard_set_textarea(kb, ta);
      lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
      lv_obj_move_foreground(kb);
    }
  }
  if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY ||
      code == LV_EVENT_CANCEL) {
    if (kb != NULL)
      lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
  }
}

static void build_run_screen(lv_obj_t *parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(parent, 10, 0);
  lv_obj_set_style_pad_gap(parent, 10, 0);

  // Top Header
  lv_obj_t *header = lv_obj_create(parent);
  lv_obj_remove_style_all(header);
  lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_layout(header, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lbl_status = lv_label_create(header);
  lv_label_set_text(lbl_status, "STATUS: IDLE");
  lv_obj_add_style(lbl_status, &style_title, 0);
  lv_obj_set_style_text_font(lbl_status, &lv_font_montserrat_16, 0);

  lbl_lifetime = lv_label_create(header);
  lv_label_set_text(lbl_lifetime, "Total: 0 bottles");
  lv_obj_add_style(lbl_lifetime, &style_title, 0);
  lv_obj_set_style_text_font(lbl_lifetime, &lv_font_montserrat_16, 0);

  // Center Stage (4 Heads)
  lv_obj_t *center = lv_obj_create(parent);
  lv_obj_remove_style_all(center);
  lv_obj_set_size(center, lv_pct(100), 300); // Fixed height to prevent overlap
  lv_obj_set_flex_grow(center, 1);
  lv_obj_set_layout(center, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(center, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(center, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  for (int i = 0; i < NUM_HEADS; i++) {
    lv_obj_t *card = create_card(center);
    lv_obj_set_size(card, lv_pct(23),
                    lv_pct(100)); // slightly less than 25% for gaps
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Bottle Icon (Larger, Glowing)
    led_bottle[i] = lv_led_create(card);
    lv_obj_set_size(led_bottle[i], 40, 40);
    lv_led_set_color(led_bottle[i], COLOR_PRIMARY);
    lv_led_off(led_bottle[i]);

    // Vertical Bar (Sleeker)
    bar_progress[i] = lv_bar_create(card);
    lv_obj_remove_style_all(bar_progress[i]);
    lv_obj_add_style(bar_progress[i], &style_bar_bg_vert, LV_PART_MAIN);
    lv_obj_add_style(bar_progress[i], &style_bar_indic_vert, LV_PART_INDICATOR);
    lv_obj_set_size(bar_progress[i], 32, lv_pct(65));
    lv_bar_set_range(bar_progress[i], 0, 330);
    lv_bar_set_value(bar_progress[i], 0, LV_ANIM_OFF);

    // Volume Text
    lbl_volume[i] = lv_label_create(card);
    lv_label_set_text(lbl_volume[i], "0 / 330 mL");
    lv_obj_add_style(lbl_volume[i], &style_label, 0);
  }

  // Bottom Footer
  lv_obj_t *footer = lv_obj_create(parent);
  lv_obj_remove_style_all(footer);
  lv_obj_set_size(footer, lv_pct(100), 80);
  lv_obj_set_layout(footer, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t *btn_start = create_button(footer, "START CYCLE", 1); // Primary
  lv_obj_set_size(btn_start, lv_pct(48), 60);

  lv_obj_t *btn_pause = create_button(footer, "PAUSE", 2); // Warning
  lv_obj_set_size(btn_pause, lv_pct(48), 60);
}

static void build_manual_screen(lv_obj_t *parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(parent, 10, 0);

  lv_obj_t *title = lv_label_create(parent);
  lv_label_set_text(title, "Maintenance / Overrides");
  lv_obj_add_style(title, &style_title, 0);
  lv_obj_set_style_pad_bottom(title, 10, 0);

  lv_obj_t *grid = lv_obj_create(parent);
  lv_obj_remove_style_all(grid);
  lv_obj_set_size(grid, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_layout(grid, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START);

  // Valves
  lv_obj_t *c_valves = create_card(grid);
  lv_obj_set_size(c_valves, 312, LV_SIZE_CONTENT); // +20% (260 * 1.2)
  lv_obj_set_layout(c_valves, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(c_valves, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(c_valves, 12, 0);
  lv_obj_set_style_pad_gap(c_valves, 2, 0); // Tighter gap between rows

  lv_obj_t *l_valves = lv_label_create(c_valves);
  lv_label_set_text(l_valves, "Solenoids");
  lv_obj_add_style(l_valves, &style_title, 0);
  lv_obj_set_style_text_font(l_valves, &lv_font_montserrat_16, 0);
  lv_obj_set_style_pad_bottom(l_valves, 8, 0);

  for (int i = 0; i < NUM_HEADS; i++) {
    lv_obj_t *row = lv_obj_create(c_valves);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT); // FIX: Content height
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(row, 6,
                             0); // +50% actually, but for better touch target
    lv_obj_t *lbl = lv_label_create(row);
    lv_label_set_text_fmt(lbl, "Beer Valve %d", i + 1);
    sw_beer[i] = create_switch(row);
  }
  lv_obj_t *row_co2 = lv_obj_create(c_valves);
  lv_obj_remove_style_all(row_co2);
  lv_obj_set_size(row_co2, lv_pct(100), LV_SIZE_CONTENT); // FIX: Content height
  lv_obj_set_layout(row_co2, LV_LAYOUT_FLEX);
  lv_obj_set_flex_align(row_co2, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_ver(row_co2, 6, 0);
  lv_obj_t *lbl_co2 = lv_label_create(row_co2);
  lv_label_set_text(lbl_co2, "Master CO2");
  sw_co2 = create_switch(row_co2);

  // Mechanicals
  lv_obj_t *c_mech = create_card(grid);
  lv_obj_set_size(c_mech, 312, LV_SIZE_CONTENT);
  lv_obj_set_layout(c_mech, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(c_mech, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(c_mech, 12, 0);
  lv_obj_set_style_pad_gap(c_mech, 2, 0);

  lv_obj_t *l_mech = lv_label_create(c_mech);
  lv_label_set_text(l_mech, "Mechanicals");
  lv_obj_add_style(l_mech, &style_title, 0);
  lv_obj_set_style_text_font(l_mech, &lv_font_montserrat_16, 0);
  lv_obj_set_style_pad_bottom(l_mech, 8, 0);

  lv_obj_t *row_lift = lv_obj_create(c_mech);
  lv_obj_remove_style_all(row_lift);
  lv_obj_set_width(row_lift, lv_pct(100));
  lv_obj_set_layout(row_lift, LV_LAYOUT_FLEX);
  lv_obj_set_flex_align(row_lift, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_t *lbl_lift = lv_label_create(row_lift);
  lv_label_set_text(lbl_lift, "Head Lift");
  sw_head_lift = create_switch(row_lift);

  lv_obj_t *row_gate = lv_obj_create(c_mech);
  lv_obj_remove_style_all(row_gate);
  lv_obj_set_width(row_gate, lv_pct(100));
  lv_obj_set_layout(row_gate, LV_LAYOUT_FLEX);
  lv_obj_set_flex_align(row_gate, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_t *lbl_gate = lv_label_create(row_gate);
  lv_label_set_text(lbl_gate, "Gate");
  sw_gate = create_switch(row_gate);

  lv_obj_t *row_conv = lv_obj_create(c_mech);
  lv_obj_remove_style_all(row_conv);
  lv_obj_set_width(row_conv, lv_pct(100));
  lv_obj_set_layout(row_conv, LV_LAYOUT_FLEX);
  lv_obj_set_flex_align(row_conv, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_t *lbl_conv = lv_label_create(row_conv);
  lv_label_set_text(lbl_conv, "Conveyor");
  lv_obj_t *rows_mech[] = {row_lift, row_gate, row_conv};
  for (int i = 0; i < 3; i++) {
    lv_obj_set_size(rows_mech[i], lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_ver(rows_mech[i], 4, 0);
  }
}

static void calib_adj_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = lv_event_get_target(e);
  lv_obj_t *ta = (lv_obj_t *)lv_event_get_user_data(e);
  intptr_t dir = (intptr_t)lv_obj_get_user_data(btn); // +1 or -1

  if (code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
    const char *txt = lv_textarea_get_text(ta);
    int val = atoi(txt);
    val += (int)dir;
    if (val < 0)
      val = 0;
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", val);
    lv_textarea_set_text(ta, buf);
  }
}

static void build_calibrate_screen(lv_obj_t *parent) {
  lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(parent, 20, 0);

  lv_obj_t *title = lv_label_create(parent);
  lv_label_set_text(title, "Flow Sensor Calibration");
  lv_obj_add_style(title, &style_title, 0);
  lv_obj_set_style_pad_bottom(title, 10, 0);

  lv_obj_t *subtitle = lv_label_create(parent);
  lv_label_set_text(subtitle, "Set total ticks for a 330 mL fill");
  lv_obj_add_style(subtitle, &style_label_dim, 0);
  lv_obj_set_style_pad_bottom(subtitle, 30, 0);

  // General Settings (CO2) - Now at the Top
  lv_obj_t *grid_settings = lv_obj_create(parent);
  lv_obj_remove_style_all(grid_settings);
  lv_obj_set_size(grid_settings, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_layout(grid_settings, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(grid_settings, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(grid_settings, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(grid_settings, 20, 0);
  lv_obj_set_style_pad_bottom(grid_settings, 30, 0); // Space to grid below

  lv_obj_t *c_co2 = create_card(grid_settings);
  lv_obj_set_size(c_co2, 320, LV_SIZE_CONTENT);
  lv_obj_set_layout(c_co2, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(c_co2, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(c_co2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(c_co2, 12, 0);

  lv_obj_t *l_co2_title = lv_label_create(c_co2);
  lv_label_set_text(l_co2_title, "CO2 Flush (ms)");
  lv_obj_add_style(l_co2_title, &style_label, 0);
  lv_obj_set_style_text_font(l_co2_title, &lv_font_montserrat_14, 0);

  lv_obj_t *row_co2 = lv_obj_create(c_co2);
  lv_obj_remove_style_all(row_co2);
  lv_obj_set_size(row_co2, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_layout(row_co2, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(row_co2, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row_co2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(row_co2, 15, 0);

  lv_obj_t *btn_co2_minus = create_button(row_co2, "-", 0);
  lv_obj_set_size(btn_co2_minus, 50, 50);
  lv_obj_set_style_radius(btn_co2_minus, 25, 0);
  lv_obj_set_user_data(btn_co2_minus, (void *)(intptr_t)-500);

  ta_co2_time = lv_textarea_create(row_co2);
  lv_textarea_set_one_line(ta_co2_time, true);
  lv_textarea_set_text(ta_co2_time, "2500");
  lv_obj_set_width(ta_co2_time, 100);
  lv_obj_set_style_border_width(ta_co2_time, 0, 0);
  lv_obj_set_style_bg_color(ta_co2_time, COLOR_BG, 0);
  lv_obj_set_style_radius(ta_co2_time, 8, 0);
  lv_obj_set_style_text_align(ta_co2_time, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_font(ta_co2_time, &lv_font_montserrat_24, 0);
  lv_obj_add_event_cb(ta_co2_time, ta_event_cb, LV_EVENT_ALL, NULL);

  lv_obj_t *btn_co2_plus = create_button(row_co2, "+", 0);
  lv_obj_set_size(btn_co2_plus, 50, 50);
  lv_obj_set_style_radius(btn_co2_plus, 25, 0);
  lv_obj_set_user_data(btn_co2_plus, (void *)(intptr_t)500);

  lv_obj_add_event_cb(btn_co2_plus, calib_adj_event_cb, LV_EVENT_ALL,
                      ta_co2_time);
  lv_obj_add_event_cb(btn_co2_minus, calib_adj_event_cb, LV_EVENT_ALL,
                      ta_co2_time);

  // Head Calibration Ticks Grid - Now below CO2
  lv_obj_t *grid = lv_obj_create(parent);
  lv_obj_remove_style_all(grid);
  lv_obj_set_size(grid, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_layout(grid, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_gap(grid, 10, 0);

  for (int i = 0; i < NUM_HEADS; i++) {
    lv_obj_t *card = create_card(grid);
    lv_obj_set_size(card, 160, LV_SIZE_CONTENT);
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(card, 10, 0);

    lv_obj_t *l_head = lv_label_create(card);
    lv_label_set_text_fmt(l_head, "HEAD %d", i + 1);
    lv_obj_add_style(l_head, &style_title, 0);
    lv_obj_set_style_text_font(l_head, &lv_font_montserrat_14, 0);

    lv_obj_t *btn_plus = create_button(card, "+", 0);
    lv_obj_set_size(btn_plus, 50, 50);
    lv_obj_set_style_radius(btn_plus, 25, 0);
    lv_obj_set_user_data(btn_plus, (void *)(intptr_t)1);

    ta_head_ticks[i] = lv_textarea_create(card);
    lv_textarea_set_one_line(ta_head_ticks[i], true);
    lv_textarea_set_text(ta_head_ticks[i], "1000");
    lv_obj_set_width(ta_head_ticks[i], 100);
    lv_obj_set_style_border_width(ta_head_ticks[i], 0, 0);
    lv_obj_set_style_bg_color(ta_head_ticks[i], COLOR_BG, 0);
    lv_obj_set_style_radius(ta_head_ticks[i], 8, 0);
    lv_obj_set_style_text_align(ta_head_ticks[i], LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(ta_head_ticks[i], &lv_font_montserrat_24, 0);
    lv_obj_add_event_cb(ta_head_ticks[i], ta_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *btn_minus = create_button(card, "-", 0);
    lv_obj_set_size(btn_minus, 50, 50);
    lv_obj_set_style_radius(btn_minus, 25, 0);
    lv_obj_set_user_data(btn_minus, (void *)(intptr_t)-1);

    lv_obj_add_event_cb(btn_plus, calib_adj_event_cb, LV_EVENT_ALL,
                        ta_head_ticks[i]);
    lv_obj_add_event_cb(btn_minus, calib_adj_event_cb, LV_EVENT_ALL,
                        ta_head_ticks[i]);

    lv_obj_t *l_unit = lv_label_create(card);
    lv_label_set_text(l_unit, "Ticks / 330ml");
    lv_obj_add_style(l_unit, &style_label_dim, 0);
    lv_obj_set_style_text_font(l_unit, &lv_font_montserrat_14, 0);
  }

  // spacer and bottom button
  lv_obj_t *spacer_bottom = lv_obj_create(parent);
  lv_obj_set_size(spacer_bottom, 1, 1);
  lv_obj_set_flex_grow(spacer_bottom, 1);

  lv_obj_t *btn_save = create_button(parent, "Save Settings", 1);
  lv_obj_set_size(btn_save, 460, 60);
  lv_obj_set_style_pad_bottom(parent, 20, 0);
}

void ui_screen_dashboard_init(void) {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_add_style(scr, &style_screen_bg, 0);
  lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_pad_all(scr, 24, 0); // Global gutter
  lv_obj_set_style_pad_gap(scr, 24, 0); // Gap between sidebar and content

  // Global Keyboard
  kb = lv_keyboard_create(scr);
  lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

  // Left Navigation Bar
  lv_obj_t *nav_bar = lv_obj_create(scr);
  lv_obj_remove_style_all(nav_bar);
  lv_obj_set_size(nav_bar, 120, lv_pct(100));
  lv_obj_set_style_bg_color(nav_bar, COLOR_CARD, 0);
  lv_obj_set_style_border_width(nav_bar, 0, 0);

  lv_obj_set_layout(nav_bar, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(nav_bar, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(nav_bar, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(nav_bar, 0, 0); // Flush alignment
  lv_obj_set_style_pad_gap(nav_bar, 12, 0);

  btn_nav_run = create_button(nav_bar, "RUN", 0);
  lv_obj_set_size(btn_nav_run, lv_pct(100), 72);

  btn_nav_manual = create_button(nav_bar, "MANUAL", 0);
  lv_obj_set_size(btn_nav_manual, lv_pct(100), 72);

  btn_nav_calibrate = create_button(nav_bar, "CALIB.", 0);
  lv_obj_set_size(btn_nav_calibrate, lv_pct(100), 72);

  // Spacer for E-Stop
  lv_obj_t *spacer = lv_obj_create(nav_bar);
  lv_obj_set_size(spacer, 1, 1);
  lv_obj_set_flex_grow(spacer, 1);

  lv_obj_t *btn_estop_nav = create_button(nav_bar, "E-STOP", 3);
  lv_obj_set_size(btn_estop_nav, lv_pct(100), 72);

  // Initial style highlight
  update_nav_styles(btn_nav_run);

  // Main Content Area (Wrapped in a box with border)
  lv_obj_t *content = lv_obj_create(scr);
  lv_obj_remove_style_all(content);
  lv_obj_add_style(content, &style_card, 0);          // BOX with border
  lv_obj_set_size(content, lv_pct(100), lv_pct(100)); // Fills available space
  lv_obj_set_flex_grow(content, 1);
  lv_obj_set_style_pad_all(content, 20, 0); // Internal spacing
  lv_obj_set_style_pad_gap(content, 20, 0);

  // Create 3 overlapping panels inside content Area
  panel_run = lv_obj_create(content);
  lv_obj_remove_style_all(panel_run);
  lv_obj_set_size(panel_run, lv_pct(100), lv_pct(100));
  build_run_screen(panel_run);

  panel_manual = lv_obj_create(content);
  lv_obj_remove_style_all(panel_manual);
  lv_obj_set_size(panel_manual, lv_pct(100), lv_pct(100));
  build_manual_screen(panel_manual);
  lv_obj_add_flag(panel_manual, LV_OBJ_FLAG_HIDDEN);

  panel_calibrate = lv_obj_create(content);
  lv_obj_remove_style_all(panel_calibrate);
  lv_obj_set_size(panel_calibrate, lv_pct(100), lv_pct(100));
  build_calibrate_screen(panel_calibrate);
  lv_obj_add_flag(panel_calibrate, LV_OBJ_FLAG_HIDDEN);

  // Assign events
  lv_obj_add_event_cb(btn_nav_run, nav_btn_event_cb, LV_EVENT_ALL, panel_run);
  lv_obj_add_event_cb(btn_nav_manual, nav_btn_event_cb, LV_EVENT_ALL,
                      panel_manual);
  lv_obj_add_event_cb(btn_nav_calibrate, nav_btn_event_cb, LV_EVENT_ALL,
                      panel_calibrate);
}

void ui_update_status(const char *status) {
  if (lbl_status) {
    char buf[64];
    snprintf(buf, sizeof(buf), "STATUS: %s", status);
    lv_label_set_text(lbl_status, buf);
  }
}

void ui_update_lifetime(uint32_t count) {
  if (lbl_lifetime) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Total: %u bottles", count);
    lv_label_set_text(lbl_lifetime, buf);
  }
}

void ui_update_head_data(uint8_t index, bool bottle_present, int volume_ml,
                         int target_ml) {
  if (index >= NUM_HEADS)
    return;

  if (bottle_present) {
    lv_led_on(led_bottle[index]);
  } else {
    lv_led_off(led_bottle[index]);
  }

  lv_bar_set_range(bar_progress[index], 0, target_ml);
  lv_bar_set_value(bar_progress[index], volume_ml, LV_ANIM_ON);

  char buf[32];
  snprintf(buf, sizeof(buf), "%d / %d mL", volume_ml, target_ml);
  lv_label_set_text(lbl_volume[index], buf);
}

void ui_update_calibration(uint8_t index, uint32_t ticks) {
  if (index >= NUM_HEADS)
    return;
  if (ta_head_ticks[index]) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%u", ticks);
    lv_textarea_set_text(ta_head_ticks[index], buf);
  }
}
