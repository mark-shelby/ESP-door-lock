


void setup() {
    Serial.begin(115200);
    Serial.println("\n\nBOOTING INDOOR SECURITY NODE...");

    // 1. Setup Pins
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // Default Lock State

    // 2. Init Local RFID
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("✅ Hardware: RFID & Relay Initialized");

    // 3. Connect to Wi-Fi
    WiFi.mode(WIFI_AP_STA); // Dual Mode for ESP-NOW + WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("🌐 Wi-Fi: Connecting");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(300); Serial.print(".");
    }
    Serial.println("\n✅ Wi-Fi Connected!");
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
    Serial.println("✅ Firebase: Initialized");

    // 5. Initialize ESP-NOW
    if (esp_now_init() == ESP_OK) {
        Serial.println("✅ ESP-NOW: Initialized Successfully");
        esp_now_register_recv_cb(OnDataRecv);
    } else {
        Serial.println("❌ ESP-NOW: Init Failed (Restarting...)");
        delay(1000); ESP.restart();
    }
    
    Serial.println("🚀 SYSTEM READY. Waiting for Scans...");
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