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

## ⚙️ Setup & Configuration

**1. Configure Credentials:**
Open the code and edit the `CONFIGURATION` section with your details:

```cpp
#define WIFI_SSID       "realme100"
#define WIFI_PASS       "shashank100"
#define API_KEY         "AIzaSyA0CYXI_qk7wNhXORAfssX0F-xLlVBuuxU"
#define DATABASE_URL    "https://lab-security-db-default-rtdb.firebaseio.com/"
#define USER_EMAIL      "esp32@lab.com"
#define USER_PASSWORD   "esp32@lab.com"

2. Sync ESP-NOW Wi-Fi Channels:
ESP-NOW requires the Sender (Outdoor ESP) and Receiver (Indoor ESP) to be on the same Wi-Fi channel.
When this Indoor node boots up, it will print its current Wi-Fi channel to the Serial Monitor.
You must hardcode your Outdoor ESP node to use this exact channel.

🔍 How It Works
-Initialization: The ESP32 connects to Wi-Fi, authenticates with Firebase, initializes the local RFID reader, 
and sets up ESP-NOW to listen for incoming data.

-Local Scan (Exit): If a user scans their card on the indoor reader, the ESP32 reads the UID, checks it
 against the /Members database, opens the relay, and logs the event as "EXIT".

-Remote Scan (Entry): If the outdoor ESP32 reads a card, it sends the 4-byte UID to this indoor ESP32 
via ESP-NOW. The indoor node processes the UID, checks the database, opens the relay, and logs the event as "ENTRY".