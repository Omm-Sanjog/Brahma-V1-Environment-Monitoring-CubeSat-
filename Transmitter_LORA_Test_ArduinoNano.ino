/*
  =============================================
  =           LORA TRANSMITTER TEST           =
  =============================================
  Format to send:   S1:Hello
  Actual Packet:    M1>S1:Hello

  Author: Omm Sanjog Barik
  Satellite: Brahma-V1
*/

#include <SPI.h>
#include <LoRa.h>

#define SS   10
#define RST  9
#define DIO0 8

String masterID = "M1";   // Master Node ID

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("=====================================");
  Serial.println("         LORA TRANSMITTER TEST       ");
  Serial.println("=====================================\n");
  
  Serial.println("Enter: <ReceiverID>:<Message>");
  Serial.println("Example:  S1:Hello\n");

  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Initializing LoRa...");
  if (!LoRa.begin(433E6)) {
    Serial.println("[ERROR] LoRa init failed! Check wiring.");
    while (1);
  }

  // Match receiver configuration
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x34);
  LoRa.crc();

  Serial.println("[OK] LoRa initialized successfully.\n");
}

void loop() {
  if (Serial.available()) {

    // Read full input line
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Must have format: S1:Hello
    int sep = input.indexOf(':');
    if (sep == -1) {
      Serial.println("❌ Invalid format! Use  S1:Hello\n");
      return;
    }

    String targetID = input.substring(0, sep);
    String message  = input.substring(sep + 1);
    message.trim();

    // Final formatted LoRa packet:  M1>S1:Hello
    String packet = masterID + ">" + targetID + ":" + message;

    Serial.println("-------------------------------------");
    Serial.print  ("📤 Sending Packet: ");
    Serial.println(packet);

    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();

    Serial.println("✅ Packet Sent Successfully!\n");
  }
}
