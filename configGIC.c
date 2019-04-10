#include "configGIC.h"
#include <stdio.h>
#include <stdbool.h>

volatile bool resetGame = false;
volatile bool pressedAgain = false;

volatile char byte1, byte2, data;
volatile char keyPressed;

//The following code is adapted from Altera's "Using the ARM Generic Interrupt Controller" document:

//Interrupt ID for PS2 devices is 79
//Address of PS2 devices is 0xFF200100

/* This file:
* 1. defines exception vectors for the A9 processor
* 2. provides code that sets the IRQ mode stack, and that dis/enables
* interrupts
* 3. provides code that initializes the generic interrupt controller
*/
char color;

int main() {
	byte1        = 0;
    byte2        = 0;
    data         = 0; // used to hold PS/2 data

    //function calls to enable interrupts in ARM and PS/2 keyboard
	disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_PS2s(); // configure PS/2 to generate interrupts
	config_KEYs(); // configure PS/2 to generate interrupts

	enable_A9_interrupts(); // enable interrupts in the A9 processor

    while(1) {}

	return 0;
}

/*Turn on interrupts in the ARM processor*/
void enable_A9_interrupts() {
	int status = 0b01010011;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

/*Turn off interrupts in the ARM processor*/
void disable_A9_interrupts() {
	int status = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps]"r"(status));
}

/* Configure the Generic Interrupt Controller (GIC)*/
void config_GIC() {
	config_interrupt(79, 1); //configure the PS2 keyboard parallel port
	config_interrupt(73, 1); //configure KEY interrupts

	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all priorities
	*((int *) 0xFFFEC104) = 0xFFFF;

	// Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
	*((int *) 0xFFFEC100) = 1;

	// Configure the Distributor Control Register (ICDDCR) to send pending interrupts to CPUs
	*((int *) 0xFFFED000) = 1;
}

/* Configure Set Enable Registers (ICDISERn) and Interrupt Processor Target Registers (ICDIPTRn). The default (reset) values are used for other registers in the GIC.*/
void config_interrupt (int N, int CPU_target) {
	int reg_offset, index, value, address;

	/* Configure the Interrupt Set-Enable Registers (ICDISERn).
	* reg_offset = (integer_div(N / 32) * 4
	* value = 1 << (N mod 32) */
	reg_offset = (N >> 3) & 0xFFFFFFFC;
	index = N & 0x1F;
	value = 0x1 << index;
	address = 0xFFFED100 + reg_offset;

	/* Now that we know the register address and value, set the appropriate bit */
	*(int *)address |= value;

	/* Configure the Interrupt Processor Targets Register (ICDIPTRn)
	* reg_offset = integer_div(N / 4) * 4
	* index = N mod 4 */
	reg_offset = (N & 0xFFFFFFFC);
	index = N & 0x3;
	address = 0xFFFED800 + reg_offset + index;

	/* Now that we know the register address and value, write to (only) the appropriate byte */
	*(char *)address = (char) CPU_target;
}

/* setup the KEY interrupts */
void config_KEYs() {
	volatile int * KEY_ptr = (int *) 0xFF200050; // KEY base address
	*(KEY_ptr + 2) = 0xF; // enable interrupts for all four KEYs
}

void KEY_ISR() {
	/* KEY base address */
	volatile int *KEY_ptr = (int *) 0xFF200050;
	/* HEX display base address */
	volatile int *HEX3_HEX0_ptr = (int *) 0xFF200020;
	volatile int* RLEDs = (int*) 0xFF200000;
	int press, HEX_bits, LEDs;
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register

	if(press != 0) { //if KEY is pressed, reset the game
		pressedAgain = !pressedAgain;
		resetGame = pressedAgain;

		if(resetGame) {
			LEDs = 0xFFFF;
			*RLEDs = LEDs;
		} else {
			LEDs = 0x0000;
			*RLEDs = LEDs;
		}

	}

	*(KEY_ptr + 3) = press; // Clear the interrupt
	/* if(*(KEY_ptr + 3) == 0) {
		resetGame = false;

	} */

	if (press & 0x1) // KEY0
		HEX_bits = 0b00111111;
	else if (press & 0x2) // KEY1
		HEX_bits = 0b00000110;
	else if (press & 0x4) // KEY2
		HEX_bits = 0b01011011;
	else // press & 0x8, which is KEY3
		HEX_bits = 0b01001111;

	*HEX3_HEX0_ptr = HEX_bits;


	return;
}

