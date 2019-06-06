#include "mbed.h"

const float PERTURB_CONST = 0.01;
const float PERIOD_US = 100; // [us] PWM period
const float CURRENT_SENSOR_MAX = 11; // [A] maximum current the sensor is able to sense properly
const float VOLTAGE_SENSOR_MAX = 9; // [V] maximum voltage the sensor is able to sense properly

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
    float readP(); // return last calculated power

    /* Perturb and Observe MPP Tracking algorithm.
    Optional target power argument in case a lower power point needs to be tracked.*/
    void PerturbObserve(float Target); 

    void pause(); // stop tracking power point, open PWM switch
    void reset(); // stop tracking power point, reset duty cycle to zero
};

MPPT::MPPT(PinName I_pin,PinName V_pin,PinName PWM_pin){
  CurrentSensor = new AnalogIn(I_pin);
  VoltageSensor = new AnalogIn(V_pin);
  PwmOutput = new PwmOut(PWM_pin);

  Perturbation = PERTURB_CONST;// the change in the duty cycle
  PwmOutput->period_us(PERIOD_US); // set PWM frequency to 10kHz (1/100us = 10kHz)
};

float MPPT::readI(){
  return CurrentSensor->read() * CURRENT_SENSOR_MAX; // return read current
}

float MPPT::readV(){
  return VoltageSensor->read() * VOLTAGE_SENSOR_MAX; // return read voltage
}

float MPPT::readP(){
  return PreviousPower; // return last calculated power
}

void MPPT::PerturbObserve(float Target = 0){
  //observe
  I = readI(); // read current sensor
  V = readV(); // read voltage sensor
  P = I*V; // calculate power

  /*perturb*/
  if (Target == 0) { // No target given -> Track MPPT
    if (P < PreviousPower) { // if previous perturbation resulted in loss of power
      Perturbation = -Perturbation; // reverse perturbation direction
    }
    
  }
  else { // Track target
    if (P>Target){
      Perturbation = -abs(Perturbation); // set perturbation to negative
    }
    else {
      Perturbation = abs(Perturbation); // set perturbation to positive
    }  
  }
  DutyCycle = DutyCycle+Perturbation; // Apply Perturbation
  /*       */

  // Constrain duty cycle to be in between 0 and 1
  if (DutyCycle < 0) {DutyCycle = 0;}
  else if (DutyCycle > 1){DutyCycle = 1;}
  
  PwmOutput->write(DutyCycle); // write PWM
  
  PreviousPower = P; // store calculated power  
}

void MPPT::pause(){
  /* Don't set DutyCycle variable to zero,
  because we want to save it for when MPPT is resumed. 
  Instead just write a zero to PwmOutput*/
  PwmOutput->write(0); // Open MOSFET
  I = readI(); // read current sensor
  V = readV(); // read voltage sensor
  P = I*V; // calculate power (should be zero)
  PreviousPower = P;
}

void MPPT::reset(){
  /* reset DutyCycle to zero */
  DutyCycle = 0;
  PwmOutput->write(DutyCycle);
  I = readI(); // read current sensor
  V = readV(); // read voltage sensor
  P = I*V; // calculate power (should be zero)
  PreviousPower = P;
}
