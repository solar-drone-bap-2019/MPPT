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
3 = overcurrent protection (track lower PP)
4 = reset MPPT
*/
int Control; 

float Target; // Target variable to be sent to MPPTs in case they need to track a lower power point

// Pins for tracking group 1
#define I1 PA_0 // Current sensor
#define V1 PA_1 // Voltage sensor
#define PWM1 PA_3 // Pulse width modulation output

/* Pins for tracking group 2 */

//Pins for charge Controller
#define Vbat PA_5 // Battery voltage sensor
#define Iload PA_6 // Load current sensor
#define PvSw PA_7 // Pin out to switches connecting Solar cells to MPPTs
#define BatSw PA_8 // Switch which can enable battery charging


MPPT MPPT1(I1,V1,PWM1); // Create Maximum Power Point Tracker 1
/* Create Maximum Power Point Tracker 2 */

ChargeController CC(Vbat,Iload,PvSw,BatSw); // Create Charge Controller object


// define the Serial object, for debugging
Serial pc(USBTX, USBRX);

int main() {
    while(1) {
        CC.Ppv = MPPT1.readP(); // Send measured solar power to the charge controller
        CC.run(); // execute charge controller algorithm
        Control = CC.readControl(); // read control variable
        if (Control == 0) { // Track MPPT
            MPPT1.PerturbObserve(); // Execute the P&O algorithm for tracking group 1
            /* Execute the P&O algorithm for tracking group 2 */
        }
        else if (Control == 1) { // Pause the MPPT algorithm    
            MPPT1.pause(); 
            /* pause MPPT2 */
        }
        else if (Control == 2) { // Track Target power
            Target = CC.readPload(); // read Pload as target power.
            MPPT1.PerturbObserve(Target);
            /* Track target MPPT2 */
        }      
        else if (Control == 3){ // Prevent overcurrent in battery
            Target = CC.readPload() + CC.readPbatMax(); // Track load power + maximum allowed battery charging power
            MPPT1.PerturbObserve(Target);
        }
        else if (Control == 4){
            MPPT1.reset();
        }

        // MPPT doesn't need to happen very fast   
        wait_ms(1); // Track the power point every 10 ms
    }
}
