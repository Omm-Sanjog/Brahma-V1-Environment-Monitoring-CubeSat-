/*
=====================================================================
                       📡 Brahma-V1 | LoRa Transmitter
=====================================================================

This Arduino Nano acts as the **LoRa transmission node** in the
Brahma-V1 Miniature Satellite project.

It receives JSON sensor packets from another Arduino Nano via Serial
and transmits them over LoRa to the ground station receiver.

─────────────────────────────────────────────────────────────
🔧 Hardware Used
─────────────────────────────────────────────────────────────
• Arduino Nano  
• LoRa SX1278 (RA-02) Module  
• AMS1117-3.3V regulator (recommended)  
• USB cable for Serial communication  
• Dupont wires

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

⚠️ Note:
The SX1278 MUST run at **3.3V**, not 5V.  
Always attach the antenna before powering on.

─────────────────────────────────────────────────────────────
📨 Packet Format
─────────────────────────────────────────────────────────────
Outgoing LoRa packet structure:

            M1:{JSON DATA}

Where:
• M1 = Transmitter (Master) ID  
• JSON Data = Sensor values sent by the acquisition Nano

Example:
    M1:{"Temp":25.3,"Hum":70.2,"MQ2":381}

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

String incomingData = "";
String masterID = "M1";  // ID for this LoRa transmitter

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("=== Serial-to-LoRa Transmitter ===");

  // ----- LoRa Setup -----
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed! Check wiring.");
    while (1);
  }

  // Matching parameters with your receiver setup
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x34);
  LoRa.enableCrc();

  Serial.println("LoRa initialized successfully!");
  Serial.println("Receiver Ready to forward Serial data...");
}

void loop() {
  // Read incoming JSON data from Serial (from other Nano)
  if (Serial.available()) {
    incomingData = Serial.readStringUntil('\n');
    incomingData.trim();

    if (incomingData.length() > 0) {
      Serial.print("Received: ");
      Serial.println(incomingData);

      // Transmit via LoRa
      Serial.println("Sending via LoRa...");
      LoRa.beginPacket();
      LoRa.print(masterID + ":" + incomingData);
      LoRa.endPacket();
      Serial.println("LoRa Transmission Sent!");
    }
  }
}
