#include <stdio.h>
#include "address_map_arm.h"

void config_GIC();
void config_interrupt (int N, int CPU_target);
void config_PS2s();
void PS2_ISR (void);
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
void PS2_ISR();
