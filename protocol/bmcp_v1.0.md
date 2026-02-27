# 🍺 Bottling Machine Communication Protocol v1.0

## 1. Physical Layer (The Wiring)
* **Connection:** UART (Serial)
* **Wiring:** HMI TX to Worker RX | HMI RX to Worker TX | GND to GND
* **Baud Rate:** 115200 
* **Logic Level:** 3.3V (Native to both ESP32-S3 boards)

## 2. Message Structure
All messages must follow a strict **Start/End Marker** format to prevent corrupted data from causing machine malfunctions.

* **Start Marker:** `<`
* **Delimiter:** `,`
* **End Marker:** `>`
* **Format:** `<COMMAND,PARAM_1,PARAM_2>`

> **Rule:** If a board receives a message missing a `<` or `>`, it must instantly discard the data.

---

## 3. Command Dictionary: HMI to Worker (The Orders)
These commands are sent from the 7-inch Touchscreen (HMI) to the DevKitC (Worker) to control the machine or change settings.

| Command string | Purpose | Example |
| :--- | :--- | :--- |
| `<START>` | Kicks off the automated bottling cycle. | `<START>` |
| `<STOP>` | Pauses the cycle safely after the current state finishes. | `<STOP>` |
| `<ESTOP>` | **EMERGENCY STOP.** Immediately cuts all solenoids, pneumatics, and motors. | `<ESTOP>` |
| `<SET_VOL,X>` | Sets the target fill volume for all 4 heads (in mL). | `<SET_VOL,330>` |
| `<SET_CO2,X>` | Sets the CO2 flush duration (in milliseconds). | `<SET_CO2,1500>` |
| `<MAN_VLV,X,Y>`| Manual valve override (for cleaning). X = Valve ID (0=CO2, 1-4=Beer), Y = State (1=Open, 0=Closed). | `<MAN_VLV,2,1>` *(Opens Beer Valve 2)* |
| `<MAN_PNU,X,Y>`| Manual pneumatic override. X = Piston ID (1=Heads, 2=Gate), Y = State (1=Actuate, 0=Release). | `<MAN_PNU,1,1>` *(Lowers Heads)* |
| `<MAN_CNV,X>` | Manual conveyor override. X = State (1=On, 0=Off). | `<MAN_CNV,1>` |

---

## 4. Telemetry Dictionary: Worker to HMI (The Status)
These commands are sent continuously from the DevKitC (Worker) to the Touchscreen (HMI) to update the UI progress bars and text fields.

| Command string | Purpose | Example |
| :--- | :--- | :--- |
| `<STATE,X>` | Broadcasts the current State Machine phase. Used to highlight steps on the screen. (States: `IDLE`, `INDEXING`, `LOWERING`, `FLUSHING`, `FILLING`, `RAISING`) | `<STATE,FLUSHING>` |
| `<FILL,X,Y>` | Live flow meter update. X = Head (1-4), Y = Current volume (mL). Sent multiple times a second during filling. | `<FILL,3,150>` *(Head 3 is at 150mL)* |
| `<DONE,X>` | Confirms a specific head has reached its target volume and closed its valve. | `<DONE,3>` |
| `<BOTTLES,X>` | Live update of how many bottles are currently detected in the indexing gate (0-4). | `<BOTTLES,4>` |

---

## 5. Error & Safety Dictionary (Worker to HMI)
If the Worker detects a physical anomaly, it halts the process and sends an error code to the HMI to display a warning pop-up to the operator.

| Command string | Cause of Error | Suggested UI Action |
| :--- | :--- | :--- |
| `<ERR,NO_BOTTLE,X>` | Conveyor ran for max time, but sensor 'X' never saw a bottle. | Show "Clear Jam" popup. |
| `<ERR,HEAD_JAM>` | Pneumatic heads fired, but the "Heads Down" limit switch never triggered. | Show "Head Jammed" popup. |
| `<ERR,NO_FLOW,X>` | Valve 'X' opened, but flow meter 'X' isn't ticking (empty keg?). | Show "Check Keg / Flow" popup. |

---

## 6. The "Heartbeat" (Optional but Recommended)
Because this is an industrial machine with pressurized gas and liquid, it is highly recommended to implement a heartbeat.
* The HMI sends `<PING>` every 2 seconds.
* The Worker immediately replies with `<PONG>`.
* **Safety Rule:** If the Worker does not receive a `<PING>` for 5 seconds (meaning the screen crashed, or the serial wire snapped), the Worker automatically triggers an internal `<ESTOP>` and closes all valves.