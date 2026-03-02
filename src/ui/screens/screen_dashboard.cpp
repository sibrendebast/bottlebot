#include "screen_dashboard.h"
#include "../ui_theme.h"
#include <stdio.h>
#include "../../comm.h"

#define NUM_HEADS 4

lv_obj_t *lbl_status;
lv_obj_t *lbl_lifetime;
lv_obj_t *lbl_volume[4];
lv_obj_t *bar_progress[4];
lv_obj_t *led_bottle[4];

lv_obj_t *ta_fill_target;
lv_obj_t *ta_co2_time;

lv_obj_t *sw_beer[4];
lv_obj_t *sw_co2;
lv_obj_t *sw_head_lift;
lv_obj_t *sw_gate_entry;
lv_obj_t *sw_gate_exit;
lv_obj_t *sw_conveyor;

static lv_obj_t *panel_run;
static lv_obj_t *panel_recipe;
static lv_obj_t *panel_manual;
static lv_obj_t *kb;

static void nav_btn_event_cb(lv_event_t * e) {
    lv_obj_t * target = (lv_obj_t *)lv_event_get_user_data(e);
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_add_flag(panel_run, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(panel_recipe, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(panel_manual, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(target, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ta_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    if(code == LV_EVENT_FOCUSED) {
        if(kb != NULL) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(kb);
        }
    }
    if(code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        if(kb != NULL) lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

static void start_btn_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        comm_send_start();
    }
}

static void stop_btn_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        comm_send_stop();
    }
}

static void estop_btn_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        comm_send_estop();
    }
}

static void config_save_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        if(ta_co2_time) {
            comm_send_set_co2(atoi(lv_textarea_get_text(ta_co2_time)));
        }
    }
}

static void manual_switch_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    bool state = lv_obj_has_state(obj, LV_STATE_CHECKED);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        if (obj == sw_co2) comm_send_man_valve(0, state);
        else if (obj == sw_head_lift) comm_send_man_pneu(1, state);
        else if (obj == sw_gate_entry) comm_send_man_pneu(2, state);
        else if (obj == sw_gate_exit) comm_send_man_pneu(3, state);
        else if (obj == sw_conveyor) comm_send_man_conv(state);
        else {
            for(int i=0; i<NUM_HEADS; i++) {
                if(obj == sw_beer[i]) {
                    comm_send_man_valve(i+1, state);
                    break;
                }
            }
        }
    }
}

