#define RLEDs ((volatile long *) 0xFF200000)
#include "address_map_arm.h"
void HEX_PS2(char, char, char);

// Movement: WASD 
int main() {
	unsigned char b1 = 0;
	unsigned char b2 = 0;
	unsigned char b3 = 0;

  	volatile int * PS2_ptr = (int *) 0xFF200100;  // PS/2 port address

	int PS2_data, RVALID;

	while (1) {
		PS2_data = *(PS2_ptr);	// read the Data register in the PS/2 port
		RVALID = (PS2_data & 0x8000);	// extract the RVALID field
		if (RVALID != 0)
		{
			/* always save the last three bytes received */
			b1 = b2;
			b2 = b3;
			b3 = PS2_data & 0xFF;
		}
		
		if ( (b2 == 0xAA) && (b3 == 0x00) )
		{
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
		}

		//W
		if (b3 == 0x1D)
		{
			// condition
		        HEX_PS2(b3, b3, b3);
			
		}
		//A
		 if (b3 == 0x1C)
		{
			// condition
			HEX_PS2(b3, b3, b3);
		}
		//S
		if (b3 == 0x1B)
		{
			// condition
			HEX_PS2(b3, b3, b3);			
		}
		//D
		 if (b3 == 0x23)
		{
			// condition
			HEX_PS2(b3, b3, b3);
		}
		// Display last byte on Red LEDs
		*RLEDs = b1;
		
	}
}

void HEX_PS2(char b1, char b2, char b3) {
    volatile int * HEX3_HEX0_ptr = (int *)HEX3_HEX0_BASE;
    volatile int * HEX5_HEX4_ptr = (int *)HEX5_HEX4_BASE;

    /* SEVEN_SEGMENT_DECODE_TABLE gives the on/off settings for all segments in
     * a single 7-seg display in the DE2 Media Computer, for the hex digits 0 -
     * F */
    unsigned char seven_seg_decode_table[] = {
        0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
        0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};
    unsigned char hex_segs[] = {0, 0, 0, 0, 0, 0, 0, 0};
    unsigned int  shift_buffer, nibble;
    unsigned char code;
    int           i;

    shift_buffer = (b1 << 16) | (b2 << 8) | b3;
    for (i = 0; i < 6; ++i) {
        nibble = shift_buffer & 0x0000000F; // character is in rightmost nibble
        code   = seven_seg_decode_table[nibble];
        hex_segs[i]  = code;
        shift_buffer = shift_buffer >> 4;
    }
    /* drive the hex displays */
    *(HEX3_HEX0_ptr) = *(int *)(hex_segs);
    *(HEX5_HEX4_ptr) = *(int *)(hex_segs + 4);
}




