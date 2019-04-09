#include "configGIC.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

char keyPressed;
int color_select;

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

/* setup the PS2 interrupts in the FPGA */
void config_PS2s() {
	volatile int* PS2_ptr = (int*)0xFF200100; // PS2 base address
	volatile int* PS2_ptr_interrupt = (int*)0xFF200104;
	*(PS2_ptr_interrupt) = 0x1; // enable interrupts for PS/2 by writing 1 to RE field at address 0xFF200104
}

// use global variables.
void PS2_ISR() { //determine which button on the keyboard was pressed: W,A,S,D or other, and display on HEX
	//clear the interrupt
	volatile int* PS2_ptr_interrupt = (int*)0xFF200104;
	*(PS2_ptr_interrupt) = 0b100000001;

	/* PS2 base address */
	volatile int *PS2_ptr = (int *) 0xFF200100;

	//HEX display base address
	volatile int *RLEDs = (int *) 0xFF200000;

	int PS2_data, RVALID, letter, LED;
	//const int W = 0x1D, A = 0x1C, S = 0x1B, D = 0x23;

	PS2_data = *(PS2_ptr);
	RVALID = PS2_data & 0x8000;
	if(RVALID) {
		letter = PS2_data & 0xFF;
		if(letter == 0x1D) {
			//LED = 0x1D;
			keyPressed = 'W';
            //numPressedW++; // still need to reset this later. I dont remeber if I need it or not...
		} else if(letter == 0x1C) {
			//LED = 0x1C;
			keyPressed = 'A';
            //numPressedA++;
		} else if(letter == 0x1B) {
			//LED = 0x1B;
			keyPressed = 'S';
            //numPressedS++;
		} else if(letter == 0x23) {
			//LED = 0x23;
			keyPressed = 'D';
            //numPressedD++;
		} else if (letter == 0x16) {
            // "1"
            color_select = 1;
			
        } else if (letter == 0x1E) {
            // "2"
            color_select = 2;
        } else if (letter == 0x26) {
            // "3"
            color_select = 3;
        } else if (letter == 0x25) {
            // "4"
            color_select = 4;
        } else if (letter == 0x2E) {
            // "5"
            color_select = 5;
        } else {
			LED = 0;
			keyPressed = 'Z';
		}
		
		LED = letter;
	}

	*RLEDs = LED; //display the letter on the HEX

	printf("%c\n", keyPressed);
	return;
}

// Define the IRQ exception handler
void __attribute__ ((interrupt)) __cs3_isr_irq () {
	int interrupt_ID = *((int *) 0xFFFEC10C); //Read the ICCIAR from the CPU Interface in the GIC
	if (interrupt_ID == 79) // check if interrupt is from the PS/2
		PS2_ISR();
	else
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