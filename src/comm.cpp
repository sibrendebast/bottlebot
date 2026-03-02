#include "comm.h"
#include "ui/screens/screen_dashboard.h"

// Communication Pins (Choosing 19 & 20 as they are free from the LGFX config)
const int PIN_HMI_RX = 19;
const int PIN_HMI_TX = 20;

// Serial Parsing Variables
const byte numChars = 64;
char receivedChars[numChars];
boolean newData = false;

// Heartbeat variables
unsigned long lastPingSent = 0;
const unsigned long pingInterval = 2000; // Send PING every 2 seconds

void comm_init() {
    Serial1.begin(330, SERIAL_8N1, PIN_HMI_RX, PIN_HMI_TX); // Low baud for test? No, protocol says 115200
    Serial1.begin(115200, SERIAL_8N1, PIN_HMI_RX, PIN_HMI_TX);
}

void parseSerialData() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            } else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        } else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void processIncomingData() {
    if (newData) {
        String msg = String(receivedChars);
        
        // Handle Telemetry: <STATE,X>
        if (msg.startsWith("STATE,")) {
            String stateStr = msg.substring(6);
            ui_update_status(stateStr.c_str());
        }
        // Handle Head Telemetry: <FILL,ID,VOL> (Though currently level probes are binary)
        else if (msg.startsWith("FILL,")) {
            // Future implementation for flow meters
        }
        // Handle Head Finished: <DONE,X>
        else if (msg.startsWith("DONE,")) {
            int headIdx = msg.substring(5).toInt() - 1;
            ui_update_head_data(headIdx, true, 330, 330); // Hardcode done for now
        }
        // Handle Errors: <ERR,X>
        else if (msg.startsWith("ERR,")) {
            String errStr = msg.substring(4);
            ui_update_status(errStr.c_str()); // Could use a popup
        }
        
        newData = false;
    }
}

void comm_update() {
    parseSerialData();
    processIncomingData();

    // Heartbeat logic
    if (millis() - lastPingSent >= pingInterval) {
        Serial1.println("<PING>");
        lastPingSent = millis();
    }
}

void comm_send_start() {
    Serial1.println("<START>");
}

void comm_send_stop() {
    Serial1.println("<STOP>");
}

void comm_send_estop() {
    Serial1.println("<ESTOP>");
}

void comm_send_set_co2(int ms) {
    Serial1.print("<SET_CO2,");
    Serial1.print(ms);
    Serial1.println(">");
}

void comm_send_set_vol(int ml) {
    Serial1.print("<SET_VOL,");
    Serial1.print(ml);
    Serial1.println(">");
}

void comm_send_man_valve(int id, bool state) {
    Serial1.print("<MAN_VLV,");
    Serial1.print(id);
    Serial1.print(",");
    Serial1.print(state ? 1 : 0);
    Serial1.println(">");
}

void comm_send_man_pneu(int id, bool state) {
    Serial1.print("<MAN_PNU,");
    Serial1.print(id);
    Serial1.print(",");
    Serial1.print(state ? 1 : 0);
    Serial1.println(">");
}

void comm_send_man_conv(bool state) {
    Serial1.print("<MAN_CNV,");
    Serial1.print(state ? 1 : 0);
    Serial1.println(">");
}
