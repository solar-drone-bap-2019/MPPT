#include "mbed.h"
#include "MPPT.h"

#define I1 PA_0
#define V1 PA_1
#define I2 PA_2
#define V2 PA_3
#define PWM1 PA_8
#define PWM2 PA_9

MPPT MPPT1(I1,V1,PWM1);
MPPT MPPT2(I2,V2,PWM2);

int main() {
    while(1) {
        MPPT1.PerturbObserve();
        MPPT2.PerturbObserve();
    }
}
