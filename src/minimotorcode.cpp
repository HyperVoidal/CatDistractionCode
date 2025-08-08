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


int sampleTriangular(int mini, int maxi, int center) {
    /*
    This function is built from the planning phase of my Randomised Value Generation system.
    In order to achieve a system where each potential angle for turret movement is prioritised differently based 
    on proximity to the last chosen angle, the necessary control system was to create a triangular distribution, 
    prioritising values closer to a given centre in a random selection, then re-selecting based on the last selected 
    value. This process repeats in a loop to provide continuously smooth and semi-close values while still providing the opportunity for change.
    */
    float u = rand() / (RAND_MAX + 1.0f); // random float in [0, 1)
    float c = (float)(center - mini) / (maxi - mini);
    float x;

    if (u < c) {
        x = mini + sqrt(u * (maxi - mini) * (center - mini));
    } else {
        x = maxi - sqrt((1-u) * (maxi - mini) * (maxi - center));
    }

    return max(min((int)round(x), maxi), mini); //clamp to the minimum and maximum values
}

int motorRotateTurret(Servo& s, float maxTime, int range) {
    int prevAngle = currentAngle;
    int mini = max(currentAngle - range, -361);
    int maxi = min(currentAngle + range, 361);
    int newAngle = sampleTriangular(mini, maxi, prevAngle);
    Serial.println(String("Rotating to: ") + newAngle);

    int step = (newAngle > prevAngle) ? 1 : -1;
    int steps = abs(newAngle - prevAngle);

    //ensure floatingpoint division for durationMs
    int durationMs = (int)(maxTime * steps / 720.0);
    int delayPerStep = (steps > 0) ? (durationMs / steps) : 0;

    for (int angle = prevAngle; angle != newAngle; angle += step) {
        delay(delayPerStep);
        s.write(angle);
    }

    //after for loop, ensure final angle is guaranteed to be met by re-calling it
    s.write(newAngle);
    Serial.println(String("Rotated to: ") + newAngle);
    return newAngle;
}

void loop() {
    /*
    Next steps - adjust the motorRotateTurret function to only output the angles and time delays, then handle the rotations incrementally during the main loop
    this is so I can avoid freezing the other components while rotates are running
    */


    //run laser pointer randomiser
    randomisertoggle(10, 5, toggleInterval, lpPin);
    //update currentAngle to new rotation point
    currentAngle = motorRotateTurret(tServo, 2000.0, 80);
}