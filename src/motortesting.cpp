#include <Arduino.h>

const int IN1=5;
const int IN2=6;
const int ENA=9; //needs to be 3, 5, 6, 9, 10, or 11 as these are the PWM specific pins
int speed = 150;


void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  Serial.begin(9600);
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
  while (true) {
  Motor1_Brake();
  //Motor2_Brake();
  delay(250);
  Motor1_Forward(speed);
  //Motor2_Forward(200);
  delay(750);
  Motor1_Brake();
  //Motor2_Brake();
  delay(250);
  /*
  Motor1_Backward(speed);
  Motor2_Backward(200);
  delay(750);
  */
  //Serial.println(speed);
  //speed = speed - 25;
  }
}

