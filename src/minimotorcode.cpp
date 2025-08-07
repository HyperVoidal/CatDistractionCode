#include <Arduino.h>
#include <Servo.h>

const int tServoPin = 11;
const int lpPin = 2;
const unsigned long toggleInterval = 3000;
unsigned long startmillis;
bool laserStatus = false;
int currentAngle = 0;

Servo tServo; // create servo object

void setup() {
    pinMode(tServoPin, OUTPUT);
    pinMode(lpPin, OUTPUT);
    tServo.attach(11, 600, 2300);
    Serial.begin(9600);
    randomSeed(analogRead(A0));
    startmillis = millis();
}


void randomisertoggle(int weightOFF, int weightON, unsigned long toggleInterval, int OutPin) {
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
        laserStatus = !laserStatus;
        //Serial.println(String("Laser set to: ") + (laserStatus ? "ON" : "OFF"));
        startmillis = currentMillis;
    } else {
        //Serial.println("No Laser Change");
    }

    //convert logic to high/low
    digitalWrite(OutPin, laserStatus ? HIGH : LOW);


}


int sampleTriangular(int min, int max, int center, float power) {
    /*
    This function is built from the planning phase of my Randomised Value Generation system.
    In order to achieve a system where each potential angle for turret movement is prioritised differently based 
    on proximity to the last chosen angle, the necessary control system was to create a triangular distribution, 
    prioritising values closer to a given centre in a random selection, then re-selecting based on the last selected 
    value. This process repeats in a loop to provide continuously smooth and semi-close values while still providing the opportunity for change.
    */
    float u = random(0, 10000) / 10000.0;  // Random float [0,1)
    float c = (float)(center - min) / (max - min);  // normalized center [0,1]

    float raw;
    if (u < c) {
        raw = min + sqrt(u * (max - min) * (center - min));
    } else {
        raw = max - sqrt((1 - u) * (max - min) * (max - center));
    }

    float offset = raw - center;

    float adjustedOffset = pow(abs(offset), power);  // Raise offset to power
    if (offset < 0) adjustedOffset *= -1;            // Restore direction

    int adjusted = round(center + adjustedOffset);

    // Optional: clamp to safe bounds
    return constrain(adjusted, -360, 360);
}

void motorRotateTurret(Servo& s, int maxTime) {
    int prevAngle = currentAngle;
    int newAngle = sampleTriangular(-361, 361, prevAngle, 1.0);
    Serial.println("Rotating to: " + newAngle);

    int step = (newAngle > prevAngle) ? 1 : -1;
    int steps = abs(newAngle - prevAngle);
    int durationMs = round((newAngle/360) * maxTime);
    int delayPerStep = durationMs / steps;

    for (int angle = prevAngle; angle != newAngle; angle += step) {
        s.write(angle);
        delay(delayPerStep);
    }
    s.write(newAngle);
}

void loop() {
    //Serial.println("Running servo direction control"); 
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
    motorRotateTurret(tServo, 2000);


}