#include "screen_dashboard.h"
#include "../ui_theme.h"
#include <stdio.h>

#define NUM_FERMENTERS 3

// UI Elements storage to update them later
static lv_obj_t *temp_labels[NUM_FERMENTERS];
static lv_obj_t *target_labels[NUM_FERMENTERS];
static lv_obj_t *status_switches[NUM_FERMENTERS];
static lv_obj_t *pid_bars[NUM_FERMENTERS];
static lv_obj_t *pid_labels[NUM_FERMENTERS];

// Dummy callback for the +/- buttons
static void target_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        int index = (int)(ptrdiff_t)lv_event_get_user_data(e);
        // Logic to increase/decrease target temp would go here
        // For now, just a placeholder
    }
}

static void create_fermenter_card(lv_obj_t *parent, int index) {
    lv_obj_t *card = create_card(parent);
    lv_obj_set_width(card, lv_pct(100));
    // Let layout handle height based on content
    lv_obj_set_layout(card, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Header Row: Title and Switch
    lv_obj_t *header_row = lv_obj_create(card);
    lv_obj_remove_style_all(header_row);
    lv_obj_set_width(header_row, lv_pct(100));
    lv_obj_set_height(header_row, LV_SIZE_CONTENT);
    lv_obj_set_layout(header_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(header_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(header_row, &style_header, 0); // Bottom border
    lv_obj_set_style_pad_bottom(header_row, 10, 0);
    lv_obj_set_style_margin_bottom(header_row, 15, 0);

    lv_obj_t *title = lv_label_create(header_row);
    char buf[32];
    snprintf(buf, sizeof(buf), "Fermenter %d", index + 1);
    lv_label_set_text(title, buf);
    lv_obj_add_style(title, &style_title, 0);

    status_switches[index] = create_switch(header_row);

    // Current Temp Row
    lv_obj_t *temp_row = lv_obj_create(card);
    lv_obj_remove_style_all(temp_row);
    lv_obj_set_width(temp_row, lv_pct(100));
    lv_obj_set_height(temp_row, LV_SIZE_CONTENT);
    lv_obj_set_layout(temp_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(temp_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(temp_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_margin_bottom(temp_row, 15, 0);

    lv_obj_t *temp_label_title = lv_label_create(temp_row);
    lv_label_set_text(temp_label_title, "Temperature:");
    lv_obj_add_style(temp_label_title, &style_label, 0);
    lv_obj_set_style_text_font(temp_label_title, &lv_font_montserrat_16, 0);

    temp_labels[index] = lv_label_create(temp_row);
    lv_label_set_text(temp_labels[index], "--.- °C");
    lv_obj_add_style(temp_labels[index], &style_title, 0); // Use large text
    lv_obj_set_style_text_color(temp_labels[index], COLOR_PRIMARY, 0); // Green color

    // PID Bar Container
    lv_obj_t *pid_container = lv_obj_create(card);
    lv_obj_remove_style_all(pid_container);
    lv_obj_set_width(pid_container, lv_pct(100));
    lv_obj_set_height(pid_container, LV_SIZE_CONTENT);
    lv_obj_set_style_margin_bottom(pid_container, 15, 0);

    pid_bars[index] = lv_bar_create(pid_container);
    lv_obj_remove_style_all(pid_bars[index]);
    lv_obj_add_style(pid_bars[index], &style_bar_bg, LV_PART_MAIN);
    lv_obj_add_style(pid_bars[index], &style_bar_indic, LV_PART_INDICATOR);
    lv_obj_set_size(pid_bars[index], lv_pct(100), 16);
    lv_bar_set_range(pid_bars[index], -100, 100);
    lv_bar_set_value(pid_bars[index], 0, LV_ANIM_OFF);
    // Draw center line manually by adding a 2px black line in the middle if possible, or just skip for simplicity
    
    lv_obj_t *pid_info_row = lv_obj_create(pid_container);
    lv_obj_remove_style_all(pid_info_row);
    lv_obj_set_width(pid_info_row, lv_pct(100));
    lv_obj_set_layout(pid_info_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(pid_info_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(pid_info_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(pid_info_row, LV_ALIGN_TOP_MID, 0, 20); // Below bar

    lv_obj_t *cool_lbl = lv_label_create(pid_info_row);
    lv_label_set_text(cool_lbl, "Cooling");
    lv_obj_add_style(cool_lbl, &style_label_dim, 0);

    pid_labels[index] = lv_label_create(pid_info_row);
    lv_label_set_text(pid_labels[index], "0%");
    lv_obj_add_style(pid_labels[index], &style_label_dim, 0);

    lv_obj_t *heat_lbl = lv_label_create(pid_info_row);
    lv_label_set_text(heat_lbl, "Heating");
    lv_obj_add_style(heat_lbl, &style_label_dim, 0);

    // Target Temp Controls Row
    lv_obj_t *ctrl_row = lv_obj_create(card);
    lv_obj_remove_style_all(ctrl_row);
    lv_obj_set_width(ctrl_row, lv_pct(100));
    lv_obj_set_height(ctrl_row, LV_SIZE_CONTENT);
    lv_obj_set_layout(ctrl_row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(ctrl_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ctrl_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *btn_minus = create_button(ctrl_row, "-", false);
    lv_obj_set_size(btn_minus, 56, 56); // w-14 h-14
    lv_obj_add_event_cb(btn_minus, target_btn_event_cb, LV_EVENT_ALL, (void *)(ptrdiff_t)index);

    target_labels[index] = lv_label_create(ctrl_row);
    lv_label_set_text(target_labels[index], "18.0");
    lv_obj_add_style(target_labels[index], &style_title, 0);
    lv_obj_set_style_text_align(target_labels[index], LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *btn_plus = create_button(ctrl_row, "+", false);
    lv_obj_set_size(btn_plus, 56, 56);
    lv_obj_add_event_cb(btn_plus, target_btn_event_cb, LV_EVENT_ALL, (void *)(ptrdiff_t)index);
}


void ui_screen_dashboard_init(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_add_style(scr, &style_screen_bg, 0);

    // Top Header container mimicking HTML <header>
    lv_obj_t *header = lv_obj_create(scr);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(header, &style_header, 0); 
    lv_obj_set_style_pad_all(header, 16, 0); // Outer padding
    
    // Header layout
    lv_obj_set_layout(header, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *app_title = lv_label_create(header);
    lv_label_set_text(app_title, "Bottle Machine (FermController)");
    lv_obj_add_style(app_title, &style_title, 0);
    // Extra bold feel
    lv_obj_set_style_text_font(app_title, &lv_font_montserrat_24, 0);

    // Right side actions container
    lv_obj_t *actions = lv_obj_create(header);
    lv_obj_remove_style_all(actions);
    lv_obj_set_size(actions, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_layout(actions, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(actions, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(actions, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(actions, 10, 0); // space-x-2 ~ 8px

    // Fake Buttons for Settings/Graphs
    lv_obj_t *btn_graph = create_button(actions, "G", false);
    lv_obj_set_size(btn_graph, 40, 40); // w-10 h-10
    lv_obj_t *btn_settings = create_button(actions, "S", false);
    lv_obj_set_size(btn_settings, 40, 40);

    // Main Grid Container for Fermenters
    // Wrapping grid since LVGL grid can be complex, flex is easier
    lv_obj_t *grid = lv_obj_create(scr);
    lv_obj_remove_style_all(grid);
    lv_obj_set_size(grid, lv_pct(100), lv_pct(100)); // Take rest of screen
    lv_obj_align(grid, LV_ALIGN_TOP_MID, 0, 70); // Offset below header
    lv_obj_set_style_pad_all(grid, 16, 0);
    
    // Flex layout mimicking grid-cols-3
    lv_obj_set_layout(grid, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    for(int i = 0; i < NUM_FERMENTERS; i++) {
        // Create wrapper to enforce width ~ 32% (3 columns with gap)
        lv_obj_t *col = lv_obj_create(grid);
        lv_obj_remove_style_all(col);
        lv_obj_set_size(col, lv_pct(32), LV_SIZE_CONTENT);
        create_fermenter_card(col, i);
    }
}

void ui_update_fermenter_data(uint8_t index, float current_temp, float target_temp, bool is_active, bool is_cooling, bool is_heating, float pid_value) {
    if (index >= NUM_FERMENTERS) return;

    // Update Temp
    char buf[16];
    if (current_temp < -50.0f) { // Dummy value for disabled/error
        lv_label_set_text(temp_labels[index], "NC");
    } else {
        snprintf(buf, sizeof(buf), "%.1f °C", current_temp);
        lv_label_set_text(temp_labels[index], buf);
    }

    // Update Target
    snprintf(buf, sizeof(buf), "%.1f", target_temp);
    lv_label_set_text(target_labels[index], buf);

    // Update Switch
    if (is_active && !lv_obj_has_state(status_switches[index], LV_STATE_CHECKED)) {
        lv_obj_add_state(status_switches[index], LV_STATE_CHECKED);
    } else if (!is_active && lv_obj_has_state(status_switches[index], LV_STATE_CHECKED)) {
        lv_obj_clear_state(status_switches[index], LV_STATE_CHECKED);
    }

    // Update PID
    lv_bar_set_value(pid_bars[index], (int32_t)pid_value, LV_ANIM_ON);
    snprintf(buf, sizeof(buf), "%.1f%%", pid_value);
    lv_label_set_text(pid_labels[index], buf);

    // Note: To mimic heating/cooling icons changing color, we'd need image objects or specific style changes to block containers.
}
