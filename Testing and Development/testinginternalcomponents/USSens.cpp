#include <Arduino.h>

const int echoPin = 12;
const int triggerPin = 13;
unsigned long pulseValue; // to ensure buffer doesn't overflow

void setup()
{
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(triggerPin, LOW);
  delay(10); // to ensure we off it enough, just in case if the ultrasonic still echoes around which usually happens in a closed chamber.
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW); 

  pulseValue = pulseIn(echoPin, HIGH);

  Serial.print("Reading: ");
  Serial.println(pulseValue); // printing the pulse value directly. 
  //pulseValue should have a value above 0. If it doesn't, probably your sensor is kaput.
  delay(1000);
}
