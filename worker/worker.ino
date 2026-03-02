#include <Arduino.h>

// --- PIN DEFINITIONS (Matching Diagram & Hardware Spec) ---
const int PIN_PROBE[4]  = {4, 5, 6, 7};
const int PIN_BOTTLE[4] = {8, 9, 10, 11};
const int PIN_SW_DOWN   = 12;
const int PIN_SW_UP     = 13;

const int PIN_BEER[4]   = {14, 15, 18, 21}; // Now triggering Pneumatic Angle Seat Valves
const int PIN_CO2       = 35;
const int PIN_HEADS      = 36;
const int PIN_GATE_EXIT  = 37; // Formerly PIN_GATE
const int PIN_GATE_ENTRY = 40; // New Entry Gate
const int PIN_CONVEYOR   = 38; 
const int PIN_ROD_POWER  = 39; // 24V Power to the feeling rods (via Relay)

// UART Pins for Serial1 (Communication with HMI)
const int PIN_RX1       = 16;
const int PIN_TX1       = 17;

// --- STATE MACHINE ENUM ---
enum MachineState {
  STATE_IDLE,
  STATE_INDEXING,
  STATE_LOWER_HEADS,
  STATE_CO2_FLUSH,
  STATE_BEER_FILL,
  STATE_RAISE_HEADS,
  STATE_ERROR
};

MachineState currentState = STATE_IDLE;

// --- TIMING & TRACKING VARIABLES ---
unsigned long stateStartTime = 0;
unsigned long co2FlushDuration = 1500; // ms
unsigned long timeoutLimit = 5000;     // 5 seconds max for mechanical moves
bool activeHeads[4] = {false, false, false, false}; // Tracks which heads have bottles

// Serial Parsing Variables
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

// Command Flags
bool startCommandReceived = false;
bool stopCommandReceived = false;
bool estopCommandReceived = false;

// Heartbeat variables
unsigned long lastPingTime = 0;
const unsigned long pingTimeout = 5000; // 5 seconds safety cutoff

