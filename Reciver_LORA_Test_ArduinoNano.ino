/*
  =============================================
  =          LORA RECEIVER TEST (S1)          =
  =============================================
  This code:
    - Initializes LoRa
    - Listens for incoming packets
    - Prints RAW packet
    - Extracts  Sender, Receiver, Message
    - Confirms if packet is meant for this node
*/

#include <SPI.h>
#include <LoRa.h>

#define SS   10
#define RST  9
#define DIO0 8

String slaveID = "S1";  // Change ID: S1, S2, S3, ...

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("=====================================");
  Serial.print  ("     LORA RECEIVER TEST (ID: ");
  Serial.print  (slaveID);
  Serial.println(")");
  Serial.println("=====================================\n");

  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Initializing LoRa...");
  if (!LoRa.begin(433E6)) {
    Serial.println("[ERROR] LoRa init failed! Check wiring.");
    while (1);
  }

  // Match transmitter (important)
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x34);
  LoRa.crc();

  Serial.println("[OK] LoRa init success!");
  Serial.println("Waiting for incoming packets...\n");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    Serial.println("-------------------------------------");
    Serial.println("📥  PACKET RECEIVED");
    Serial.print("Packet Size: ");
    Serial.println(packetSize);

    // Read raw packet
    String packet = "";
    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }

    Serial.print("RAW DATA : ");
    Serial.println(packet);

    // Expected Format:  M1>S1:Hello
    int arrowIndex = packet.indexOf('>');
    int colonIndex = packet.indexOf(':');

    if (arrowIndex == -1 || colonIndex == -1) {
      Serial.println("[WARN] Invalid packet format!");
      Serial.println("-------------------------------------\n");
      return;
    }

    // Extract parts
    String senderID   = packet.substring(0, arrowIndex);
    String receiverID = packet.substring(arrowIndex + 1, colonIndex);
    String message    = packet.substring(colonIndex + 1);

    Serial.print("Sender ID   : ");
    Serial.println(senderID);

    Serial.print("Receiver ID : ");
    Serial.println(receiverID);

    Serial.print("Message     : ");
    Serial.println(message);

    // Check if message is for this receiver
    if (receiverID == slaveID) {
      Serial.println("✔ This message is for ME.");
    } else {
      Serial.println("✖ Message NOT for this receiver.");
    }

    Serial.println("-------------------------------------\n");
  }
}
