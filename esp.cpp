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