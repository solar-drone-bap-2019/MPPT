#include "mbed.h"

float I, V, P; // Variables for Current, Voltage and Power


/* Maximum Power Point Tracker Class
Control the duty cycle of a DC-DC converter switch using PWM,
to optimize the power flow.
Power is calculated using current and voltage sensors.
*/
class MPPT{
  private:
    AnalogIn *CurrentSensor;
    AnalogIn *VoltageSensor;
    PwmOut *PwmOutput;
    float PreviousPower; // Power measured previous iteration
    float DutyCycle; // Duty cycle of PWM output signal
    float Perturbation; // Change in the duty cycle every iteration

  public:
    MPPT(PinName I_pin,PinName V_pin,PinName PWM_pin);
    float readI(); // read from current sensor
    float readV(); // read from voltage sensor
    void PerturbObserve(); // Perturb and Observe MPP Tracking algorithm
};

MPPT::MPPT(PinName I_pin,PinName V_pin,PinName PWM_pin){
  AnalogIn Iin(I_pin); // Set I_pin as an Analog Input source
  AnalogIn Vin(V_pin); // Set V_pin as an Analog Input source
  PwmOut PWMout(PWM_pin); // Set PWM_pin as a PWM output

  //Store locations of In/Output Objects as class variables
  CurrentSensor = &Iin;
  VoltageSensor = &Vin;
  PwmOutput = &PWMout;

  Perturbation = 0.01;// the change in the duty cycle
  PwmOutput->period_us(100); // set PWM frequency to 10kHz (1/100us = 10kHz)
};

float MPPT::readI(){
  return CurrentSensor->read(); // return read current
}

float MPPT::readV(){
  return VoltageSensor->read(); // return read voltage
}

void MPPT::PerturbObserve(){
  //observe
  I = CurrentSensor->read(); // read current sensor
  V = VoltageSensor->read(); // read voltage sensor
  P = I*V; // calculate power

  //perturb
  if (P < PreviousPower) { // if previous perturbation resulted in loss of power
    Perturbation = -Perturbation; // reverse perturbation direction
  }
  DutyCycle = DutyCycle+Perturbation; // Apply Perturbation

  // Constrain duty cycle to be in between 0 and 1
  if (DutyCycle < 0) {DutyCycle = 0;}
  else if (DutyCycle > 1){DutyCycle = 1;}
  
  PwmOutput->write(DutyCycle); // write PWM
  
  PreviousPower = P; // store calculated power  
}
