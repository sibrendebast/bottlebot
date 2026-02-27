# 🖥️ Bottling Machine HMI - UI Specification v1.0

**Hardware:** Waveshare 7.0-inch ESP32-S3 Touchscreen (1024x600)
**UI Framework:** LVGL (via SquareLine Studio / EEZ Studio)

---

## 1. The Screen Hierarchy

The interface is divided into three core screens, accessible via a persistent navigation bar (bottom or left-side).

### Screen 1: The Main Dashboard ("Run Screen")
*This is the primary operational view.*
* **Top Header:**
    * **State Indicator:** Dynamic text displaying the current machine state (e.g., `STATUS: IDLE`, `STATUS: CO2 FLUSHING`).
    * **Lifetime Counter:** Total successful bottles filled.
* **Center Stage (The 4 Fill Heads):**
    * Divided into 4 equal vertical panels.
    * **Bottle Presence Icon:** Illuminates green when the optical sensor detects a bottle in that specific slot.
    * **Live Progress Bar:** Fills vertically as the flow meter ticks. 
    * **Volume Text:** Live readout of the current volume (e.g., `215 / 330 mL`).
* **Bottom Footer (Controls):**
    * 🟢 **START CYCLE:** Massive green button (sends `<START>`).
    * 🟠 **PAUSE:** Large orange button (sends `<STOP>`).
    * 🔴 **GLOBAL E-STOP:** Always-visible red button (sends `<ESTOP>`).

### Screen 2: The Settings Screen ("Recipe Screen")
*Used for adjusting machine parameters. Tapping an input field opens an on-screen numpad.*
* **Fill Target:** Input field for target volume in mL (sends `<SET_VOL,X>`).
* **CO2 Flush Time:** Input field for flush duration in milliseconds (sends `<SET_CO2,X>`).
* **Save/Apply:** Button to commit changes and transmit over UART.

### Screen 3: Manual Override ("Maintenance Screen")
*Strictly for cleaning, line purging, and mechanical testing while the machine is IDLE.*
* **Valve Controls:** * 4x Beer Solenoid Toggles (sends `<MAN_VLV,1,1>` through `<MAN_VLV,4,1>`).
    * 1x CO2 Master Solenoid Toggle (sends `<MAN_VLV,0,1>`).
* **Pneumatic Controls:** * Head Lift/Lower Toggle (sends `<MAN_PNU,1,1>`).
    * Gate Up/Down Toggle (sends `<MAN_PNU,2,1>`).
* **Motor Controls:** * Conveyor On/Off Toggle (sends `<MAN_CNV,1>`).

---

## 2. Navigation & Error Handling Flow

The following diagram illustrates how the operator moves through the UI, including asynchronous hardware error popups triggered by the Worker board.

```mermaid
graph TD
    %% Define styles
    classDef screen fill:#2c3e50,color:white,stroke:#34495e,stroke-width:2px;
    classDef popup fill:#c0392b,color:white,stroke:#e74c3c,stroke-width:3px,font-weight:bold;
    classDef nav fill:#2980b9,color:white,stroke-width:0px;

    %% Navigation Bar
    NAV_BAR[Persistent Bottom/Side Nav Bar]:::nav

    %% Screens
    MAIN_DASH(1. Main Dashboard \n 'The Run Screen'):::screen
    SETTINGS(2. Settings \n 'The Recipe Screen'):::screen
    MANUAL(3. Manual Control \n 'The Cleaning Screen'):::screen

    %% Modals/Popups
    ERR_MODAL{{ERROR POPUP \n 'Clear Jam / Check Gas'}}:::popup
    KEYPAD{{On-Screen Numpad}}:::screen

    %% Links
    NAV_BAR --- MAIN_DASH
    NAV_BAR --- SETTINGS
    NAV_BAR --- MANUAL

    SETTINGS -. taps input field .-> KEYPAD
    KEYPAD -. saves .-> SETTINGS

    %% Interrupts
    MAIN_DASH -. Worker sends <ERR> .-> ERR_MODAL
    SETTINGS -. Worker sends <ERR> .-> ERR_MODAL
    MANUAL -. Worker sends <ERR> .-> ERR_MODAL

    ERR_MODAL -. Operator taps 'Acknowledge' .-> MAIN_DASH
'''