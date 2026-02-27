#if defined(ARDUINO)
#include <Arduino.h>
#include <Wire.h>
#else
#include <stdio.h>
#include <unistd.h>
// #define delay(ms) usleep((ms)*1000) // Conflicting with LovyanGFX's delay()
#endif
#include "lv_conf.h"
#include <lvgl.h>
#include "LGFX_Waveshare_7.hpp"
#include "ui/screens/screen_dashboard.h"
#include "ui/ui_theme.h"

// Create LGFX instance
LGFX lcd;

// CH422G I2C Address (default is 0x24 or 0x20 depending on wiring)
// The library used ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000 which is 0x24.
#define CH422G_I2C_ADDR 0x24

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

void setup() {
#if defined(ARDUINO)
    Serial.begin(115200);
    Serial.println("Starting Bottle Machine HMI...");
#else
    printf("Starting Bottle Machine HMI (Native)...\n");
#endif

#if defined(ARDUINO)
    // 1. Initialize I2C for Expander & Touch
    Wire.begin(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    
    // 2. Initialize IO Expander (CH422G for Waveshare 7) manually
    // Bit mapping based on schematic:
    // Bit 1: TP_RST
    // Bit 2: LCD_BL (DISP)
    // Bit 3: LCD_RST
    // Bit 4: SD_CS
    // Bit 5: USB_SEL
    // Bit 6: LCD_VDD_EN
    
    // We want to turn on LCD_VDD_EN (6), LCD_BL (2), LCD_RST (3), and initially TP_RST (1)
    // 0x4E = 0100 1110 -> Bits 6(VDD), 3(LCD_RST), 2(LCD_BL), 1(TP_RST) are HIGH
    
    // First, configure outputs (CH422G requires setting IO direction, usually register 0x24)
    Wire.beginTransmission(CH422G_I2C_ADDR);
    Wire.write(0x4E); // All vital pins HIGH
    Wire.endTransmission();
    
    delay(100);
    
    // Touch reset sequence (Pull TP_RST LOW briefly)
    // 0x4C = 0100 1100 -> Bit 1 is LOW
    Wire.beginTransmission(CH422G_I2C_ADDR);
    Wire.write(0x4C); 
    Wire.endTransmission();
    delay(20);
    // Bring TP_RST HIGH again
    Wire.beginTransmission(CH422G_I2C_ADDR);
    Wire.write(0x4E); 
    Wire.endTransmission();
    delay(100);
#endif

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

    // 5. Initialize Custom UI
    ui_theme_init();
    ui_screen_dashboard_init();
    
    // Set some dummy data
    ui_update_fermenter_data(0, 18.5f, 18.0f, true, true, false, -30.0f);
    ui_update_fermenter_data(1, 21.0f, 21.0f, true, false, false, 0.0f);
    ui_update_fermenter_data(2, 4.0f, 4.0f, true, true, false, -80.0f);

#if defined(ARDUINO)
    Serial.println("Setup done");
#else
    printf("Setup done\n");
#endif
}

void loop() {
    lv_timer_handler(); // let the GUI do its work
    lv_tick_inc(5);     // tell LVGL 5ms have passed
#if defined(ARDUINO)
    delay(5);
#else
    usleep(5000);
#endif
}

#if !defined(ARDUINO)
int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}
#endif
