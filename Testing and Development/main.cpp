#include <Arduino.h>
#include <SoftwareSerial.h>

//Planning section - 11/08/2025 at 7:40pm
//Pins in use: 2, 3, 4, 5, 6, 7, 8, 9, 11
//Pins Semi-free: 0, 1 -> Can't be attached to anything during uploads, so only good for post-testing deployment
//Pins free: 10, 12, 13
//Pin 12, 13 can be allocated to USSens and 10 to the laser pointer since that can be isolated or swapped to 0,1 for deployment
//Consider looking into using the analog pins for ENAs to free up extra space if necessary

//in an ideal world where I could have the rotating turret, pin10 would be the last available pinset to attach 
//the motor to, since the one I would use could operate from the arduino.


SoftwareSerial bluetooth(2, 3); // RX, TX

bool Manual = false;
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

String received;

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
  digitalWrite(laserPin, LOW);
  
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

unsigned long pulseValue;

void checkDistance() {
  digitalWrite(triggerPin, LOW);
  delay(10); // to ensure we off it enough, just in case if the ultrasonic still echoes around which usually happens in a closed chamber.
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW); 
  pulseValue = pulseIn(echoPin, HIGH, 20000);
  if (pulseValue == 0) {
    pulseValue == 99999;
  }
}

// ===== AUTO MOVEMENT =====
enum AutoState { AUTO_FORWARD, AUTO_TURN, AUTO_BACKWARD };
AutoState autoState = AUTO_FORWARD;

unsigned long autoStart = 0;
unsigned long autoDuration = 0;
bool rightturn = false; //false == left, true == right

void updateAutoMovement() {
    unsigned long now = millis();
    checkDistance(); // updates global pulseValue

    if (pulseValue < 750) {
        // Too close: go backward
        if (autoState != AUTO_BACKWARD) {
            autoState = AUTO_BACKWARD;
            autoStart = now;
            autoDuration = 1000; // move backward for 1 second
            Backward(Speed);
        } else if (now - autoStart >= autoDuration) {
            // After backing up, go into a turn
            autoState = AUTO_TURN;
            autoStart = now;
            autoDuration = 800;
            rightturn = random(0, 2);
            rightturn ? RightTurn(0, Speed) : LeftTurn(0, Speed);
        }
        return; // skip other logic while backing up
    }

    if (pulseValue >= 750 && pulseValue < 1200) {
        if (autoState != AUTO_TURN && now - autoStart >= autoDuration) {
            // Obstacle in warning range: do a turn
            autoState = AUTO_TURN;
            autoStart = now;
            autoDuration = (random(0, 2) == 0) ? 1500 : 800;
            rightturn = random(0, 2);
            rightturn ? RightTurn(0, Speed) : LeftTurn(0, Speed);
        } else if (autoState == AUTO_TURN && now - autoStart >= autoDuration) {
            // After turning, go forward again
            autoState = AUTO_FORWARD;
            autoStart = now;
            autoDuration = 2000;
            Forward(Speed);
        }
    } else {
      // Clear path: move forward, but occasionally vary movement
      if (autoState != AUTO_FORWARD || now - autoStart >= autoDuration) {
          autoState = AUTO_FORWARD;
          autoStart = now;

          // Randomly decide if we go straight or curve
          int choice = random(1, 11);  // 1-10 inclusive
          if (choice == 1) {
              // Arc left
              autoDuration = 1500;
              LeftTurn(0, Speed);
          } else if (choice == 2) {
              // Arc right
              autoDuration = 1500;
              RightTurn(0, Speed);
          } else {
              // Go straight
              autoDuration = 2000;
              Forward(Speed);
          }
        }
      }
    }

// ===== LASER TOGGLE =====
bool laserStatus = false;
unsigned long lastLaserToggle = 0;
const unsigned long laserDebounce = 200; // ms

void tryToggleLaser() {
    unsigned long now = millis();
    if (now - lastLaserToggle > laserDebounce) {
        laserStatus = !laserStatus;
        digitalWrite(laserPin, laserStatus);
        lastLaserToggle = now;
    }
}

// ===== Bluetooth System =====
void bluetoothCheck() {
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

void manualControl() {
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
  } else if (received == "STOP") {
    BrakeM1();
    BrakeM2();
    delay(5000); //sudden stop in situation such as emergency, enables user to take hands off controls to pick up tank
  } else if (received == "N/A") { //pause when no controls are being entered
    BrakeM1();
    BrakeM2(); 
  } else {
    //do nothing idk
  }
}



void loop() {
    if (Manual) {
      manualControl();
    } else if (Manual == false) {
      //No control functions are accepted
      updateAutoMovement();
      if (received == "SWAP") {
        Manual = !Manual;
      } else if (received == "LASER") {
        tryToggleLaser();
      } else if (received == "STOP") {
        BrakeM1();
        BrakeM2();
        delay(5000); //sudden stop in situation such as emergency, enables user to take hands off controls to pick up tank
      }
    } else {
      
    }
}

