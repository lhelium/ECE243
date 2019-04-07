#include <stdio.h>
#include "configGIC.h"
#include "address_map_arm.h"
#define RLEDs ((volatile long *) 0xFF200000)

extern char keyPressed;

int main() {
	
	//function calls to enable interrupts in ARM and PS/2 keyboard
	disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_PS2s(); // configure PS/2 to generate interrupts
	enable_A9_interrupts(); // enable interrupts in the A9 processor
	
	while(1) {}
	
	return 0;
	
	/* unsigned char byte1 = 0;
	unsigned char byte2 = 0;
	unsigned char byte3 = 0;

  	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address

	int PS2_data, RVALID;

	while (1) {
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			//always save the last three bytes received
			byte1 = byte2;
			byte2 = byte3;
			byte3 = PS2_data & 0xFF;
		}
		if ( (byte2 == 0xAA) && (byte3 == 0x00) )
		{
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
		}
		// Display last byte on Red LEDs
		*RLEDs = byte3;
	}  */
}
