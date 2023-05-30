#include "nu32dip.h"
#define zeroMs .7f
#define oneEightyMs 2.7f
#define PR2Calced 59999

int setup() {
    T2CONbits.TCKPS = 4;     // Timer2 prescaler N=16 (1:16)
    PR2 = PR2Calced;              // period = (PR2+1) * N * (1/48000000) = 50 (.02)
    TMR2 = 0;                // initial TMR2 count is 0
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
    OC1CONbits.OCTSEL = 0;   // Use timer2
    OC1RS = 500;             // duty cycle (ms / 20) = OC1RS/(PR2+1) = 25%
    OC1R = 500;              // initialize before turning OC1 on; afterward it is read-only
    T2CONbits.ON = 1;        // turn on Timer2
    OC1CONbits.ON = 1;       // turn on OC1
    RPB15Rbits.RPB15R = 0b0101; // SEt OC pin
}

void setDuty(float degrees){
    float range = oneEightyMs - zeroMs;
    char m[100];
    unsigned int duty = (((range * (degrees / 180.0f)) + zeroMs)/20) * (PR2Calced + 1);
    sprintf(m, "%d\r\n", duty);
    NU32DIP_WriteUART1(m);
    OC1RS = duty;
}

void main(){
    setup();
    
    while(1) {
        //move the servo from 0 to 180 degrees every second
        setDuty(45);
        int time = _CP0_GET_COUNT();
        while(_CP0_GET_COUNT() - time < 24000000 * 4) {
            ;
        }
        
        //move the servo from 180 to 0 degrees every second
        
        setDuty(135);
        time = _CP0_GET_COUNT();
        while(_CP0_GET_COUNT() - time < 24000000 * 4) {
            ;
        }
        
    }
}