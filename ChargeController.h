#include "mbed.h"

float Ibat, Vbat, Iload, Vload, Pbat, Pload; // Variables for current, voltage and power for battery and load


/*
Control the power flow in the system using switches
*/
class ChargeController{
    private: 
        AnalogIn *BatCurrentSensor; // Pointer to the Battery Current Sensor object
        AnalogIn *BatVoltageSensor; // Pointer to the Battery Voltage Sensor object
        AnalogIn *LoadCurrentSensor; // Pointer to the Load Current Sensor object
        AnalogIn *LoadVoltageSensor; // Pointer to the Load Voltage Sensor object
        DigitalOut *PvSwitch; // This switch can disconnect the PV system from the batteries and load
        DigitalOut *BatSwitch; // This switch can disconnect the Batteries from the PV system, not allowing them to charge

    public:
        float Ppv; // Incoming power from solar panels
        ChargeController(PinName BatIPin, PinName BatVPin, PinName LoadIPin, PinName LoadVPin, PinName PvSwPin, PinName BatSwPin);
        void run();

};

ChargeController::ChargeController(PinName BatIPin, PinName BatVPin, PinName LoadIPin, PinName LoadVPin, PinName PvSwPin, PinName BatSwPin){
    AnalogIn Ib(BatIPin); // set BatCurrentPin as analog input source
    AnalogIn Vb(BatVPin); // set BatVoltagePin as analog input source
    AnalogIn Il(LoadIPin); // set LoadCurrentPin as analog input source
    AnalogIn Vl(LoadVPin); // set LoadVoltagePin as analog input source
    DigitalOut SwPv(PvSwPin); // set PvSwPin as digital output
    DigitalOut SwB(BatSwPin); // set BatSwPin as digital output
    

    // Store memory locations of In/Output Objects as class variables
    BatCurrentSensor = &Ib;
    BatVoltageSensor = &Vb;
    LoadCurrentSensor = &Il;
    LoadVoltageSensor = &Vl;
    PvSwitch = &SwPv;
    BatSwitch = &SwB;
};

void ChargeController::run(){
    Ibat = BatCurrentSensor->read();
    Vbat = BatVoltageSensor->read();
    Iload = LoadCurrentSensor->read();
    Vload = LoadVoltageSensor->read();
    Pbat = Ibat*Vbat;
    Pload = Iload*Vload;


}