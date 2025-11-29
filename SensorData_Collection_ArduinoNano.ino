/* 
====================================================================
📡 PROJECT: BRAHMA-V1 CubeSat – Sensor Acquisition Unit (Arduino Nano)
--------------------------------------------------------------------
AUTHOR  : Omm Sanjog Barik  
LICENSE : MIT  
SYSTEM  : Distributed Computing (2x Arduino Nano + 1x Receiver Nano)

DESCRIPTION:
This Arduino code runs on the *Sensor Arduino (Nano-1)*.  
It reads data from ALL environmental sensors used in Brahma-V1:

  ✓ MQ-2       – Smoke / LPG / H₂  
  ✓ MQ-7       – CO  
  ✓ MQ-8       – Hydrogen  
  ✓ MQ-9       – CO / Methane  
  ✓ DHT22      – Temperature & Humidity  
  ✓ BMP280     – Pressure + Altitude  
  ✓ MPU6050    – Orientation (Accel + Gyro)  
  ✓ LDR        – Light Intensity  
  ✓ Voltage Sensor – Battery Monitoring

All sensor data is encoded into a compact JSON packet and 
sent via Serial to the *Transmission Arduino (Nano-2)*.
====================================================================
*/

#include <Wire.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <MPU6050.h>
#include <Adafruit_BMP280.h>

// ----- Pin Definitions -----
#define MQ8_PIN        A0
#define MQ2_PIN        A1
#define MQ9_PIN        A2
#define MQ7_PIN        A3
#define LDR_PIN        A6
#define VOLT_PIN       A7
#define DHT_PIN        2
#define DHT_TYPE       DHT22

// Constants
#define STD_PRESSURE 1013.25   // hPa
#define VOLTAGE_DIVIDER_RATIO 5.0  // (100k:20k = 5x scaling)

// Objects
DHT dht(DHT_PIN, DHT_TYPE);
MPU6050 mpu;
Adafruit_BMP280 bmp;  // I²C

void setup() {
  Serial.begin(9600);
  Wire.begin();
  dht.begin();

  // MPU6050 init
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU6050 OK" : "MPU6050 FAIL!");

  // BMP280 init
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 FAIL!");
  } else {
    Serial.println("BMP280 OK");
  }

  delay(1500);
}

void loop() {

  // ----- Read MQ Gas Sensors -----
  int mq8Value = analogRead(MQ8_PIN);
  int mq2Value = analogRead(MQ2_PIN);
  int mq9Value = analogRead(MQ9_PIN);
  int mq7Value = analogRead(MQ7_PIN);

  // ----- Read DHT22 -----
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    temperature = -999;
    humidity    = -999;
  }

  // ----- Read BMP280 -----
  float pressure = bmp.readPressure() / 100.0; // Pa → hPa  
  float altitude = bmp.readAltitude(STD_PRESSURE);

  // ----- Read MPU6050 -----
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // ----- Read LDR -----
  int ldrValue = analogRead(LDR_PIN);

  // ----- Read Battery Voltage -----
  int rawVolt = analogRead(VOLT_PIN);
  float voltage = (rawVolt * (5.0 / 1023.0)) * VOLTAGE_DIVIDER_RATIO;

  // ----- Create JSON -----
  StaticJsonDocument<500> doc;

  // Gas Sensors
  doc["MQ8"] = mq8Value;
  doc["MQ2"] = mq2Value;
  doc["MQ9"] = mq9Value;
  doc["MQ7"] = mq7Value;

  // Climate Sensors
  doc["Temperature"] = temperature;
  doc["Humidity"] = humidity;
  doc["Pressure"] = pressure;
  doc["Altitude"] = altitude;

  // Light
  doc["LDR"] = ldrValue;

  // Battery
  doc["Voltage"] = voltage;

  // Orientation Sensors
  JsonObject accel = doc.createNestedObject("Accelerometer");
  accel["x"] = ax;
  accel["y"] = ay;
  accel["z"] = az;

  JsonObject gyro = doc.createNestedObject("Gyroscope");
  gyro["x"] = gx;
  gyro["y"] = gy;
  gyro["z"] = gz;

  // ---- Serialize Data -----
  serializeJson(doc, Serial);
  Serial.println();

  delay(2000);  // 2 seconds sampling time
}

