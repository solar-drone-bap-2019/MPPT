 #include <Arduino.h>

class MPPT {
  private:
    int I_PIN; // current sensor pin
    int V_PIN; // voltage sensor pin
    int PWM_PIN; // pwm output pin
  public:
    MPPT(int I_pin, int V_pin, int PWM_pin);
  
    float Pprev; // previously measured power
    float perturbDir; // perturbation direction
    float dutyCycle; // PWM duty cycle

    float ReadI();
    float ReadV();
    void WritePWM();
};

  MPPT::MPPT(int I_pin,int V_pin,int PWM_pin){
    I_PIN = I_pin;
    V_PIN = V_pin;
    PWM_PIN = PWM_pin;

    pinMode(I_pin,INPUT);
    pinMode(V_pin,INPUT);
    pinMode(PWM_pin,OUTPUT);

  }

  float MPPT::ReadI(){
    return analogRead(I_PIN);
  }

  float MPPT::ReadV(){
    return analogRead(V_PIN);
  }

  void MPPT::WritePWM(){
    analogWrite(PWM_PIN,dutyCycle);
  }