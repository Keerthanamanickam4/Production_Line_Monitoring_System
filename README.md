# Line Monitoring System using PIC16F877A, LCD (4-bit), GSM, IR Sensor & Buzzer

A real-time **industrial line monitoring system** built using the **PIC16F877A** microcontroller.  
The system continuously monitors the production line status (**Running / Stopped**) and sends alerts through a **GSM module**, while displaying real-time information on a **16x2 LCD**.

---

## Features

* IR sensor detects line movement (object presence)
* 16x2 LCD displays:
  - Line status: **Running / Stopped**
  - Total running time (in seconds)
* GSM module (SIM800 / SIM900) sends:
  - **"Line Running"** → sent once when the line starts
  - **"Line Stopped: X sec"** → sent once when the line stops
* Buzzer activates when the line stops
* Non-blocking timing logic for accurate LCD updates
* Fully compatible with 8-bit **PIC16F877A** using **4-bit LCD mode**

---

## System Working

* The system continuously monitors the IR sensor to determine whether the production line is running or stopped.
* When the IR sensor output becomes **0**, the line is considered **running**.
* In the running state:
  - The LCD displays **"Line Running"** along with the current running time in seconds.
  - The buzzer remains **OFF**.
  - The internal timer increments every second.
  - The GSM module sends the **"Line Running"** message only once when the line starts.
* When the IR sensor output becomes **1**, the line is considered **stopped**.
* In the stopped state:
  - The LCD displays **"Line Stopped"** along with the total time the line was running before it stopped.
  - The buzzer turns **ON** to alert the user.
  - The GSM module sends a single SMS: **"Line Stopped: X sec"**, where **X** is the running duration.
* After sending the stop message:
  - The timer resets to zero.
  - The system waits for the next running and stopping cycle.

---

## Software Flow

1. Initialize LCD, UART, and GPIO ports
2. Read IR sensor input
3. Detect state transitions:
   - Running → Stopped
   - Stopped → Running
4. Update LCD every 1 second
5. Send SMS only on state change
6. Activate buzzer during stoppage

---

## Important Notes

* Operates at **8 MHz** crystal/oscillator
* GSM module requires stable power 
* UART baud rate: **9600 bps**
* IR sensor logic: **0 = Object detected (Running)**
* LCD operates in **4-bit mode** (RD4–RD7 for data lines)

---

## Testing Procedure

1. Power ON the system
2. LCD displays **System Ready**
3. Place an object in front of the IR sensor → Line Running
4. Remove the object → Line Stopped
5. Verify:
   - LCD status updates
   - GSM SMS alerts
   - Buzzer ON/OFF behavior

---

## Author

**Keerthana M**  
Embedded Software Engineer