static void build_run_screen(lv_obj_t * parent) {
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(parent, 20, 0);

    // Top Header
    lv_obj_t *header = lv_obj_create(parent);
    lv_obj_remove_style_all(header);
    lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_layout(header, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lbl_status = lv_label_create(header);
    lv_label_set_text(lbl_status, "STATUS: IDLE");
    lv_obj_add_style(lbl_status, &style_title, 0);

    lbl_lifetime = lv_label_create(header);
    lv_label_set_text(lbl_lifetime, "Total: 0 bottles");
    lv_obj_add_style(lbl_lifetime, &style_title, 0);

    // Center Stage (4 Heads)
    lv_obj_t *center = lv_obj_create(parent);
    lv_obj_remove_style_all(center);
    lv_obj_set_size(center, lv_pct(100), 380); // Rest of space
    lv_obj_set_layout(center, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(center, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(center, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    for (int i=0; i<NUM_HEADS; i++) {
        lv_obj_t *card = create_card(center);
        lv_obj_set_size(card, lv_pct(23), lv_pct(100)); // slightly less than 25% for gaps
        lv_obj_set_layout(card, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(card, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Bottle Icon
        led_bottle[i] = lv_led_create(card);
        lv_obj_set_size(led_bottle[i], 32, 32);
        lv_led_set_color(led_bottle[i], COLOR_PRIMARY);
        lv_led_off(led_bottle[i]);

        // Vertical Bar
        bar_progress[i] = lv_bar_create(card);
        lv_obj_remove_style_all(bar_progress[i]);
        lv_obj_add_style(bar_progress[i], &style_bar_bg_vert, LV_PART_MAIN);
        lv_obj_add_style(bar_progress[i], &style_bar_indic_vert, LV_PART_INDICATOR);
        lv_obj_set_size(bar_progress[i], 40, lv_pct(60)); 
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
    lv_obj_set_flex_align(footer, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *btn_start = create_button(footer, "START CYCLE", 1); // Primary
    lv_obj_set_size(btn_start, lv_pct(30), 80);
    lv_obj_add_event_cb(btn_start, start_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_pause = create_button(footer, "PAUSE", 2); // Warning
    lv_obj_set_size(btn_pause, lv_pct(30), 80);
    lv_obj_add_event_cb(btn_pause, stop_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_estop = create_button(footer, "GLOBAL E-STOP", 3); // Danger
    lv_obj_set_size(btn_estop, lv_pct(30), 80);
    lv_obj_add_event_cb(btn_estop, estop_btn_event_cb, LV_EVENT_CLICKED, NULL);
}

static void build_recipe_screen(lv_obj_t * parent) {
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(parent, 40, 0);

    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Recipe Settings");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_pad_bottom(title, 40, 0);

    lv_obj_t *form = create_card(parent);
    lv_obj_set_size(form, 500, LV_SIZE_CONTENT);
    lv_obj_set_layout(form, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(form, LV_FLEX_FLOW_COLUMN);

    // Target fill volume
    lv_obj_t *lbl_target = lv_label_create(form);
    lv_label_set_text(lbl_target, "Fill Target (mL)");
    lv_obj_add_style(lbl_target, &style_label, 0);
    
    ta_fill_target = lv_textarea_create(form);
    lv_textarea_set_one_line(ta_fill_target, true);
    lv_textarea_set_text(ta_fill_target, "330");
    lv_obj_set_width(ta_fill_target, lv_pct(100));
    lv_obj_add_event_cb(ta_fill_target, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_style_pad_bottom(ta_fill_target, 20, 0);

    // CO2 Flush
    lv_obj_t *lbl_co2 = lv_label_create(form);
    lv_label_set_text(lbl_co2, "CO2 Flush Time (ms)");
    lv_obj_add_style(lbl_co2, &style_label, 0);
    
    ta_co2_time = lv_textarea_create(form);
    lv_textarea_set_one_line(ta_co2_time, true);
    lv_textarea_set_text(ta_co2_time, "2500");
    lv_obj_set_width(ta_co2_time, lv_pct(100));
    lv_obj_add_event_cb(ta_co2_time, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_style_pad_bottom(ta_co2_time, 40, 0);

    lv_obj_t *btn_save = create_button(form, "Save & Apply", 1);
    lv_obj_set_width(btn_save, lv_pct(100));
    lv_obj_add_event_cb(btn_save, config_save_event_cb, LV_EVENT_CLICKED, NULL);
}

static void build_manual_screen(lv_obj_t * parent) {
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(parent, 20, 0);

    lv_obj_t *title = lv_label_create(parent);
    lv_label_set_text(title, "Maintenance / Overrides");
    lv_obj_add_style(title, &style_title, 0);
    lv_obj_set_style_pad_bottom(title, 20, 0);

    lv_obj_t *grid = lv_obj_create(parent);
    lv_obj_remove_style_all(grid);
    lv_obj_set_size(grid, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_layout(grid, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // Valves
    lv_obj_t *c_valves = create_card(grid);
    lv_obj_set_size(c_valves, 300, LV_SIZE_CONTENT);
    lv_obj_set_layout(c_valves, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(c_valves, LV_FLEX_FLOW_COLUMN);
    lv_obj_t *l_valves = lv_label_create(c_valves);
    lv_label_set_text(l_valves, "Solenoids");
    lv_obj_add_style(l_valves, &style_title, 0);
    
    for(int i=0; i<NUM_HEADS; i++) {
        lv_obj_t *row = lv_obj_create(c_valves);
        lv_obj_remove_style_all(row);
        lv_obj_set_width(row, lv_pct(100));
        lv_obj_set_layout(row, LV_LAYOUT_FLEX);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t *lbl = lv_label_create(row);
        lv_label_set_text_fmt(lbl, "Beer Valve %d", i+1);
        sw_beer[i] = create_switch(row);
    }
    lv_obj_t *row_co2 = lv_obj_create(c_valves);
    lv_obj_remove_style_all(row_co2);
    lv_obj_set_width(row_co2, lv_pct(100));
    lv_obj_set_layout(row_co2, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(row_co2, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t *lbl_co2 = lv_label_create(row_co2);
    lv_label_set_text(lbl_co2, "Master CO2");
    sw_co2 = create_switch(row_co2);

    // Mechanicals
    lv_obj_t *c_mech = create_card(grid);
    lv_obj_set_size(c_mech, 300, LV_SIZE_CONTENT);
    lv_obj_set_layout(c_mech, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(c_mech, LV_FLEX_FLOW_COLUMN);
    lv_obj_t *l_mech = lv_label_create(c_mech);
    lv_label_set_text(l_mech, "Mechanicals");
    lv_obj_add_style(l_mech, &style_title, 0);

    lv_obj_t *row_lift = lv_obj_create(c_mech);
    lv_obj_remove_style_all(row_lift);
    lv_obj_set_width(row_lift, lv_pct(100));
    lv_obj_set_layout(row_lift, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(row_lift, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t *lbl_lift = lv_label_create(row_lift);
    lv_label_set_text(lbl_lift, "Head Lift");
    sw_head_lift = create_switch(row_lift);

    lv_obj_t *row_gate_e = lv_obj_create(c_mech);
    lv_obj_remove_style_all(row_gate_e);
    lv_obj_set_width(row_gate_e, lv_pct(100));
    lv_obj_set_layout(row_gate_e, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(row_gate_e, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t *lbl_gate_e = lv_label_create(row_gate_e);
    lv_label_set_text(lbl_gate_e, "Entry Gate");
    sw_gate_entry = create_switch(row_gate_e);

    lv_obj_t *row_gate_x = lv_obj_create(c_mech);
    lv_obj_remove_style_all(row_gate_x);
    lv_obj_set_width(row_gate_x, lv_pct(100));
    lv_obj_set_layout(row_gate_x, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(row_gate_x, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t *lbl_gate_x = lv_label_create(row_gate_x);
    lv_label_set_text(lbl_gate_x, "Exit Gate");
    sw_gate_exit = create_switch(row_gate_x);

    lv_obj_t *row_conv = lv_obj_create(c_mech);
    lv_obj_remove_style_all(row_conv);
    lv_obj_set_width(row_conv, lv_pct(100));
    lv_obj_set_layout(row_conv, LV_LAYOUT_FLEX);
    lv_obj_set_flex_align(row_conv, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t *lbl_conv = lv_label_create(row_conv);
    lv_label_set_text(lbl_conv, "Conveyor");
    sw_conveyor = create_switch(row_conv);

    // Manual Events
    lv_obj_add_event_cb(sw_co2, manual_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(sw_head_lift, manual_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(sw_gate_entry, manual_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(sw_gate_exit, manual_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(sw_conveyor, manual_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    for(int i=0; i<NUM_HEADS; i++) {
        lv_obj_add_event_cb(sw_beer[i], manual_switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    }
}

void ui_screen_dashboard_init(void) {
    lv_obj_t *scr = lv_scr_act();
    lv_obj_add_style(scr, &style_screen_bg, 0);
    lv_obj_set_layout(scr, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW);

    // Global Keyboard
    kb = lv_keyboard_create(scr);
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUMBER);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    
    // Left Navigation Bar
    lv_obj_t *nav_bar = lv_obj_create(scr);
    lv_obj_remove_style_all(nav_bar);
    lv_obj_set_size(nav_bar, 120, lv_pct(100));
    lv_obj_add_style(nav_bar, &style_card, 0); // Reusing card style for white bg+border
    lv_obj_set_style_border_width(nav_bar, 2, 0);
    lv_obj_set_style_border_side(nav_bar, LV_BORDER_SIDE_RIGHT, 0);
    
    lv_obj_set_layout(nav_bar, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(nav_bar, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(nav_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(nav_bar, 10, 0);

    lv_obj_t *lbl_logo = lv_label_create(nav_bar);
    lv_label_set_text(lbl_logo, "B-BOT");
    lv_obj_add_style(lbl_logo, &style_title, 0);
    lv_obj_set_style_pad_bottom(lbl_logo, 40, 0);

    lv_obj_t *btn_nav_run = create_button(nav_bar, "RUN", 0);
    lv_obj_set_size(btn_nav_run, lv_pct(100), 60);

    lv_obj_t *btn_nav_recipe = create_button(nav_bar, "RECIPE", 0);
    lv_obj_set_size(btn_nav_recipe, lv_pct(100), 60);

    lv_obj_t *btn_nav_manual = create_button(nav_bar, "MANUAL", 0);
    lv_obj_set_size(btn_nav_manual, lv_pct(100), 60);

    // Main Content Area
    lv_obj_t *content = lv_obj_create(scr);
    lv_obj_remove_style_all(content);
    lv_obj_set_size(content, lv_pct(100), lv_pct(100)); // Flex will constrain it
    lv_obj_set_flex_grow(content, 1);

    // Create 3 overlapping panels inside content Area
    panel_run = lv_obj_create(content);
    lv_obj_remove_style_all(panel_run);
    lv_obj_set_size(panel_run, lv_pct(100), lv_pct(100));
    build_run_screen(panel_run);

    panel_recipe = lv_obj_create(content);
    lv_obj_remove_style_all(panel_recipe);
    lv_obj_set_size(panel_recipe, lv_pct(100), lv_pct(100));
    build_recipe_screen(panel_recipe);
    lv_obj_add_flag(panel_recipe, LV_OBJ_FLAG_HIDDEN);

    panel_manual = lv_obj_create(content);
    lv_obj_remove_style_all(panel_manual);
    lv_obj_set_size(panel_manual, lv_pct(100), lv_pct(100));
    build_manual_screen(panel_manual);
    lv_obj_add_flag(panel_manual, LV_OBJ_FLAG_HIDDEN);

    // Assign events
    lv_obj_add_event_cb(btn_nav_run, nav_btn_event_cb, LV_EVENT_ALL, panel_run);
    lv_obj_add_event_cb(btn_nav_recipe, nav_btn_event_cb, LV_EVENT_ALL, panel_recipe);
    lv_obj_add_event_cb(btn_nav_manual, nav_btn_event_cb, LV_EVENT_ALL, panel_manual);
}

void ui_update_status(const char* status) {
    if(lbl_status) {
        char buf[64];
        snprintf(buf, sizeof(buf), "STATUS: %s", status);
        lv_label_set_text(lbl_status, buf);
    }
}

void ui_update_lifetime(uint32_t count) {
    if(lbl_lifetime) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Total: %u bottles", count);
        lv_label_set_text(lbl_lifetime, buf);
    }
}

void ui_update_head_data(uint8_t index, bool bottle_present, int volume_ml, int target_ml) {
    if (index >= NUM_HEADS) return;

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
