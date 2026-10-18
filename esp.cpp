
// B. The Core Logic: Check DB -> Open Door -> Log It
void checkAccessAndLog(String uidStr, String status) {
    Serial.println("------------------------------------------------");
    Serial.print("🔍 CHECKING DATABASE FOR UID: "); Serial.println(uidStr);

    if (Firebase.ready()) {
        // 1. Construct Path: e.g., /Members/917BFE00
        String memberPath = "/Members/" + uidStr;

        // 2. Fetch Data
        Serial.print("📡 QUERY: Reading from " + memberPath + "... ");
        if (Firebase.RTDB.getJSON(&fbdo, memberPath)) {
            
            // Check if data actually exists
            if (fbdo.dataType() == "json") {
                Serial.println("✅ FOUND!");
                
                // 3. Parse JSON to get Name and Scholar No
                FirebaseJson &json = fbdo.jsonObject();
                FirebaseJsonData result;
                
                // Get Name
                json.get(result, "name");
                String memberName = result.stringValue;
                
                // Get Scholar Number
                json.get(result, "scholar_number");
                String scholarNum = result.stringValue;

                Serial.printf("👤 MEMBER IDENTIFIED: %s (Scholar: %s)\n", memberName.c_str(), scholarNum.c_str());

                // 4. OPEN THE DOOR
                triggerRelay();

                // 5. PUSH LOG TO DB
                Serial.print("📝 LOGGING: Uploading entry to /Entry_log... ");
                
                FirebaseJson logEntry;
                logEntry.add("uid", uidStr);
                logEntry.add("name", memberName);
                logEntry.add("scholar_number", scholarNum);
                logEntry.add("status", status); // "ENTRY" or "EXIT"
                
                // FIX: Correct way to set Server Timestamp in this library
                // This creates { "timestamp": { ".sv": "timestamp" } }
                logEntry.set("timestamp/.sv", "timestamp"); 

                if (Firebase.RTDB.pushJSON(&fbdo, "/Entry_log", &logEntry)) {
                    Serial.println("✅ SUCCESS");
                } else {
                    Serial.println("❌ LOG FAILED: " + fbdo.errorReason());
                }

            } else {
                Serial.println("❌ FAILED. (Data is null/empty)");
            }
        } else {
            // If the path doesn't exist (Member not registered)
            Serial.println("❌ ACCESS DENIED (Member not found in DB)");
            Serial.println("REASON: " + fbdo.errorReason());
        }
    } else {
        Serial.println("⚠️ ERROR: Firebase not ready/disconnected.");
    }
    Serial.println("------------------------------------------------");
}



// C. Callback for ESP-NOW (Outdoor Scanner)
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
    Serial.print("📡 ESP-NOW: Data Received (" + String(len) + " bytes). ");
    
    // Safety check for data length
    if (len == 4) {
        String uidStr = "";
        for (int i = 0; i < 4; i++) {
            uidStr += String(incomingData[i] < 0x10 ? "0" : "");
            uidStr += String(incomingData[i], HEX);
        }
        uidStr.toUpperCase();
        
        Serial.println("Processing External Scan...");
        // Call the main logic with status "ENTRY"
        checkAccessAndLog(uidStr, "ENTRY");
    } else {
        Serial.println("⚠️ IGNORED: Invalid data length.");
    }
}

// ---------------------------------------------------------------------------
// 4. SETUP
// ---------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    Serial.println("\n\nBOOTING INDOOR SECURITY NODE...");

    // 1. Setup Pins
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // Default Lock State

    // 2. Init Local RFID
    SPI.begin();
    rfid.PCD_Init();
    Serial.println(" Hardware: RFID & Relay Initialized");

    // 3. Connect to Wi-Fi
    WiFi.mode(WIFI_AP_STA); // Dual Mode for ESP-NOW + WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print(" Wi-Fi: Connecting");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(300); Serial.print(".");
    }
    Serial.println("\n Wi-Fi Connected!");
    Serial.print("   IP Address: "); Serial.println(WiFi.localIP());
    // * IMPORTANT DEBUG INFO FOR OUTDOOR ESP *
    Serial.print("   CHANNEL: "); Serial.println(WiFi.channel()); 
    Serial.println("   (Make sure Outdoor ESP is hardcoded to this channel!)");

    // 4. Initialize Firebase
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    // FIX: Removed the token_status_callback line to prevent errors
    // config.token_status_callback = tokenStatusCallback; 

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    Serial.println(" Firebase: Initialized");

    // 5. Initialize ESP-NOW
    if (esp_now_init() == ESP_OK) {
        Serial.println(" ESP-NOW: Initialized Successfully");
        esp_now_register_recv_cb(OnDataRecv);
    } else {
        Serial.println(" ESP-NOW: Init Failed (Restarting...)");
        delay(1000); ESP.restart();
    }
    
    Serial.println(" SYSTEM READY. Waiting for Scans...");
}

// ---------------------------------------------------------------------------
// 5. LOOP (Handles Exit/Local Scanner)
// ---------------------------------------------------------------------------
void loop() {
    // A. Keep Firebase Alive is handled automatically

    // B. Check Local RFID (Exit Logic)
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        Serial.println("👋 LOCAL SCAN DETECTED (Inside/Exit)");

        String uidStr = "";
        for (byte i = 0; i < rfid.uid.size; i++) {
            uidStr += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
            uidStr += String(rfid.uid.uidByte[i], HEX);
        }
        uidStr.toUpperCase();

        // Call logic with status "EXIT"
        checkAccessAndLog(uidStr, "EXIT");

        // Halt PICC
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }
}