#include <Arduino.h>
#include <SoftwareSerial.h>

//Planning section - 11/08/2025 at 7:40pm
//Pins in use: 2, 3, 4, 5, 6, 7, 8, 9, 11
//Pins Semi-free: 0, 1 -> Can't be attached to anything during uploads, so only good for post-testing deployment
//Pins free: 10, 12, 13
//Pin 12, 13 can be allocated to USSens and 10 to the laser pointer since that can be isolated or swapped to 0,1 for deployment
//Consider looking into using the analog pins for ENAs to free up extra space if necessary


SoftwareSerial bluetooth(2, 3); // RX, TX

bool Manual = false;
bool laserStatus = false;
unsigned long pulseValue;
bool forwardMove = true;

//Motor 1 Components
const int IN1=5;
const int IN2=6;
const int ENA=9; //needs to be 3, 5, 6, 9, 10, or 11 as these are the PWM specific pins
//Motor 2 Components
const int IN3=7;
const int IN4=8;
const int ENA2=11;
//Laser Output Pin
const int laserPin=4;
//US Sensor Pin
const int echoPin = 12;
const int triggerPin = 13;

int Speed = 135;

void setup() {
  Serial.begin(9600);    
  bluetooth.begin(9600);
  Serial.println("Arduino Bluetooth Ready!");
  //motor controls
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA2, OUTPUT);
  //US Sensor
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  //Laser Pin
  pinMode(laserPin, OUTPUT);
  
  //Adjust random seed generation to get reseeded random values from noise generation
  randomSeed(analogRead(A0));
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

void BrakeM1() {
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  analogWrite(ENA, 0);
}

void BrakeM2() {
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  analogWrite(ENA2, 0);
}

void checkDistance() {
  digitalWrite(triggerPin, LOW);
  delay(10); // to ensure we off it enough, just in case if the ultrasonic still echoes around which usually happens in a closed chamber.
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW); 
  pulseValue = pulseIn(echoPin, HIGH);
}

void NonManual(){
  //Main HC-SR04 controls, motor adjustments
  //Basically all the automated movement systems.
  //This may need to be split into multiple functions later down the line.
  checkDistance();
  if (pulseValue >= 750) { //750 is approx 20cm, determined from pretesting using the USSens.cpp file
    //execute main movement code
    /*
    current plans for movement code:
    Tank is initialised, moves forwards for a set duration, then rolls a random number check between 1 and 30. 
    If the value exceeds 25, the tank will initiate a turn.
    If the value equals 28, the tank will turn while maintaining forward direction.
    Turns will be determined by a 1 in 2 roll, with each value corresponding to one direction.
    */
    checkDistance();
    int randval = random(0, 31);
    if (randval >= 28) {
      BrakeM1();
      BrakeM2();
      int rand2 = random(1, 3); //returns 1 or 2, for some reason arduino random is inclusive of min but exclusive of max.
      if (rand2 == 1) {
        RightTurn((forwardMove ? 1 : 2), Speed);
      } else {
        LeftTurn((forwardMove ? 1 : 2), Speed);
      }
    } else if (randval >= 25) {
      int rand2 = random(1, 3); //returns 1 or 2, for some reason arduino random is inclusive of min but exclusive of max.
      if (rand2 == 1) {
        RightTurn(0, Speed);
      } else {
        LeftTurn(0, Speed);
      }
    } else {
      forwardMove ? Forward(Speed) : Backward(Speed);
    }
  } else {
    int randvalue = random(0, 2); //randomly select to either rotate left or right. Outside of while loop to avoid going back on previous movements
    do {
      //execute avoidance manouvres
      checkDistance(); //break to return to normal movements if stable pulsevalue is large enough that the tank can continue
      BrakeM1();
      BrakeM2();
      delay(400);
      Backward(Speed);
      delay(500);
      //rotate in place in direction according to randvalue
      if (randvalue == 0) {
        RightTurn(0, Speed);
      } else {
        LeftTurn(0 , Speed);
      }
      delay(500); //guessed about 45 degrees, needs verification
      BrakeM1();
      BrakeM2();
      delay(200); //pause to stabilise the US Sensor
    } while (pulseValue < 750);
  } 
}

void togglelaser() {
  laserStatus = !laserStatus;
  digitalWrite(laserPin, laserStatus); //booleans auto-map to high and low.

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

    if (received == "SWAP") {
      Manual = !Manual;
      }
    if (received == "LASER") {
        togglelaser();
      }
    if (received == "STOP") {
      BrakeM1();
      BrakeM2();
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
      } else if (received == "0, 1") {
        LeftTurn(1, Speed);
      } else if (received == "0, 3") {
        RightTurn(1, Speed);
      } else if (received == "2, 1") {
        LeftTurn(2, Speed);
      } else if (received == "2, 3") {
        RightTurn(2, Speed);
      } else {
        // Do nothing or stop motors
      }
    }
  }
}