// --- HELPER FUNCTIONS ---
void closeAllValves() {
  for (int i=0; i<4; i++) digitalWrite(PIN_BEER[i], LOW);
  digitalWrite(PIN_CO2, LOW);
  digitalWrite(PIN_HEADS, LOW);
  digitalWrite(PIN_GATE_EXIT, HIGH);  // HIGH/Actuated = Gate Pin UP (Blocked)
  digitalWrite(PIN_GATE_ENTRY, HIGH); // HIGH/Actuated = Gate Pin UP (Blocked)
  digitalWrite(PIN_CONVEYOR, LOW);
  digitalWrite(PIN_ROD_POWER, LOW);
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

void handleCommand() {
  if (newData) {
    String cmd = String(receivedChars);
    
    // Heartbeat logic
    if (cmd == "PING") {
      Serial1.println("<PONG>");
      lastPingTime = millis();
    }
    else if (cmd == "START") {
      startCommandReceived = true;
    }
    else if (cmd == "STOP") {
      stopCommandReceived = true;
    }
    else if (cmd == "ESTOP") {
      estopCommandReceived = true;
    }
    else if (cmd.startsWith("SET_CO2,")) {
      co2FlushDuration = cmd.substring(8).toInt();
    }
    else if (cmd.startsWith("MAN_VLV,")) {
      int comma1 = cmd.indexOf(',');
      int comma2 = cmd.indexOf(',', comma1 + 1);
      int id = cmd.substring(comma1 + 1, comma2).toInt();
      int state = cmd.substring(comma2 + 1).toInt();
      if (id == 0) digitalWrite(PIN_CO2, state);
      else if (id >= 1 && id <= 4) digitalWrite(PIN_BEER[id-1], state);
    }
    else if (cmd.startsWith("MAN_PNU,")) {
      int comma1 = cmd.indexOf(',');
      int comma2 = cmd.indexOf(',', comma1 + 1);
      int id = cmd.substring(comma1 + 1, comma2).toInt();
      int state = cmd.substring(comma2 + 1).toInt();
      if (id == 1) digitalWrite(PIN_HEADS, state);
      else if (id == 2) digitalWrite(PIN_GATE_ENTRY, state);
      else if (id == 3) digitalWrite(PIN_GATE_EXIT, state);
    }
    else if (cmd.startsWith("MAN_CNV,")) {
      int state = cmd.substring(8).toInt();
      digitalWrite(PIN_CONVEYOR, state);
    }
    
    newData = false;
  }
}

// ==========================================
// MAIN SETUP
// ==========================================
void setup() {
  Serial.begin(115200); // Debug Serial
  Serial1.begin(115200, SERIAL_8N1, PIN_RX1, PIN_TX1); // Brain/HMI Serial

  // Set Input Pins
  for (int i=0; i<4; i++) {
    pinMode(PIN_PROBE[i], INPUT_PULLUP);
    pinMode(PIN_BOTTLE[i], INPUT_PULLUP);
  }
  pinMode(PIN_SW_DOWN, INPUT_PULLUP);
  pinMode(PIN_SW_UP, INPUT_PULLUP);

  // Set Output Pins
  for (int i=0; i<4; i++) pinMode(PIN_BEER[i], OUTPUT);
  pinMode(PIN_CO2, OUTPUT);
  pinMode(PIN_HEADS, OUTPUT);
  pinMode(PIN_GATE_EXIT, OUTPUT);
  pinMode(PIN_GATE_ENTRY, OUTPUT);
  pinMode(PIN_CONVEYOR, OUTPUT);
  pinMode(PIN_ROD_POWER, OUTPUT);

  closeAllValves();
  lastPingTime = millis();
  Serial.println("Worker Booted: IDLE");
}

// ==========================================
// MAIN LOOP (The State Machine)
// ==========================================
void loop() {
  parseSerialData();
  handleCommand();

  // Emergency Stop Logic
  if (estopCommandReceived || (millis() - lastPingTime > pingTimeout)) {
    currentState = STATE_ERROR;
    closeAllValves();
    if (estopCommandReceived) Serial1.println("<ERR,ESTOP>");
    else Serial1.println("<ERR,TIMEOUT>");
    estopCommandReceived = false;
  }

  // Handle Safely Stopping
  if (stopCommandReceived && currentState == STATE_IDLE) {
    // Already idle or safety pause
    stopCommandReceived = false; 
  }

  switch (currentState) {
    
    // -----------------------------------
    case STATE_IDLE:
      if (startCommandReceived) {
        startCommandReceived = false;
        currentState = STATE_INDEXING;
        stateStartTime = millis();
        Serial1.println("<STATE,INDEXING>");
      }
      break;

    // -----------------------------------
    case STATE_INDEXING:
      digitalWrite(PIN_CONVEYOR, HIGH); 
      digitalWrite(PIN_GATE_EXIT, HIGH);  // Exit blocked (Hold bottles)
      digitalWrite(PIN_GATE_ENTRY, LOW);  // Entry open (Let bottles in)

      // Check if all 4 bottles are present (LOW means triggered)
      if (digitalRead(PIN_BOTTLE[0]) == LOW && digitalRead(PIN_BOTTLE[1]) == LOW &&
          digitalRead(PIN_BOTTLE[2]) == LOW && digitalRead(PIN_BOTTLE[3]) == LOW) {
        
        // Mark all heads as active for this fill
        for(int i=0; i<4; i++) activeHeads[i] = true;
        
        digitalWrite(PIN_GATE_ENTRY, HIGH); // Close entry gate to stop 5th bottle
        currentState = STATE_LOWER_HEADS;
        stateStartTime = millis();
        Serial1.println("<STATE,LOWERING>");
      }

      // Safety Timeout (e.g., jam on the conveyor)
      if (millis() - stateStartTime > 10000) {
        currentState = STATE_ERROR;
        Serial1.println("<ERR,MISSING_BOTTLES>");
      }
      break;

    // -----------------------------------
    case STATE_LOWER_HEADS:
      digitalWrite(PIN_HEADS, HIGH); // Fire pneumatics downward

      if (digitalRead(PIN_SW_DOWN) == LOW) { // Limit switch hit
        currentState = STATE_CO2_FLUSH;
        stateStartTime = millis();
        Serial1.println("<STATE,FLUSHING>");
        digitalWrite(PIN_CO2, HIGH); // Open CO2
      }

      if (millis() - stateStartTime > timeoutLimit) {
        currentState = STATE_ERROR;
        Serial1.println("<ERR,HEAD_JAM>");
      }
      break;

    // -----------------------------------
    case STATE_CO2_FLUSH:
      if (millis() - stateStartTime >= co2FlushDuration) {
        digitalWrite(PIN_CO2, LOW); // Close CO2
        
        currentState = STATE_BEER_FILL;
        Serial1.println("<STATE,FILLING>");
        
        digitalWrite(PIN_ROD_POWER, HIGH); // Power the feeling rods
        
        // Open Angle Seat Valves ONLY for heads that have a bottle
        for(int i=0; i<4; i++) {
          if (activeHeads[i]) digitalWrite(PIN_BEER[i], HIGH);
        }
      }
      break;

    // -----------------------------------
    case STATE_BEER_FILL:
      bool allFinished = true;

      for(int i=0; i<4; i++) {
        if (activeHeads[i]) {
          // If the feeling rod senses current (vaccum/liquid closes the circuit)
          // HIGH means the optocoupler sees the 24V from the rod via the beer
          if (digitalRead(PIN_PROBE[i]) == HIGH) { 
            digitalWrite(PIN_BEER[i], LOW); // Close pneumatic angle seat valve
            activeHeads[i] = false; // Mark as done
            Serial1.print("<DONE,"); Serial1.print(i+1); Serial1.println(">");
          } else {
            allFinished = false; // Still waiting on at least one bottle
          }
        }
      }

      if (allFinished) {
        digitalWrite(PIN_ROD_POWER, LOW); // Turn off rod power
        currentState = STATE_RAISE_HEADS;
        stateStartTime = millis();
        Serial1.println("<STATE,RAISING>");
      }
      break;

    // -----------------------------------
    case STATE_RAISE_HEADS:
      digitalWrite(PIN_HEADS, LOW); // Retract pistons
      
      // Wait for heads to be up before releasing bottles
      if (digitalRead(PIN_SW_UP) == LOW) {
        digitalWrite(PIN_GATE_EXIT, LOW); // Release filled bottles
        
        // Check if all 4 bottle sensors are clear (HIGH = No bottle)
        if (digitalRead(PIN_BOTTLE[0]) == HIGH && digitalRead(PIN_BOTTLE[1]) == HIGH &&
            digitalRead(PIN_BOTTLE[2]) == HIGH && digitalRead(PIN_BOTTLE[3]) == HIGH) {
          
          digitalWrite(PIN_GATE_EXIT, HIGH); // Close exit gate for next batch
          
          if (stopCommandReceived) {
            digitalWrite(PIN_CONVEYOR, LOW);
            currentState = STATE_IDLE;
            Serial1.println("<STATE,IDLE>");
            stopCommandReceived = false;
          } else {
            // Immediately start next batch
            currentState = STATE_INDEXING;
            stateStartTime = millis();
            Serial1.println("<STATE,INDEXING>");
          }
        }
      }

      if (millis() - stateStartTime > timeoutLimit) {
        currentState = STATE_ERROR;
        Serial1.println("<ERR,HEAD_JAM>");
      }
      break;

    // -----------------------------------
    case STATE_ERROR:
      closeAllValves();
      // Wait for Start again or a manual Reset which we could map to START in IDLE
      if (startCommandReceived) {
        startCommandReceived = false;
        currentState = STATE_IDLE;
        Serial1.println("<STATE,IDLE>");
      }
      break;
  }
}

