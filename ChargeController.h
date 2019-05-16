#include "mbed.h"

#define open 0
#define close 1

float Ibat, Vbat, Iload, Vload, Pbat; // Variables for current, voltage for battery and load, and power from battery
float SoC; // Variable for State of Charge of the battery pack
const float VbMax = 12; // Maximum Battery voltage (SoC = 100)   //// Need to verify value
const int SoCMax = 95; // Maximum allowed battery State of Charge //// Need to verify value
const int SoCDanger = 97; // Dangerously high SoC //// Need to verify value

/*
Control the power flow in the system using switches
*/
class ChargeController{
    private: 
        /* Control signal to be read by the main code. Will tell it how to control the MPPT.
        0 = track MPPT
        1 = pause MPPT
        2 = track Pload
        */
        int Control; 

        float Pload; // Power used by the load

        AnalogIn *BatVoltageSensor; // Pointer to the Battery Voltage Sensor object
        AnalogIn *LoadCurrentSensor; // Pointer to the Load Current Sensor object

        DigitalOut *PvSwitch; // This switch can disconnect the PV system from the batteries and load
        DigitalOut *BatSwitch; // This switch can disconnect the Batteries from the PV system, not allowing them to charge

    public:
        float Ppv; // Incoming power from solar panels
        ChargeController(PinName BatVPin, PinName LoadIPin, PinName PvSwPin, PinName BatSwPin);
        void run(); // Execute charge controller algorithm
        int readControl(); // Allows the control signal to be read
        float readTarget(); // Allows the target variable to be read, description given in MPPT class

};

ChargeController::ChargeController(PinName BatVPin, PinName LoadIPin, PinName PvSwPin, PinName BatSwPin){
    AnalogIn Vb(BatVPin); // set BatVoltagePin as analog input source
    AnalogIn Il(LoadIPin); // set LoadCurrentPin as analog input source
    DigitalOut SwPv(PvSwPin); // set PvSwPin as digital output
    DigitalOut SwB(BatSwPin); // set BatSwPin as digital output
    

    // Store memory locations of In/Output Objects as class variables
    BatVoltageSensor = &Vb;
    LoadCurrentSensor = &Il;
    PvSwitch = &SwPv;
    BatSwitch = &SwB;
};

int ChargeController::readControl(){
    return Control; // Return control signal
}

float ChargeController::readTarget(){
    return Pload/Ppv; // Return fraction of Pload and Ppv
}

void ChargeController::run(){
    Vbat = BatVoltageSensor->read(); // read battery voltage
    SoC = VbMax/Vbat; // Determine state of charge //// This formula is incorrect, need to change
    Iload = LoadCurrentSensor->read(); // read load current
    Vload = Vbat; // Load is in parallel with the batteries   
    Pload = Iload*Vload; // Calculate power used by the load

    if (SoC >= SoCDanger) { // Battery charge is dangerously high
        Control = 1; // Pause MPPT (open MOSFET and pause algorithm)
        // open both switches
        PvSwitch->write(open); // PV power must be cut off,
        BatSwitch->write(open); // As the battery should quickly discharge to a safe level      
    }
    else if (SoC < SoCMax && PvSwitch->read()==open) // Battery has discharged to safe level, but PV is still disconnected
    {
        Control = 0; // Track MPPT
        PvSwitch->write(close); // Connect PV
        BatSwitch->write(close); // Close Battery switch in case PV power happens to be higher than l->oad power       
    }


    else if (SoC >= SoCMax && Ppv >= Pload) { // Both SoC and Ppv are too high
        Control = 2; // Tell main loop to read Pload. MPPTs must try to set Ppv equal to Pload
        PvSwitch->write(close); // Keep PV connected to supply load power
        BatSwitch->write(close); // Keep Battery connected to absorb excess current        
    }

    else if (SoC >= SoCMax && Ppv < Pload) { // SoC is too high but PV power is too low (so battery is discharging)
        Control = 0; // Track MPPT
        PvSwitch->write(close); // Keep PV connected to supply as much power as possible
        BatSwitch->write(open); // PV power is too low to charge the batteries       
    }

    else if (SoC < SoCMax && Ppv >= Pload) { // Battery can be charged, and PV is supplying enough power to do so
        Control = 0; // Track MPPT
        PvSwitch->write(close); // Connect PV
        BatSwitch->write(close); // Enable battery charging
    }

    else if (SoC < SoCMax && Ppv < Pload) { // Battery can be charged, but PV is not supplying enough power to do so
        Control = 0; // Track MPPT
        PvSwitch->write(close); // Connect PV
        BatSwitch->write(open); // PV power is too low to charge the batteries
    }


    

}
