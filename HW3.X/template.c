#include "nu32dip.h" // constants, functions for startup and UART
#include <math.h>

#define MESSAGE_SIZE 100
void blink(int, int); // blink the LEDs function
void sendSin(float *);

int main(void)
{
	float message[MESSAGE_SIZE];
	float stepsize = 2 * M_PI / MESSAGE_SIZE;
	float accumulator = 0;
	for (int i = 0; i < 100; i++)
	{
		message[i] = sin(accumulator);
		accumulator += stepsize;
	}

	NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
	while (1)
	{
		if (!NU32DIP_USER)
		{
			sendSin(message);
		}
	}
}

void sendSin(float *message)
{
	char tempMessage[100];
	for (int i = 0; i < 100; i++)
	{
		sprintf(tempMessage, "%f\r\n", message[i]);
		NU32DIP_WriteUART1(tempMessage);
		_CP0_SET_COUNT(0);
		while (_CP0_GET_COUNT() < (24000000 * 0.01))
		{
		}
	}
}
