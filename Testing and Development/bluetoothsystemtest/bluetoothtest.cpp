#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial bluetooth(2, 3); // RX, TX

String received;

void setup() {
  Serial.begin(9600);    
  bluetooth.begin(9600);
  Serial.println("Arduino Bluetooth Ready!");
}

void loop() {
  if (bluetooth.available()) {
    String received = bluetooth.readStringUntil('\n'); //Received will end up as 0 or 1 or 2 or 3 or 0,1 or 0,3 or 2,1 or 2,3 or SWAP or LASER
    received.trim();
    received.replace(" ", "");

    Serial.print("Received from Bluetooth: ");
    Serial.println(received);

    // Respond back to Bluetooth
    bluetooth.println("Received: " + received);
  }
}