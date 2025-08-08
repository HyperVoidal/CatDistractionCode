#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <algorithm>


#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

int currentAngle = 0;
int range = 80;

    /*
    This function is built from the planning phase of my Randomised Value Generation system.
    In order to achieve a system where each potential angle for turret movement is prioritised differently based 
    on proximity to the last chosen angle, the necessary control system was to create a triangular distribution, 
    prioritising values closer to a given centre in a random selection, then re-selecting based on the last selected 
    value. This process repeats in a loop to provide continuously smooth and semi-close values while still providing the opportunity for change.
    */
int sampleTriangular(int min, int max, int center) {
    float u = rand() / (RAND_MAX + 1.0f); // random float in [0, 1)
    
    float c = (float)(center - min) / (max - min);  // normalized center in [0,1]

    float x;
    if (u < c) {
        x = min + sqrt(u * (max - min) * (center - min));
    } else {
        x = max - sqrt((1 - u) * (max - min) * (max - center));
    }

    return std::clamp((int)round(x), min, max); //clamp to minimum and maximum values
}

int motorcontrol() {
    int prevAngle = currentAngle;
    int min = std::max(currentAngle - range, -361);
    int max = std::min(currentAngle + range, 361);
    int newAngle = sampleTriangular(min, max, prevAngle);
    std::cout << "Rotating to: " << newAngle << std::endl;

    int step = (newAngle > prevAngle) ? 1 : -1;
    int steps = std::abs(newAngle - prevAngle);

    // Ensure floating point division for durationMs
    int durationMs = (int)(2000.0 * steps / 720.0);
    int delayPerStep = (steps > 0) ? (durationMs / steps) : 0;

    for (int angle = prevAngle; angle != newAngle; angle += step) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayPerStep));
        std::cout << "Angle: " << angle << std::endl;
    }

    std::cout << "Angle: " << newAngle << std::endl;

    return newAngle;
}

int main() {
    while (true) {
        currentAngle = motorcontrol();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
