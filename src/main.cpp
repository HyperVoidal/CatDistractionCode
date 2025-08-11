#include <Arduino.h>
#include <SoftwareSerial.h>


SoftwareSerial bluetooth(2, 3); // RX, TX

bool Manual = false;

const int IN1=5;
const int IN2=6;
const int ENA=9; //needs to be 3, 5, 6, 9, 10, or 11 as these are the PWM specific pins
const int IN3=7;
const int IN4=8;
const int ENA2=11;
int Speed = 135;

void setup() {
  Serial.begin(9600);    
  bluetooth.begin(9600);
  Serial.println("Arduino Bluetooth Ready!");
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);
}

void LeftTurn(int direction, int Speed) {
  if (direction == 1) {
    //Direction is forward overall
    //Speed is normal for right side motor
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, Speed);
    //Speed is halved for left side motor
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, constrain((Speed/2), 130, 150)); //speed is minimum 130 maximum 150 to avoid unpowering the motor or going too fast 
    //both motors rotate in the same direction but right side rotates faster, creating a wide turn arc to the left

  } else if (direction == 2) {
    //Direction is backward
    //Speed is normal but backwards for right side motor
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, Speed);
    //SPeed is halved for left side motor but backwards
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA2, constrain((Speed/2), 130, 150));
  } else {
    //Direction is 0 or otherwise, tank remains stationary while turning
    //M1 (right) rotates forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, Speed);
    //M2 (left) rotates backward
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENA2, Speed);
  }
}

void RightTurn(int direction, int Speed) {
  if (direction == 1) {
    //Direction is forward
    //Speed is halved for right side motor
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, constrain((Speed/2), 130, 150));
    //Speed is normal for left side motor
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, Speed);
  } else if (direction == 2) {
    //Direction is backward
    //Speed is halved for the right side motor
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, constrain((Speed/2), 130, 150));
    //Speed is normal for left side motor
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, Speed);
  } else {
    //Direction is 0 or otherwise, tank remains stationary while turning
    //M1 (right) rotates forward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, Speed);
    //M2 (left) rotates backward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, Speed);
  }
}

void Forward(int Speed){
  //M1 inputs
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, Speed);
  //M2 inputs
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA2, Speed);
}

void Backward(int Speed){
  //M1 inputs
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, Speed);
  //M2 inputs
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA2, Speed);
}

void NonManual(){
  //Main HC-SR04 controls, motor adjustments
  //Basically all the automated movement systems.
  //This may need to be split into multiple functions later down the line.
}

void Laser(){
  //Enable Laser pin or disable laser pin
  //This function should be a toggle, essentially
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
      NonManual();
    } else {
      //I wanted to do a case switch here but apparently that doesn't work with string comparisons in arduino code
      if (received == "0") {
        Forward(Speed);
      } else if (received == "1") {
        LeftTurn(0, Speed);
      } else if (received == "2") {
        Backward(Speed);
      } else if (received == "3") {
        RightTurn(0, Speed);
      } else if (received == "0,1") {
        LeftTurn(1, Speed);
      } else if (received == "0,3") {
        RightTurn(1, Speed);
      } else if (received == "2,1") {
        LeftTurn(2, Speed);
      } else if (received == "2,3") {
        RightTurn(2, Speed);
      } else {
        // Do nothing or stop motors
      }
    }
  }
}