/* setup the PS2 interrupts in the FPGA */
void config_PS2s() {
	volatile int* PS2_ptr = (int*)0xFF200100; // PS2 base address
	volatile int* PS2_ptr_interrupt = (int*)0xFF200104;
	*(PS2_ptr_interrupt) = 0x1; // enable interrupts for PS/2 by writing 1 to RE field at address 0xFF200104
}

 void PS2_ISR() { //determine which button on the keyboard was pressed: W,A,S,D or other, and display on HEX
	//clear the interrupt
	volatile int* PS2_ptr_interrupt = (int*)0xFF200104;
	*(PS2_ptr_interrupt) = 0b100000001;

	/* PS2 base address */
	volatile int *PS2_ptr = (int *) 0xFF200100;

	//HEX display base address
	volatile int *RLEDs = (int *) 0xFF200000;

	int PS2_data, RAVAIL, RVALID, data, LED;
	//const int W = 0x1D, A = 0x1C, S = 0x1B, D = 0x23;

	PS2_data = *(PS2_ptr);
	RAVAIL = (PS2_data & 0xFFFF0000) >> 16;

	if(RAVAIL > 0) {
		byte1 = byte2;
		byte2 = data;
		data = PS2_data & 0xFF;

		//determine the direction of movement (W/A/S/D)

		//if(byte1 == data) {
			if(data == 0x1D) {
				LED = 0x1D;
				keyPressed = 'W';
				//printf("W pressed\n");
			} else if(data == 0x1C) {
				LED = 0x1C;
				keyPressed = 'A';
				//printf("A pressed\n");
			} else if(data == 0x1B) {
				LED = 0x1B;
				keyPressed = 'S';
				//printf("S pressed\n");
			} else if(data == 0x23) {
				LED = 0x23;
				keyPressed = 'D';
				//printf("D pressed\n");

			//determine the color to move (R/G/B/Y/O)
			} else if(data == 0x16) {
				LED = 0x16;
				keyPressed = '1';
				//color = 'R';
				//printf("1 pressed\n");
			} else if(data == 0x1E) {
				LED = 0x1E;
				//color = 'G';
				keyPressed = '2';
				//printf("2 pressed\n");
			} else if(data == 0x26) {
				LED = 0x26;
				//color = 'B';
				keyPressed = '3';
				//printf("3 pressed\n");
			} else if(data == 0x25) {
				LED = 0x25;
				//color = 'Y';
				keyPressed = '4';
				//printf("4 pressed\n");
			} else if(data == 0x2E) {
				LED = 0x2E;
				//color = 'O';
				keyPressed = '5';
				//printf("5 pressed\n");

			//error handling
			} else {
				LED = 0xFFFF;
				//printf("unknown key pressed\n");
				keyPressed = '?';
				//color = 'B';
			}
		//}

	}

	//printf("data: %c\n", data);
	printf("%c key pressed\n", keyPressed);
	*RLEDs = LED; //display the hex code on the LEDs

	//printf("%c\n", keyPressed);
	return;
}

// Define the IRQ exception handler
void __attribute__ ((interrupt)) __cs3_isr_irq () {
	int interrupt_ID = *((int *) 0xFFFEC10C); //Read the ICCIAR from the CPU Interface in the GIC
	if (interrupt_ID == 79) { // check if interrupt is from the PS/2
		PS2_ISR();
	} else if(interrupt_ID == 73) {
		KEY_ISR();
	} else
		while (1); // if unexpected, then stay here

	// Write to the End of Interrupt Register (ICCEOIR)
	*((int *) 0xFFFEC110) = interrupt_ID;
}

// Define the remaining exception handlers
void __attribute__ ((interrupt)) __cs3_reset () {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_undef () {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_swi () {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_pabort () {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_dabort () {
	while(1);
}

void __attribute__ ((interrupt)) __cs3_isr_fiq () {
	while(1);
}

/* Initialize the banked stack pointer register for IRQ mode*/
void set_A9_IRQ_stack() {
	int stack, mode;
	stack = 0xFFFFFFFF - 7; // top of A9 onchip memory, aligned to 8 bytes

	/* change processor to IRQ mode with interrupts disabled */
	mode = 0b11010010;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));

	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r" (stack));

	/* go back to SVC mode before executing subroutine return! */
	mode = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r" (mode));
}
