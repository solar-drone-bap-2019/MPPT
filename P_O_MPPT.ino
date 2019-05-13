#include <Arduino.h>
#include <MPPT.h>

//current and voltage sensors
#define I1 PA0 
#define V1 PA1
#define I2 PA2
#define V2 PA3

//PWM pins for MPPT DC-DC converter switches
#define PWM1      PA8
#define PWM2      PA9

float perturbation = 0.01; // the change in the duty cycle

  MPPT MPPT1(I1,V1,PWM1);
  MPPT MPPT2(I2,V2,PWM2);

//declare variables for current, voltage and power
float I,V,P;


void setup()
{  
}

void loop()
{  
  P_O(MPPT1);
  P_O(MPPT2);
  delay(100);
}

//Track maximum power point using the 'Perturb and Observe' algorithm
void P_O(MPPT &mppt) 
{  
  //observe
  I = mppt.ReadI(); // read current sensor
  V = mppt.ReadV(); // read voltage sensor
  P = I*V; // calculate power

  //perturb
  if (P < mppt.Pprev) { // if previous perturbation resulted in loss of power
    mppt.perturbDir = -mppt.perturbDir; // reverse perturbation direction
  }
  mppt.dutyCycle = constrain(mppt.dutyCycle+perturbation*mppt.perturbDir,0,1); // Apply perturbation as long as duty cycle stays in between 0 and 1
  mppt.WritePWM(); // write PWM
  
  mppt.Pprev = P; // store calculated power
}
 

