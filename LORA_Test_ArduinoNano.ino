/*
  =============================================
  =        LORA MODULE TEST (SX1278)          =
  =============================================
  This code checks:
    - SPI communication
    - LoRa initialization
    - Frequency set
    - Register read test
*/

#include <SPI.h>
#include <LoRa.h>

#define SS   10
#define RST  9
#define DIO0 8

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("=====================================");
  Serial.println("       LORA MODULE TEST START        ");
  Serial.println("=====================================");
  delay(500);

  Serial.println("Setting LoRa pins...");
  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Initializing LoRa...");
  if (!LoRa.begin(433E6)) {
    Serial.println("[ERROR] LoRa Init Failed!");
    Serial.println("Check wiring, power & antenna.");
  } 
  else {
    Serial.println("[OK] LoRa Init Successful!");
    Serial.print("Frequency Set To: ");
    Serial.println(LoRa.getFrequency());
  }

  // Read a sample register to verify SPI communication
  byte version = LoRa.readRegister(0x42);
  Serial.print("LoRa Chip Version (0x42): ");
  Serial.println(version, HEX);

  if (version == 0x12) {
    Serial.println("[OK] LoRa Chip Responding!");
  } else {
    Serial.println("[ERROR] Unexpected Version! SPI wiring issue?");
  }

  Serial.println("-------------------------------------");
  Serial.println("Test Completed.");
  Serial.println("-------------------------------------");
}

void loop() {
  // Nothing here — test is printed only once
}
