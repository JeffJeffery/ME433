#include <xc.h>
#include "i2c_master_noint.h"
#include "nu32dip.h"

unsigned char write_address = 0b01000000;
unsigned char read_address = 0b01000001;

void setGP7(int input){
    i2c_master_start();
    i2c_master_send(write_address);
    i2c_master_send(0x0A);
    i2c_master_send(input << 7);
    i2c_master_stop();
}


void readGP0(){
    i2c_master_start();
    i2c_master_send(write_address);
    i2c_master_send(0x09);
    i2c_master_restart();
    i2c_master_send(read_address);
    unsigned char r = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    if (r & 0b00000001){
        setGP7(1);
    }
    else{
        setGP7(0);
    }
}

void main(){
    NU32DIP_Startup();
    //Set GP7 to output, rest to input
    i2c_master_setup();
    
    i2c_master_start();
    i2c_master_send(write_address);
    i2c_master_send(0x00);
    i2c_master_send(0b00000001);
    i2c_master_stop();
    
    int currentHeartBeat = 0;
    
    while(1){
        currentHeartBeat = ~currentHeartBeat;
        NU32DIP_GREEN = currentHeartBeat;
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 12000*2000){
            readGP0();
        }
    }
    
    
}
