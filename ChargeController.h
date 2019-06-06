#include "mbed.h"

#define open 0
#define close 1

float Ibat, Vbat, Iload, Vload, Pbat; // Variables for current, voltage for battery and load, and power from battery
float SoC; // Variable for State of Charge of the battery pack
const float VbMax = 8; //[v] Maximum Battery voltage (SoC = 100)   //// Need to verify value

const int SoCMax = 95; // Maximum allowed battery State of Charge //// Need to verify value
const int SoCDanger = 97; // Dangerously high SoC //// Need to verify value
const int SoCMin = 20; // Minimum allowed battery SoC //// Need to verify value

const float BatCap = 3800; // [mAh] //// Need to verify value
const float IbatDanger = 2*BatCap/1000; // [A] maximum safe charging current
const float IbatMax = IbatDanger*0.8;

const float LOADCURRENT_SENSOR_MAX = 11; // [A] maximum current the sensor is able to sense properly
const float BATVOLTAGE_SENSOR_MAX = 9; // [V] maximum voltage the sensor is able to sense properly

/*
Control the power flow in the system using switches
*/
class ChargeController{
    private: 
        /* Control signal to be read by the main code. Will tell it how to control the MPPT.
        0 = track MPPT
        1 = pause MPPT
        2 = track Pload
        3 = track Pload + PbatMax (overcurrent protection)
        4 = reset MPPT 
        */
        int Control; 

        float Pload; // Power used by the load
        float PbatMax; // Maximum allowed battery charging power

        AnalogIn *BatVoltageSensor; // Pointer to the Battery Voltage Sensor object
        AnalogIn *LoadCurrentSensor; // Pointer to the Load Current Sensor object

        DigitalOut *PvSwitch; // This switch can disconnect the PV system from the batteries and load
        DigitalOut *BatSwitch; // This switch can disconnect the Batteries from the PV system, not allowing them to charge

        float readVbat(){
            return BatVoltageSensor->read()*BATVOLTAGE_SENSOR_MAX;
        };
        float readIload(){
            return LoadCurrentSensor->read()*LOADCURRENT_SENSOR_MAX;
        };

    public:
        float Ppv; // Incoming power from solar panels
        ChargeController(PinName BatVPin, PinName LoadIPin, PinName PvSwPin, PinName BatSwPin);
        void run(); // Execute charge controller algorithm
        int readControl(); // Allows the control signal to be read
        float readPload(); // Allows the load power to be read
        float readPbatMax(); // Allows PbatMax to be read
};

ChargeController::ChargeController(PinName BatVPin, PinName LoadIPin, PinName PvSwPin, PinName BatSwPin){
    BatVoltageSensor = new AnalogIn(BatVPin);
    LoadCurrentSensor = new AnalogIn(LoadIPin);
    PvSwitch = new DigitalOut(PvSwPin);
    BatSwitch = new DigitalOut(BatSwPin);
};

int ChargeController::readControl(){
    return Control; // Return control signal
}

float ChargeController::readPload(){
    return Pload; // Return Pload
}

float ChargeController::readPbatMax(){
    return PbatMax; 
}

void ChargeController::run(){
    Vbat = readVbat(); // read battery voltage
    SoC = Vbat/VbMax*100; // Determine state of charge [%] //// This formula is incorrect, need to change
    Iload = readIload(); // read load current
    Vload = Vbat; // Load is in parallel with the batteries   
    Pload = Iload*Vload; // Calculate power used by the load
    Ibat = (Ppv - Pload)/Vbat; // Calculate current flowing into battery
    PbatMax = IbatMax*Vbat; // Calculate maximum allowed battery charging power

    if(Ibat >= IbatDanger){ // Battery charging current is dangerously high
        Control = 4; // Reset MPPT (open MOSFET and reset duty cycle to zero)
        // open both switches
        PvSwitch->write(open); // PV power must be cut off,
        BatSwitch->write(open); // As the battery should quickly discharge to a safe level 

    }
    if (SoC >= SoCDanger) { // Battery charge is dangerously high
        Control = 1; // Pause MPPT (open MOSFET and pause algorithm)
        // open both switches
        PvSwitch->write(open); // PV power must be cut off,
        BatSwitch->write(open); // As the battery should quickly discharge to a safe level      
    }

    else if (SoC >= SoCMax && Ppv >= Pload) { // Both SoC and Ppv are too high
        Control = 2; // Tell main loop to read Pload. MPPTs must try to set Ppv equal to Pload
        PvSwitch->write(close); // Keep PV connected to supply load power
        BatSwitch->write(close); // Keep Battery connected to absorb excess current        
    }

    else if (Ibat >= IbatMax){ // Battery charging current is very high, but within limits
        Control = 3; // Track lower power point
        
        PvSwitch->write(close);
        BatSwitch->write(close);
        
    }    

    /*  Might need to implement some control algorithm in case of low voltage */
    else if (SoC <= SoCMin){ // Battery charge is too low
        Control = 0;
        PvSwitch->write(close);
        BatSwitch->write(close);
    }

     else 
    {
        Control = 0;
        PvSwitch->write(close); 
        BatSwitch->write(close);     
    }
}
