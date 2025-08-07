#include <Arduino.h>
#include <Servo.h>

const int tServoPin = 11;
const int lpPin = 2;
unsigned long startmillis = millis();
const unsigned long toggleInterval = 3000;
int checkmillis = 0;
bool laserStatus = false;

Servo tServo; // create servo object

void setup() {
    pinMode(tServoPin, OUTPUT);
    pinMode(lpPin, OUTPUT);
    tServo.attach(11, 600, 2300);
    Serial.begin(9600);
    randomSeed(analogRead(A0));
}

void lasertoggle() {
    /*
    This is a system to intuitively toggle the laser pointer in a way that favours being on compared to off.
    The basic system design is to run a randomiser to toggle the laser every few seconds, where the chance to
    switch off is weighted more heavily to whatever mode (on/off) it is currently in.
    */
    unsigned long currentMillis = millis();
    if (currentMillis - startmillis < toggleInterval) {
        return; // Too soon, wait more
    }

    int weightOffL = 10; //weighting against turning the laser off
    int weightOnL = 5; //weighting against turning the laser on

    //run seperate laser randomisers dependent on whether the laser is on or off
    int setValWeights = laserStatus ? weightOffL : weightOnL;

    //randomiser component and logic
    int toggleCheck = random(0, setValWeights);
    if (toggleCheck == 0) {
        laserStatus = !laserStatus;
        Serial.print("Laser set to ");
        Serial.println(laserStatus ? "ON" : "OFF");
    } else {
        Serial.println("Laser randomiser failed, retaining status");
    }
    
    //convert logic to high/low outputs
    digitalWrite(lpPin, laserStatus ? HIGH : LOW);
    
}

void loop() {
    Serial.println("Running servo direction control"); 
    // Note to self: Servo direction control recommended for implementing proper weighted 
    // random movements to laser pointer in emulating animal movement patterns
    tServo.write(0); //initial angle
    delay(1000);
    tServo.write(90);
    delay(500);
    tServo.write(180);
    delay(1500);

    //run laser pointer randomiser
    lasertoggle();


}