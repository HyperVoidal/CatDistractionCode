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


void randomisertoggle(int weightOFF, int weightON, int toggleInterval, int OutPin) {
    /*
    Redesigned laser pointer toggle to now work with any random toggle system, factoring weights, intervals,
    and the pin to write data to. Depending on how I decide to handle the servo motor systems, this may affect that 
    or may instead just be used as a more optimal process for the laser pointer.
    */
    unsigned long currentMillis = millis();
    if (currentMillis - startmillis < toggleInterval) {
        return; //too soon, wait longer before trying
    }

    //set the value weights to either on or off
    int setValWeights = laserStatus ? weightOFF : weightON;

    //randomiser component and logic
    int toggleCheck = random(0, setValWeights);
    if (toggleCheck == 0) {
        laserStatus == !laserStatus;
        Serial.print(String("Laser set to: ") + (laserStatus ? "ON" : "OFF"));
    } else {
        Serial.print("Laser randomiser returned no switch value");
    }

    //convert logic to high/low
    digitalWrite(OutPin, laserStatus ? HIGH : LOW);


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
    randomisertoggle(10, 5, toggleInterval, lpPin);


}