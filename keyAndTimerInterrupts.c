#include <stdio.h>
#include <stdbool.h>
#include "address_map_arm.h"

void config_KEYs();
void KEY_ISR();
void config_timer();
void timer_ISR();

volatile bool resetGame = false;
volatile int timeElapsed = 0;
volatile bool pressedAgain = false; 

void config_GIC();
void config_interrupt (int N, int CPU_target);

void config_KEYs();
void KEY_ISR (void);

void config_timer();
void timer_ISR (void);

void __attribute__ ((interrupt)) __cs3_isr_irq ();
void __attribute__ ((interrupt)) __cs3_reset ();
void __attribute__ ((interrupt)) __cs3_isr_undef ();
void __attribute__ ((interrupt)) __cs3_isr_swi ();
void __attribute__ ((interrupt)) __cs3_isr_pabort ();
void __attribute__ ((interrupt)) __cs3_isr_dabort ();
void __attribute__ ((interrupt)) __cs3_isr_fiq ();
void disable_A9_interrupts();
void set_A9_IRQ_stack();
void enable_A9_interrupts();

void displayHEX(int num);

int main() {
	
	 //function calls to enable interrupts in ARM and PS/2 keyboard
	disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_KEYs(); // configure PS/2 to generate interrupts
	config_timer();
	enable_A9_interrupts(); // enable interrupts in the A9 processor
	
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
	config_interrupt(72, 1); //configure Private Timer interrupts
	
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

void config_timer() {
    volatile int * MPcore_private_timer_ptr = (int *)0xFFFEC600; // timer base address

    /* set the timer period */
    int counter = 200000000; // private timer runs on 200 MHz clock, 200,000,000/200,000,000 = 1 second
    *(MPcore_private_timer_ptr) = counter; // write to timer load register

    /* write to control register to start timer, with interrupts */
    *(MPcore_private_timer_ptr + 2) = 0x7; // int mask = 1, mode = 1, enable = 1
}

void timer_ISR() {
	timeElapsed++;
	if(timeElapsed < 9999) {
		displayHEX(timeElapsed);
	}
}

void displayHEX(int num) {
	volatile int* HEX_ptr = (int*) 0xFF200020;
	int HEX_bits;
	int allHEXBits = 0;
	
	if(num == 0) {
		allHEXBits = 0b00111111;
		*HEX_ptr = allHEXBits;
		return;
	}
	
	while(num > 0) {
		int numMod = num % 10;
		num = num / 10;
		
		if(numMod == 0) {
			HEX_bits = 0b00111111;
		} else if(numMod == 1) {
			HEX_bits = 0b00000110;
		} else if(numMod == 2) {
			HEX_bits = 0b01011011;
		} else if(numMod == 3) {
			HEX_bits = 0b01001111;
		} else if(numMod == 4) {
			HEX_bits = 0b01100110;
		} else if(numMod == 5) {
			HEX_bits = 0b01101101;
		} else if(numMod == 6) {
			HEX_bits = 0b01111100;
		} else if(numMod == 7) {
			HEX_bits = 0b00000111;
		} else if(numMod == 8) {
			HEX_bits = 0b01111111;
		} else if(numMod == 9) {
			HEX_bits = 0b01100111;
		} else {
			HEX_bits = 0b00000000;
		}
		
		allHEXBits = allHEXBits | HEX_bits;
	}
	
	*HEX_ptr = allHEXBits;
	return;
}

// Define the IRQ exception handler
void __attribute__ ((interrupt)) __cs3_isr_irq () {
	int interrupt_ID = *((int *) 0xFFFEC10C); //Read the ICCIAR from the CPU Interface in the GIC
	/*if (interrupt_ID == 79) { // check if interrupt is from the PS/2
		PS2_ISR();
	} else */if(interrupt_ID == 73) {
		KEY_ISR();
	} else if(interrupt_ID == 72) {
		timer_ISR();
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