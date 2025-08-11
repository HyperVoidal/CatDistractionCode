#include <Arduino.h>

int direction = false; //bool value for direction. false means left and true means right
int rotationspeed = 0.1; //10% of max rotation speed of motor
const int laserpin = 25; //pin for where the laser draws power
const int motorpin1 = 10; //first pin for the second motor driver
const int motorpin2 = 11; //second pin for the second motor driver

int directionrandom;
int randomlaser;


bool checkdirectionswap() {
    directionrandom = random(1, 10);
    if (directionrandom > 3) {
        //do not toggle direction
    } else {
        direction = not direction;
    }
}

void togglelaser() {
    randomlaser = random(1, 10);
    if (randomlaser > 2) {
        //do not toggle laser
    } else {
        int pinState = digitalRead(laserpin);
        if (pinState == HIGH) {
            digitalWrite(laserpin, LOW);
        } else {
            digitalWrite(laserpin, HIGH);
        }
    }
}

void setup() {
    //activate all integrated pins
    pinMode(laserpin, OUTPUT);
    pinMode(motorpin1, OUTPUT);
    pinMode(motorpin2, OUTPUT);
}

