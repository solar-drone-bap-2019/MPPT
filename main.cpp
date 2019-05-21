#include "mbed.h"
#include "MPPT.h"
#include "ChargeController.h"

/*
Tracks the maximum power point for multiple solar arrays (solar cell groups),
And controls the power flow in the system, to maximize solar power usage, without overcharging the batteries.
*/

/* Control signal to be read from ChargeController.
0 = track MPPT
1 = pause MPPT
2 = track Pload
*/
int Control; 

float Target; // Target variable to be sent to MPPTs in case they need to track a lower power point

/* All pins still need to be verified */

// Pins for tracking group 1
#define I1 PA_0 // Current sensor
#define V1 PA_1 // Voltage sensor
#define PWM1 PA_8 // Pulse width modulation output

// Pins for tracking group 2
#define I2 PA_2 // Current sensor
#define V2 PA_3 // Voltage sensor
#define PWM2 PA_9 // Pulse width modulation output

//Pins for charge Controller
#define Vbat PA_4 // Battery voltage sensor
#define Iload PA_5 // Load current sensor
#define PvSw PA_6 // Pin out to switches connecting Solar cells to MPPTs
#define BatSw PA_7 // Switch which can enable battery charging


MPPT MPPT1(I1,V1,PWM1); // Create Maximum Power Point Tracker 1
MPPT MPPT2(I2,V2,PWM2); // Create Maximum Power Point Tracker 2

ChargeController CC(Vbat,Iload,PvSw,BatSw); // Create Charge Controller object

int main() {
    while(1) {
        CC.run();
        Control = CC.readControl();
        if (Control == 0) { // Track MPPT
            MPPT1.PerturbObserve(); // Execute the P&O algorithm for tracking group 1
            MPPT2.PerturbObserve(); // Execute the P&O algorithm for tracking group 2
        }
        else if (Control == 1) { // Pause the MPPT algorithm    
            MPPT1.pause(); 
            MPPT2.pause();
        }
        else if (Control == 2) { // Track Target power
            Target = CC.readTarget(); // read target variable.
            MPPT1.PerturbObserve(Target);
            MPPT2.PerturbObserve(Target);
        }      

        // MPPT doesn't need to happen very fast   
        wait_ms(10); // Track the power point every 10 ms
    }
}
