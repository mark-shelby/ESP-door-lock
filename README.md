# ESP32 Smart Door Lock 

This repository contains the firmware for the Indoor Security Node of an IoT-enabled smart door lock system. The system utilizes an ESP32, an MFRC522 RFID reader, and a Relay module to control physical access. It features real-time database verification and logging via Firebase Realtime Database (RTDB) and uses ESP-NOW to receive scan data securely from a secondary outdoor ESP32 node.

## 🚀 Features

- **Real-time Authentication:** Verifies scanned RFID UIDs against a Firebase database.
- **Dual-Scanner Architecture:**
  - Acts as the "Exit" scanner using a physically connected MFRC522.
  - Listens for "Entry" scans via ESP-NOW from a remote outdoor ESP32.
- **Access Logging:** Automatically logs all entry and exit events with the user's name, scholar number, status (ENTRY/EXIT), and a precise Firebase server timestamp.
- **Relay Control:** Triggers an electronic door lock (relay) for 7 seconds upon successful authentication.

## 🛠️ Hardware Requirements

- 1x ESP32 Development Board
- 1x MFRC522 RFID Reader Module
- 1x Relay Module (Active LOW/HIGH depending on your specific lock hardware)
- Jumper wires & Breadboard/PCB
- An Electronic Door Strike or Magnetic Lock

## 📌 Pin Configuration

Wire your components to the ESP32 according to the following definitions in the code:

| Component | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| **Relay Signal** | GPIO 35 | Triggers the door lock |
| **RFID SDA (SS)**| GPIO 5 | MFRC522 Slave Select |
| **RFID RST** | GPIO 22 | MFRC522 Reset |
| **RFID SCK** | Standard SPI | Usually GPIO 18 on ESP32 |
| **RFID MOSI** | Standard SPI | Usually GPIO 23 on ESP32 |
| **RFID MISO** | Standard SPI | Usually GPIO 19 on ESP32 |

## 📚 Software Dependencies

Install the following libraries via the Arduino Library Manager:
- `MFRC522` by GithubCommunity
- `Firebase_ESP_Client` by Mobizt
- Built-in ESP32 libraries: `WiFi.h`, `esp_now.h`, `SPI.h`