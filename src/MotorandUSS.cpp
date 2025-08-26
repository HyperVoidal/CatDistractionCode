#include <Arduino.h>

const int IN2=5;
const int IN1=6;
const int ENA=9; //needs to be 3, 5, 6, 9, 10, or 11 as these are the PWM specific pins
const int IN3=7;
const int IN4=8;
const int ENA2=10;
int speed = 130;

const int echoPin = 12;
const int triggerPin = 13;
unsigned long pulseValue; // to ensure buffer doesn't overflow


void setup() {
  delay(5000);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);
  Serial.begin(9600);

  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
}

void Motor1_Backward(int Speed) {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  analogWrite(ENA,Speed);
}

void Motor1_Forward(int Speed) {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);
  analogWrite(ENA,Speed);
}

void Motor1_Brake() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  analogWrite(ENA, 0);
}

void Motor2_Backward(int Speed) {
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);
  analogWrite(ENA2, Speed);
}

void Motor2_Forward(int Speed) {
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);
  analogWrite(ENA2, Speed);
}

void Motor2_Brake() {
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  analogWrite(ENA2, 0);
}

void LeftTurn(int Speed) {
  //Direction is 0 or otherwise, one tread is stationary and the other takes full power
  //M1 (right) rotates forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, Speed);
  //M2 (left) doesn't move
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA2, 0);
  delay(800);
}

void RightTurn(int Speed) {
  //Direction is 0 or otherwise, one tread is stationary and the other takes full power
    //M1 (right) doesn't move
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
    //M2 (left) rotates forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, Speed);
    delay(800);
}

/* void LeftTurn(int direction, int Speed) {
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
    delay(800);

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
    delay(800);

  } else {
    //Direction is 0 or otherwise, one tread is stationary and the other takes full power
    //M1 (right) rotates forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, Speed);
    //M2 (left) rotates backward
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA2, 0);
    delay(800);
  }
} */

void checkUltrasonic() {
  digitalWrite(triggerPin, LOW);
  delay(10); // to ensure we off it enough, just in case if the ultrasonic still echoes around which usually happens in a closed chamber.
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW); 

  pulseValue = pulseIn(echoPin, HIGH);
}

void loop() {
  checkUltrasonic(); // Always check distance
  
  float distance = pulseValue * 0.034 / 2; // Convert time to distance in cm using speed of sound calc
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance < 20) {  // Adjust this threshold as needed
    // Obstacle detected!
    Motor1_Brake();
    Motor2_Brake();
    delay(500); // Small pause

    Motor1_Backward(speed);
    Motor2_Backward(speed);
    delay(700); // Move backward for a bit

    Motor1_Brake();
    Motor2_Brake();
    delay(500); // Stop again
    
    
    int dirturn = random(0, 2);
    dirturn ? RightTurn(speed) : LeftTurn(speed);
    Motor1_Brake();
    Motor2_Brake();
  } else {
    // Path is clear
    Motor1_Forward(speed);
    Motor2_Forward(speed);
  }

  delay(100); // Short delay to avoid excessive sensor reading
}

