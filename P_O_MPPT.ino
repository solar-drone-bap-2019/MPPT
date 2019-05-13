//current and voltage sensors
#define CURRENT_1 PA0 
#define VOLTAGE_1 PA1
#define CURRENT_2 PA2
#define VOLTAGE_2 PA3

//PWM pins for DC-DC converter switches
#define PWM_1      PA8
#define PWM_2      PA9

//set up software counter to reduce effective clock speed
int counter_lim = 720000;
int counter;


float Perturbation = 0.01; // the change in the duty cycle

float CURRENT_ARRAY[2] = {CURRENT_1, CURRENT_2};
float VOLTAGE_ARRAY[2] = {VOLTAGE_1, VOLTAGE_2};
float PWM_ARRAY[2] = {PWM_1, PWM_2};
float P_prev_ARRAY[2];
float Perturbation_ARRAY[2] = {Perturbation,Perturbation};
float Duty_ARRAY[2];

//declare variables for current, voltage and power
float I,V,P;


void setup()
{
  for (int N=0;N < sizeof(CURRENT_ARRAY);N++) { // for all MPPT groups
    pinMode(CURRENT_ARRAY[N], INPUT); // set current sensor as input
    pinMode(VOLTAGE_ARRAY[N], INPUT); // set voltage sensor as input
    pinMode(PWM_ARRAY[N], OUTPUT); // set PWM signal as output
  }
}

void loop()
{
  
counter += 1;
if (counter == counter_lim){ 
  MPPT(1);
  MPPT(2);
  
  counter = 0;
}

}

//Track maximum power point using the 'Perturb and Observe' algorithm
void MPPT(int N) // N = number of MPPT group
{  
  //observe
  I = analogRead(CURRENT_ARRAY[N]); // read current sensor
  V = analogRead(VOLTAGE_ARRAY[N]); // read voltage sensor
  P = I*V; // calculate power

  //perturb
  if (P < P_prev_ARRAY[N]) { // if previous perturbation resulted in loss of power
    Perturbation_ARRAY[N] = -Perturbation_ARRAY[N]; // reverse perturbation direction
  }
  Duty_ARRAY[N] = constrain(Duty_ARRAY[N]+Perturbation_ARRAY[N],0,1); // Apply perturbation as long as duty cycle stays in between 0 and 1
  analogWrite(PWM_ARRAY[N],Duty_ARRAY[N]*255); // write PWM

  
  P_prev_ARRAY[N] = P; // write calculated power in array
}
 


