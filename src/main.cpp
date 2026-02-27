#include <Arduino.h>
#include "lv_conf.h"
#include <lvgl.h>
#include "LGFX_Waveshare_7.hpp"
#include <ESP_IOExpander_Library.h>
#include <Wire.h>

// Create LGFX instance
LGFX lcd;

// Create IO expander instance
ESP_IOExpander *expander;

// Extender Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5
#define LCD_VDD_EN 6

// I2C Pin define
#define I2C_MASTER_NUM 0
#define I2C_MASTER_SDA_IO 8
#define I2C_MASTER_SCL_IO 9

/* Change to your screen resolution */
static const uint16_t screenWidth  = 1024;
static const uint16_t screenHeight = 600;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10]; // 1/10 screen size buffer is standard

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    lcd.startWrite();
    lcd.setAddrWindow(area->x1, area->y1, w, h);
    lcd.pushPixels((uint16_t *)&color_p->full, w * h, true);
    lcd.endWrite();

    lv_disp_flush_ready(disp_drv);
}

/* Read the touchpad */
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    uint16_t touchX, touchY;

    bool touched = lcd.getTouch(&touchX, &touchY);

    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Bottle Machine HMI...");

    // 1. Initialize I2C for Expander & Touch
    Wire.begin(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    
    // 2. Initialize IO Expander (CH422G for Waveshare 7)
    expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000);
    expander->init();
    expander->begin();
    
    // Configure expander pins as output
    expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL | LCD_VDD_EN, OUTPUT);
    
    // Power on LCD VDD & Backlight, pull Touch Reset High
    expander->digitalWrite(LCD_VDD_EN, HIGH);
    delay(100);
    expander->digitalWrite(LCD_BL, HIGH);
    expander->digitalWrite(LCD_RST, HIGH);
    
    // Reset touch controller
    expander->digitalWrite(TP_RST, LOW);
    delay(20);
    expander->digitalWrite(TP_RST, HIGH);
    delay(100);

    // 3. Initialize Display
    lcd.begin();
    lcd.fillScreen(TFT_BLACK);

    // 4. Initialize LVGL
    lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    // 5. Create basic UI Test
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Bottle Machine HMI Starting...");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);
    
    lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 50);
    
    lv_obj_t *btn_label = lv_label_create(btn1);
    lv_label_set_text(btn_label, "Start Machine");
    lv_obj_center(btn_label);

    Serial.println("Setup done");
}

void loop() {
    lv_timer_handler(); // let the GUI do its work
    delay(5);
}
