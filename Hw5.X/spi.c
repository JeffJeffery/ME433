#include <xc.h>
#include "nu32dip.h" // constants, functions for startup and UART
#include "spi.h"
#include <math.h> //sin function, M_PI constant

// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1
    // Turn of analog pins
    ANSELB = 0;
    // Make an output pin for CS = B7
    TRISBbits.TRISB7 = 0;   // Configure Pin B7 as an output
    LATBbits.LATB7 = 1;      // Set Pin B7 to high
    // Set SDO1 = B8
    RPB8Rbits.RPB8R = 0b0011;
    // Set SDI1
    SDI1Rbits.SDI1R = 0b0010;

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1000; // 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi 
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void main(){
    NU32DIP_Startup();
    initSPI();
    
    
    unsigned char cs = 0;
    unsigned short p = 0;
    unsigned short voltage_out;
    int sinBuf[100];
    
    for(int i=0; i<100; i++){
        float radians = 2.0 * M_PI * i / 50;
        sinBuf[i]= (int)(512*(sin(radians)+1));
    }
    
    int triBuf[100];
    float slope = 1023/50;
    for(int i=0; i<50; i++){
        float sample = i*slope;
        triBuf[i] = (int)(sample);
    }
    for (int i=0; i < 50; i++){
        float sample = 1023 - i*slope;
        triBuf[i + 50] = (int)(sample);
    }

    while (1){
        int i;
        for(i=0; i<100; i++){
        //sin wave, channel 1
            
            //generate 8 bit outputs
                cs=0;
                p = cs<<15;
                p = p | (0b111<<12);
                voltage_out = sinBuf[i];
                p = p | (voltage_out<<2);
            //send upper 8bits
                LATBbits.LATB7 = 0;
                spi_io(p>>8);
            //send lower 8bits
                spi_io(p);
                LATBbits.LATB7 = 1;
            //triangle wave, channel 2
            
            //generate 8 bit outputs
                cs=1;
                p = cs<<15;
                p = p | (0b111<<12);
                voltage_out = triBuf[i];
                p = p | (voltage_out<<2);
            //send upper 8bits
                LATBbits.LATB7 = 0;
                spi_io(p>>8);
            //send lower 8bits
                spi_io(p);
                LATBbits.LATB7 = 1;
                _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() < (24000000 * 0.01))
            {
                
            }
        }
        i=0;
}
}