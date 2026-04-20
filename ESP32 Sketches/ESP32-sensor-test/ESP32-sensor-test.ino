#include <Wire.h>

void setup() {
  Serial.begin(9600);
  delay(2000);

  Wire.begin();   // uses XIAO ESP32C6 default I2C pins
  Serial.println("I2C scanner starting...");
}

void loop() {
  byte error, address;
  int count = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found I2C device at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.println("Done.");
  }

  delay(3000);
}