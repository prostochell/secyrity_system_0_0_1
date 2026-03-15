# 🛡️ ESP32-C3 Security System

![Assembled Board](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/board_soldered.jpg)

## 📝 Project Description

This project is a basic security system built around the high-performance **ESP32-C3** microcontroller. The system integrates a PIR motion sensor, a keypad for command input, an OLED display for status updates, and a siren for audible alerts.

The device allows a user to arm and disarm the security system, change the access password, and it reacts to motion detection by triggering visual and audible alarms.

## ✨ Key Features

- 💂 **Arming and Disarming:** Control the security state using the keypad (buttons '9' to arm, '0' to disarm).
- 🚶 **Motion Detection:** Uses an HC-SR501 PIR sensor to detect movement in the protected area.
- 🚨 **Alarm System:** When the sensor is triggered, a loud piezoelectric buzzer and flashing LEDs are activated.
- 🖥️ **Informative Display:** A monochrome OLED display shows the current system status ("Protection ON/OFF", "ALARM!", password prompts).
- 🔑 **Password Management:** Ability to set and change the access password. The password is saved in non-volatile storage (NVS).
- ⚙️ **Compact Design:** All components are housed on a custom 48x58 mm dual-layer printed circuit board.

## 🛠️ Hardware Components

| Component | Model | Purpose |
|:---|:---|:---|
| Microcontroller | **Espressif ESP32-C3** | 🧠 The "brain" of the system, managing all peripherals and logic. |
| Motion Sensor | **HC-SR501** | 👀 Passive Infrared (PIR) sensor for motion detection. |
| Display | **OLED 128x32 SSD1306** | 📟 Displays system information and status. |
| Keypad | **3x4 Matrix** | ⌨️ For entering commands and the password. |
| Piezo Buzzer | **CMI-1275C-050** | 🔊 Audible alarm (siren). |
| Transistor | **BC846** | 💪 Drives the LEDs and the buzzer. |
| LDO Regulator | **MIC5216-3.3** | ⚡ Provides a stable 3.3V power supply for the microcontroller. |

## 💻 Software and Tools

- **IDE:** `Visual Studio Code`
- **Framework:** `ESP-IDF` (Espressif IoT Development Framework)
- **PCB Design Software:** `Altium Designer`

## 👨‍💻 Software Details

The firmware was developed in **Visual Studio Code** using the **ESP-IDF** framework. The code architecture is built on the **FreeRTOS** real-time operating system, which allows for effective management of multiple processes simultaneously.

### 🏛️ Code Architecture

The core logic is based on two parallel tasks that run independently:

1.  `pir_task` — Solely responsible for monitoring the PIR motion sensor.
2.  `keypad_task` — Handles all keypad inputs and manages the system's state logic.

This approach ensures a rapid system response: a sensor trigger will be detected instantly, even if the user is entering a password at the same time.

### 💾 Settings Persistence

To save the password after a power cycle, the firmware uses the **NVS (Non-Volatile Storage)** library built into the ESP-IDF framework.

-   **Reading:** On boot, the password is read from NVS. If no password has been set, a default value ("1234") is used.
-   **Writing:** After a successful password change, the new value is written to NVS, ensuring it is saved permanently.

### 🚀 Boot Process (`app_main`)

The main `app_main` function performs a sequential system initialization:
1.  Initialize NVS for memory operations.
2.  Initialize the OLED display.
3.  Briefly show a logo on the screen.
4.  Read the saved password from NVS.
5.  Create and launch the `pir_task` and `keypad_task`.

After these steps, control is handed over to the FreeRTOS scheduler, which manages the parallel execution of the two tasks.

### Flowcharts

| Main Function (`app_main`) | PIR Sensor Task (`pir_task`) | Keypad Task (`keypad_task`) |
|:---:|:---:|:---:|
| ![Main flowchart](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/flowchart_main.png) | ![PIR flowchart](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/flowchart_pir.png) | ![Keypad flowchart](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/flowchart_keypad.png) |

## 🔌 Schematic and PCB

The board was designed in Altium Designer. It features two layers and has dimensions of 48x58 mm. The component placement was optimized to ensure stable operation of the ESP32-C3's Wi-Fi module.

![PCB Design](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/pcb_design.png)
_PCB Layout_

![3D PCB Model](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/pcb_3d.png)
_3D Render of the PCB_

## 📸 Gallery

The manufactured PCB and the final assembled device.

![Manufactured PCB](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/board_manufactured.jpg)
_Manufactured PCB (before soldering)_

![Assembled Board](https://raw.githubusercontent.com/prostochell/secyrity_system_0_0_1/main/img/board_assembled.jpg)
_Assembled Device_

