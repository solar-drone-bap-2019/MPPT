#include "mbed.h"

float I, V, P;

class MPPT{
  private:
    AnalogIn *CurrentSensor;
    AnalogIn *VoltageSensor;
    PwmOut *PwmOutput;
    float PreviousPower;
    float DutyCycle;
    float Perturbation;

  public:
    MPPT(PinName I_pin,PinName V_pin,PinName PWM_pin);
    float readI();
    float readV();
    void PerturbObserve();
};

MPPT::MPPT(PinName I_pin,PinName V_pin,PinName PWM_pin){
  AnalogIn Iin(I_pin);
  AnalogIn Vin(V_pin);
  PwmOut PWMout(PWM_pin);
  CurrentSensor = &Iin;
  VoltageSensor = &Vin;
  PwmOutput = &PWMout;
  Perturbation = 0.01;// the change in the duty cycle
  PwmOutput->period_us(100); // set PWM frequency to 10kHz (1/100us = 10kHz)
};

float MPPT::readI(){
  return CurrentSensor->read();
}

float MPPT::readV(){
  return VoltageSensor->read();
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
