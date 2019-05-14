#include "mbed.h"
#include "MPPT.h"

/*
Tracks the maximum power point for multiple solar arrays (solar cell groups).
*/

// Pins for tracking group 1
#define I1 PA_0 // Current sensor
#define V1 PA_1 // Voltage sensor
#define PWM1 PA_8 // Pulse width modulation output

// Pins for tracking group 2
#define I2 PA_2 // Current sensor
#define V2 PA_3 // Voltage sensor
#define PWM2 PA_9 // Pulse width modulation output

MPPT MPPT1(I1,V1,PWM1); // Create Maximum Power Point Tracker 1
MPPT MPPT2(I2,V2,PWM2); // Create Maximum Power Point Tracker 2

int main() {
    while(1) {
        MPPT1.PerturbObserve(); // Execute the P&O algorithm for tracking group 1
        MPPT2.PerturbObserve(); // Execute the P&O algorithm for tracking group 2

        //MPPT doesn't need to happen very fast    
        wait_ms(10); // Track the power point every 10 ms
    }
}
