#include <Arduino.h>

const int IN1=5;
const int IN2=6;
const int ENA=9; //needs to be 3, 5, 6, 9, 10, or 11 as these are the PWM specific pins
const int IN3=7;
const int IN4=8;
const int ENA2=11;
int speed = 135;


void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);
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

void Motor2_Forward(int Speed) {
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENA, Speed);
}

void Motor2_Backward(int Speed) {
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  analogWrite(ENA, Speed);
}

void Motor2_Brake() {
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  analogWrite(ENA, 0);
}

void loop() {
  while (true) {
    Serial.println("Hello!");
    Motor1_Brake();
    Motor2_Brake();
    delay(250);
    Motor1_Forward(speed);
    Motor2_Forward(speed);
    delay(750);
    Motor1_Brake();
    Motor2_Brake();
    delay(750);
    Motor1_Backward(speed);
    Motor2_Backward(speed);
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

