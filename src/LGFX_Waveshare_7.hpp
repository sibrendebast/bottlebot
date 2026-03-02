#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#if defined(ARDUINO)
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>

#define TFT_HOR_RES 1024
#define TFT_VER_RES 600

#define TOUCH_SDA 8
#define TOUCH_SCL 9
#define TOUCH_INT 4
#define TOUCH_RST -1 // Managed by IO Expander

class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Bus_RGB _bus_instance;
  lgfx::Panel_RGB _panel_instance;
  lgfx::Touch_GT911 _touch_instance;

  LGFX(void) {
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = TFT_HOR_RES;
      cfg.memory_height = TFT_VER_RES;
      cfg.panel_width = TFT_HOR_RES;
      cfg.panel_height = TFT_VER_RES;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }

    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      cfg.pin_d0 = 14; // B0 -> B3
      cfg.pin_d1 = 38; // B1 -> B4
      cfg.pin_d2 = 18; // B2 -> B5
      cfg.pin_d3 = 17; // B3 -> B6
      cfg.pin_d4 = 10; // B4 -> B7

      cfg.pin_d5 = 39;  // G0 -> G2
      cfg.pin_d6 = 0;   // G1 -> G3
      cfg.pin_d7 = 45;  // G2 -> G4
      cfg.pin_d8 = 48;  // G3 -> G5
      cfg.pin_d9 = 47;  // G4 -> G6
      cfg.pin_d10 = 21; // G5 -> G7

      cfg.pin_d11 = 1;  // R0 -> R3
      cfg.pin_d12 = 2;  // R1 -> R4
      cfg.pin_d13 = 42; // R2 -> R5
      cfg.pin_d14 = 41; // R3 -> R6
      cfg.pin_d15 = 40; // R4 -> R7

      cfg.pin_henable = 5; // DE
      cfg.pin_vsync = 3;   // VSYNC
      cfg.pin_hsync = 46;  // HSYNC
      cfg.pin_pclk = 7;    // PCLK

      // Typical RGB panel frequency (can be increased up to 16Mhz)
      cfg.freq_write = 16000000;

      // Synchronization parameters - often require tuning based on specific LCD
      // panel controller if screen shifts
      cfg.hsync_polarity = 0;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 4;
      cfg.hsync_back_porch = 8;

      cfg.vsync_polarity = 0;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 4;
      cfg.vsync_back_porch = 8;

      cfg.pclk_active_neg = 1;
      cfg.de_idle_high = 0;
      cfg.pclk_idle_high = 0;

      _bus_instance.config(cfg);
    }
    _panel_instance.setBus(&_bus_instance);

    {
      auto cfg = _touch_instance.config();
      cfg.x_min = 0;
      cfg.x_max = TFT_HOR_RES - 1;
      cfg.y_min = 0;
      cfg.y_max = TFT_VER_RES - 1;
      cfg.pin_int = TOUCH_INT;
      cfg.pin_rst = TOUCH_RST; // Reset is handled manually through CH422G IO
                               // Expander before LGFX init
      cfg.bus_shared = false;
      cfg.offset_rotation = 0;
      cfg.i2c_port = 1; // Used to be I2C_NUM_1
      cfg.pin_sda = TOUCH_SDA;
      cfg.pin_scl = TOUCH_SCL;
      cfg.freq = 400000;
      cfg.i2c_addr = 0x5D;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};
#else
// Native (PC) SDL configuration
class LGFX : public lgfx::LGFX_Device {
public:
  lgfx::Panel_sdl _panel_instance;
  LGFX(void) {
    {
      auto cfg = _panel_instance.config();
      cfg.memory_width = 1024;
      cfg.memory_height = 600;
      cfg.panel_width = 1024;
      cfg.panel_height = 600;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      _panel_instance.config(cfg);
    }
    _panel_instance.setScaling(2, 2);
    setPanel(&_panel_instance);
  }
};
#endif
