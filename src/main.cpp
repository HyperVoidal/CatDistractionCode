#include <Arduino.h>
#include <SoftwareSerial.h>


SoftwareSerial bluetooth(2, 3); // RX, TX

bool Manual = false;

void setup() {
  Serial.begin(9600);    
  bluetooth.begin(9600);
  Serial.println("Arduino Bluetooth Ready!");
}

void LeftTurn(int direction) {
  if (direction == 1) {
    //Direction is forward
  } else if (direction == 2) {
    //Direction is backward
  } else {
    //Direction is 0 or otherwise, tank remains stationary
  }
}

void RightTurn(int direction) {
  if (direction == 1) {
    //Direction is forward
  } else if (direction == 2) {
    //Direction is backward
  } else {
    //Direction is 0 or otherwise, tank remains stationary
  }
}

void Forward(){

}

void Backward(){

}

void ModeSwap(){

}

void Laser(){

}

void loop() {
  if (bluetooth.available()) {
    String received = bluetooth.readStringUntil('\n'); //Received will end up as 0 or 1 or 2 or 3 or 0,1 or 0,3 or 2,1 or 2,3 or SWAP or LASER
    received.trim();

    Serial.print("Received from Bluetooth: ");
    Serial.println(received);

    // Respond back to Bluetooth
    bluetooth.println("Received: " + received);

    if (received == "SWAP") {
      Manual = !Manual;
    }
    if (received == "LASER") {
        Laser();
      }
    
    if (Manual == false) {
      //No control functions are accepted
    } else {
      //I wanted to do a case switch here but apparently that doesn't work with string comparisons in arduino code
      if (received == "0") {
        Forward();
      } else if (received == "1") {
        LeftTurn(0);
      } else if (received == "2") {
        Backward();
      } else if (received == "3") {
        RightTurn(0);
      } else if (received == "0,1") {
        LeftTurn(1);
      } else if (received == "0,3") {
        RightTurn(1);
      } else if (received == "2,1") {
        LeftTurn(2);
      } else if (received == "2,3") {
        RightTurn(2);
      } else {
        // Do nothing or stop motors
      }
    }
  }
}
