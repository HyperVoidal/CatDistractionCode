#include <Arduino.h>

const int IN1=12; // 5 for ARD
const int IN2=14; // 6 for ARD
const int ENA=13; // 7 for ARD


void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
}

void Motor1_Forward(int Speed) {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(ENA,Speed);
}

void Motor1_Backward(int Speed) {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  analogWrite(ENA,Speed);
}

void Motor1_Brake() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  analogWrite(ENA, 0);
}

void loop() {
  Motor1_Brake();
  //Motor2_Brake();
  delay(100);
  Motor1_Forward(200);
  //Motor2_Forward(200);
  delay(1000);
  Motor1_Brake();
  //Motor2_Brake();
  delay(100);
  Motor1_Backward(200);
  //Motor2_Backward(200);
  delay(1000);
}

