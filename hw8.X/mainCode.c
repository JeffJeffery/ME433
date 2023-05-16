#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "mpu6050.h"
#include "ssd1306.h"
#include "font.h"
#include <stdio.h>
#include <xc.h>

void blink(int, int); // blink the LEDs function


void drawLetter(unsigned char ascii_code, unsigned char x, unsigned char y){
    unsigned char AsciiBits[5];
    for(int i=0; i<5; i++){
        AsciiBits[i] = ASCII[ascii_code - 0x20][i];
    }
    
    for(int i=0; i<5; i++){
        for(int j=0; j<8; j++){ 
           ssd1306_drawPixel(x + i, y + j, (AsciiBits[i] >> j & 0b1));
        }
    }
}

void drawMessage(char* charArr, unsigned char x, unsigned char y){
    int i = 0;
    while (charArr[i]){
        drawLetter(charArr[i], x, y);
        x += 5;
        if(x >= 125){
            x = 0;
            y += 8;
        }
        if (y >31){
            charArr[i+1] = 0;
        }
        i++;
    }
}



int main(void) {
    //Initialization
    NU32DIP_Startup();
    i2c_master_setup();
    ssd1306_setup();
    init_mpu6050();
    
    char toWrite[100]; //what we are going to write
    float t2;
    
    // char array for the raw data
    unsigned char d[14];
	// float to store the data
	float az;
    
	// read whoami
    unsigned char who;
    who = whoami();
 
	// if whoami is not 0x68, stuck in loop with LEDs on
    if (who != 0x68){
        while(1){
            blink(2, 1000);
        }
    }
    
    //Real code now
    
    while (1){
        _CP0_SET_COUNT(0); //reset count for FPS calcs
        blink(1, 5); //blink heartbeat
        burst_read_mpu6050(d); // read IMU
        az = conv_zXL(d); // convert data
        
        // print out the data
        sprintf(toWrite, "az: %f", az);
        drawMessage(toWrite, 0, 0);
        
        //calculate FPS
        t2 = 24000000 /_CP0_GET_COUNT();
        
        //Print FPS
        sprintf(toWrite, "fps: %f", t2);
        drawMessage(toWrite, 0, 24);  
        
        //Update screen
        ssd1306_update();
    }
}

// blink the LEDs
void blink(int iterations, int time_ms) {
    int i;
    unsigned int t;
    for (i = 0; i < iterations; i++) {
        NU32DIP_GREEN = 0; // on
        NU32DIP_YELLOW = 1; // off
        t = _CP0_GET_COUNT(); // should really check for overflow here
        // the core timer ticks at half the SYSCLK, so 24000000 times per second
        // so each millisecond is 24000 ticks
        // wait half in each delay
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }

        NU32DIP_GREEN = 1; // off
        NU32DIP_YELLOW = 0; // on
        t = _CP0_GET_COUNT(); // should really check for overflow here
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }
    }
}

