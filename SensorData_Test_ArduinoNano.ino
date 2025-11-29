/* 
===============================================================
 BRAHMA-V1 – ALL SENSOR TEST CODE (Arduino Nano)
---------------------------------------------------------------
 Prints raw values of all connected sensors to Serial Monitor.
 Use this to verify wiring & sensor operation before integration.
===============================================================
*/

#include <Wire.h>
#include <DHT.h>
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

// Battery calculation
#define VOLTAGE_DIVIDER_RATIO 5.0   // Adjust if you use 100k:20k divider

// Objects
DHT dht(DHT_PIN, DHT_TYPE);
MPU6050 mpu;
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  dht.begin();

  Serial.println("Initializing sensors...");

  // Init MPU6050
  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU6050: OK" : "MPU6050: FAIL!");

  // Init BMP280
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280: FAIL!");
  } else {
    Serial.println("BMP280: OK");
  }

  delay(2000);
}

void loop() {

  Serial.println("===== SENSOR TEST OUTPUT =====");

  // MQ Sensors
  Serial.print("MQ-8 (H2): "); Serial.println(analogRead(MQ8_PIN));
  Serial.print("MQ-2 (Smoke/LPG): "); Serial.println(analogRead(MQ2_PIN));
  Serial.print("MQ-9 (CH4/CO): "); Serial.println(analogRead(MQ9_PIN));
  Serial.print("MQ-7 (CO): "); Serial.println(analogRead(MQ7_PIN));

  // Light Sensor
  Serial.print("LDR: "); Serial.println(analogRead(LDR_PIN));

  // Battery Voltage
  int rawVolt = analogRead(VOLT_PIN);
  float voltage = (rawVolt * (5.0 / 1023.0)) * VOLTAGE_DIVIDER_RATIO;
  Serial.print("Battery Voltage: "); Serial.print(voltage); Serial.println(" V");

  // DHT22
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  Serial.print("Temperature (C): "); Serial.println(isnan(t) ? -999 : t);
  Serial.print("Humidity (%): "); Serial.println(isnan(h) ? -999 : h);

  // BMP280
  Serial.print("Pressure (hPa): "); Serial.println(bmp.readPressure() / 100.0);
  Serial.print("Altitude (m): "); Serial.println(bmp.readAltitude(1013.25));

  // MPU6050 – Accel + Gyro
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.println("--- MPU6050 ---");
  Serial.print("Accel X: "); Serial.println(ax);
  Serial.print("Accel Y: "); Serial.println(ay);
  Serial.print("Accel Z: "); Serial.println(az);

  Serial.print("Gyro X: "); Serial.println(gx);
  Serial.print("Gyro Y: "); Serial.println(gy);
  Serial.print("Gyro Z: "); Serial.println(gz);

  Serial.println("==============================\n");

  delay(1500); // Read every 1.5s
}

