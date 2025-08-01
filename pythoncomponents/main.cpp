#include <Arduino.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled!
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  while (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  while (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.write(c);
    SerialBT.write(c);
  }
}

/*
Main loop ideas - constantly scan BT serial port for commands. If in manual mode, respond to all commands
ie. directions of forwards + backwards, turns and their associated movements, STOP and SWAP
if not in manual mode, still scan everything but only respond to SWAP to know to reconfigure to manual mode
*/