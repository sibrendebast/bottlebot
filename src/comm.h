#pragma once
#include <Arduino.h>

void comm_init();
void comm_update();

// Commands to send to the Worker
void comm_send_start();
void comm_send_stop();
void comm_send_estop();
void comm_send_set_co2(int ms);
void comm_send_set_vol(int ml);

// Manual overrides
void comm_send_man_valve(int id, bool state);
void comm_send_man_pneu(int id, bool state);
void comm_send_man_conv(bool state);
