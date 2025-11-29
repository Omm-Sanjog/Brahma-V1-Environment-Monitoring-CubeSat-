/*
=====================================================================
                       📡 Brahma-V1 | LoRa Receiver
=====================================================================

This Arduino Nano functions as the **LoRa ground-station receiver**
for the Brahma-V1 Miniature Satellite project.

It receives JSON telemetry packets sent by the LoRa transmitter
on the satellite module (Arduino Nano) and prints them for logging
on the laptop.

─────────────────────────────────────────────────────────────
🔧 Hardware Used
─────────────────────────────────────────────────────────────
• Arduino Nano  
• LoRa SX1278 (RA-02) Module  
• AMS1117-3.3V regulator (recommended)  
• USB cable  
• Jumper wires

─────────────────────────────────────────────────────────────
📡 LoRa → Arduino Nano Wiring
─────────────────────────────────────────────────────────────
  LoRa Pin      →   Nano Pin
  --------------------------------
  VCC           →   3.3V
  GND           →   GND
  SCK           →   D13
  MISO          →   D12
  MOSI          →   D11
  NSS / CS      →   D10
  RST           →   D9
  DIO0          →   D8

⚠️ Important:
Do NOT power LoRa SX1278 with 5V. It MUST be 3.3V.

─────────────────────────────────────────────────────────────
📨 Expected Packet Format
─────────────────────────────────────────────────────────────
Incoming LoRa packet format:

            M1:{ JSON DATA }

Where:
• M1 = Transmitter ID  
• JSON DATA = Telemetry payload sent by satellite module

Example:
    M1:{"Temp":25.3,"Hum":78.1,"MQ7":412}

Receiver extracts:  
✔ Sender ID  
✔ JSON payload  
✔ RSSI (signal strength)

─────────────────────────────────────────────────────────────
👨‍💻 Author: Omm Sanjog Barik  
📘 Project: Brahma-V1 NanoSat Prototype  
📄 License: MIT  
=====================================================================
*/

#include <SPI.h>
#include <LoRa.h>

// ----- LoRa Pin Configuration -----
#define SS   10
#define RST  9
#define DIO0 8

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("=== LoRa Receiver Ready ===");

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed! Check wiring.");
    while (1);
  }

  // Match transmitter parameters
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x34);
  LoRa.enableCrc();

  Serial.println("LoRa initialized successfully!");
  Serial.println("Waiting for incoming JSON packets...");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }

    // Print raw received message
    Serial.println("\n--- Received LoRa Packet ---");
    Serial.print("Raw: ");
    Serial.println(receivedData);

    // Optional: Split ID and JSON if message is like "M1:{...}"
    int colonIndex = receivedData.indexOf(':');
    if (colonIndex > 0) {
      String senderID = receivedData.substring(0, colonIndex);
      String jsonData = receivedData.substring(colonIndex + 1);

      Serial.print("Sender ID: ");
      Serial.println(senderID);
      Serial.print("JSON Data: ");
      Serial.println(jsonData);
    } else {
      Serial.println("No sender ID found, raw JSON:");
      Serial.println(receivedData);
    }

    // Print RSSI for signal strength info
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.println("-----------------------------");
  }
}
