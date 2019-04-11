volatile int pixel_buffer_start; // global variable
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "configGIC.h" // comment this out for CPULATOR
#include "address_map_arm.h"
#include "GameOver.h"

#define ROWS 5
#define COLS 5

// defining the colors
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define ORANGE 5

//define u16 code for the colors
#define RED_U16 0xF800
#define GREEN_U16 0x7E0
#define BLUE_U16 0x1F
#define YELLOW_U16 0xFFE0
#define ORANGE_U16 0xFBE0
#define BLACK_U16 0x0000
#define WHITE_U16 0xFFFF



// graphical functions
void clear_screen();
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int x1, int y0, int y1, short int color);
void swap(int* a, int*b); // you gotta use pointers
void fill_color(int x, int y, short int color);
void initializeBoard (int board[][COLS]);
void animate_line(int boardX, int boardY, int direction, short int line_color, short int color, int board[][COLS]);
void animate (int x, int y, short int color, int board[][COLS]);
void draw_piece(int piecex, int piecey, int currDirection, short int color);

int xpos_global, ypos_global;
short int color_global;
int pieceRadius = 16;

// Interrupts
int numPressedW;
int numPressedA;
int numPressedS;
int numPressedD;
void enable_A9_interrupts();
void disable_A9_interrupts();
void config_GIC();
void config_interrupt (int N, int CPU_target);
void config_PS2s();
void PS2_ISR();
void __attribute__ ((interrupt)) __cs3_isr_irq ();
void __attribute__ ((interrupt)) __cs3_reset ();
void __attribute__ ((interrupt)) __cs3_isr_undef ();
void __attribute__ ((interrupt)) __cs3_isr_swi ();
void __attribute__ ((interrupt)) __cs3_isr_pabort ();
void __attribute__ ((interrupt)) __cs3_isr_dabort ();
void __attribute__ ((interrupt)) __cs3_isr_fiq ();
void set_A9_IRQ_stack();


volatile bool reset = false;
volatile bool pressedAgain = false;
volatile char byte1, byte2, data;
volatile char keyPressed;
volatile int color_select = 0;

bool keyRed = false;
bool keyGreen = false;
bool keyBlue = false;
bool keyYellow = false;
bool keyOrange = false;

//current position
int currX = 1000;
int currY = 1000;
int currDirection = 1000;

//game legality
bool isLegalMove(int color_select, char keyPressed);
bool checkGameOver();
void draw_game_over();

//game board
char gameBoard[5][5];
int board [5][5];

//global variables for keeping track of game status
bool gameOver = false;
bool redPathFound = false;
bool greenPathFound = false;
bool bluePathFound = false;
bool yellowPathFound = false;
bool orangePathFound = false;

//global variables for keeping track of position - initialized to the starting positions
int redCurrentX = 0;
int redCurrentY = 0;

int greenCurrentX = 2;
int greenCurrentY = 0;

int blueCurrentX = 2;
int blueCurrentY = 4;

int yellowCurrentX = 4;
int yellowCurrentY = 0;

int orangeCurrentX = 3;
int orangeCurrentY = 4;

//reset functions
void resetGame(bool reset, int board[][COLS]);


void initializeBoard (int board[][COLS]) {
// 1 for red, 2 for green, 3 for blue, 4 for yellow, 5 for orange
    //initialize everything to 0 first
    int i = 0;
    int j = 0;
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            board [i][j] = 0;
        }
    }

    // hardcoding the positions
    // RED
    board [0][0] = 1;
    board [1][4] = 1;
    // GREEN
    board [2][0] = 2;
    board [1][3] = 2;
    // BLUE
    board [2][1] = 3;
    board [2][4] = 3;
    // YELLOW
    board [4][0] = 4;
    board [3][3] = 4;
    // ORANGE
    board [4][1] = 5;
    board [3][4] = 5;

	//initializing the gameBoard variable
	int x = 0, y = 0;
	for(x = 0; x < 5; x++) {
		for(y = 0; y < 5; y++) {
			if(x == 0 && y == 0) { //red start
				gameBoard[x][y] = 'r';
			} else if(y == 1 && x == 4) { //red end
				gameBoard[x][y] = 'R';
			} else if(y == 2 && x == 0) { //green start
				gameBoard[x][y] = 'g';
			} else if(y == 1 && x == 3) { //green end
				gameBoard[x][y] = 'G';
			} else if(y == 2 && x == 4) { //blue start
				gameBoard[x][y] = 'b';
			} else if(y == 2 && x == 1) { //blue end
				gameBoard[x][y] = 'B';
			} else if(y == 4 && x == 0) { //yellow start
				gameBoard[x][y] = 'y';
			} else if(y == 3 && x == 3) { //yellow end
				gameBoard[x][y] = 'Y';
			} else if(y == 3 && x == 4) { //orange start
				gameBoard[x][y] = 'o';
			} else if(y == 4 && x == 1) { //orange end
				gameBoard[x][y] = 'O';
			} else { //all other non-occupied places
				gameBoard[x][y] = '0';
			}
		}
	}

}

int main(void) {

    byte1        = 0;
    byte2        = 0;
    data         = 0; // used to hold PS/2 data
    keyPressed   = "U";

    int x;
    int y;

	//function calls to enable interrupts in ARM and PS/2 keyboard
	disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_PS2s(); // configure PS/2 to generate interrupts
	config_KEYs(); // configure PS/2 to generate interrupts

	enable_A9_interrupts(); // enable interrupts in the A9 processor

	//while(1) {}

	volatile int * pixel_ctrl_ptr = (int *)0xFF203020; // pixel buffer address.
    /* Read location of the pixel buffer from the pixel buffer controller */
    //pixel_buffer_start = *pixel_ctrl_ptr;
    int N = 4; // have N declared objects
    // board of arrays:

    initializeBoard(board);

    // colors to choose from:
    //short int color[7] = {RED_U16, GREEN_U16, BLUE_U16, YELLOW_U16, ORANGE_U16, BLACK_U16, WHITE_U16};
    //  xpos    ypos     dx incre   dy incre  color
    //int x_pos[N], y_pos[N], dx[N], dy[N], color_display[7];

    // we may not need this part of the code.
    //int fill_x [5] = {0, 64, 128, 192, 256};
    //int fill_y [5] = {0, 48, 96, 144, 192};
    // till here

    // int i = 0;
    // for (i = 0; i < N; i++) {
    //       dx[i] = rand() %2 *2 - 1; // set to 1 or -1
    //       dy[i] = rand() %2 *2 - 1; // set to 1  or -1
    //       //color_display[i] = color[rand()%10]; // array of 10 colors
    //       x_pos[i] = rand() % 319; // set to a random x coordinate
    //       y_pos[i] = rand() % 239; // set to a random y coordinate
    // }

    //

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    //clear_screen();
    //coloring a square (testing code for now)
	while(1) {
		while (!checkGameOver()) {
		clear_screen();
		int row = 0;
		int col = 0;
		int direction = 0; // 1: UP, 2: LEFT, 3: DOWN, 4: RIGHT

		//this doesn't want to work:
		for (row = 0; row < ROWS; row++) {
			for (col = 0; col < COLS; col++) {
				if (board[row][col] == RED) {
					// printf("Color: Red \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, RED_U16);
				}

				else if (board[row][col] == GREEN) {
					// printf("Color: Green \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, GREEN_U16);
				}

				else if (board[row][col] == BLUE) {
					// printf("Color: Blue \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, BLUE_U16);
				}

				else if (board[row][col] == YELLOW) {
					// printf("Color: Yellow \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, YELLOW_U16);
				}

				else if (board[row][col] == ORANGE) {
					// printf("Color: Orange \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, ORANGE_U16);
				}

				// printf("value: ");
				// printf("%d", board[row][col]);
				// printf("\n");
			}
		 }
			//fill_color(0, 2, GREEN_U16);

			// Background
			// Horizontal Line

			// UNCOMMENT THE IF STATMENT TO TEST THIS
			draw_line(64, 64, 0, 239, WHITE_U16);
			draw_line(128, 128, 0, 239, WHITE_U16);
			draw_line(192, 192, 0, 239, WHITE_U16);
			draw_line(256, 256, 0, 239, WHITE_U16);
			//Vertical Line
			draw_line(0, 319, 48, 48, WHITE_U16);
			draw_line(0, 319, 96, 96, WHITE_U16);
			draw_line(0, 319, 144, 144, WHITE_U16);
			draw_line(0, 319, 192, 192, WHITE_U16);

			 // if (keyPressed == 'W') {
			 //        //board[0][4] = RED;
			 //     // volatile int* RLEDs = (int*) 0xFF200000;
			 //     // int LED;
			 //     // LED = 0xFFFF;
			 //     // *RLEDs = LED;
			 // }


			// first select the color
			if (keyPressed == '1') {
				keyRed = true;
			}

			else if (keyPressed == '2') {
				keyGreen = true;
			}

			else if (keyPressed == '3') {
				keyBlue = true;
			}

			else if (keyPressed == '4') {
				keyYellow = true;
			}

			else if (keyPressed == '5') {
				keyOrange = true;
			}
			// actual key presses
			if (keyRed && (keyPressed == 'W' || keyPressed == 'A' || keyPressed == 'S' || keyPressed == 'D')) {
				// turn off all other flags
				keyGreen = false;
				keyBlue = false;
				keyYellow = false;
				keyOrange = false;

				// HARDCODED FOR NOW:
				x = redCurrentX;
				y = redCurrentY;

				currX = x;
				currY = y;

				// animate the line:
				if (keyPressed == 'W') {
					direction  = 1;
					animate_line(x, y, direction, RED_U16, RED, board);
					keyRed = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'A') {
					direction  = 2;
					animate_line(x, y, direction, RED_U16, RED, board);
					keyRed = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'S') {
					direction  = 3;
					animate_line(x, y, direction, RED_U16, RED, board);
					keyRed = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'D') {
					direction  = 4;
					animate_line(x, y, direction, RED_U16, RED, board);
					keyRed = false; // turn off the
					currDirection = direction; // this is for the drawing of a square
				}

	//            draw_piece(x, y, WHITE_U16);

			} else if (keyGreen && (keyPressed == 'W' || keyPressed == 'A' || keyPressed == 'S' || keyPressed == 'D')) {

				keyRed = false;
				keyBlue = false;
				keyYellow = false;
				keyOrange = false;

				x =  greenCurrentX;
				y =  greenCurrentY;

				// for drawing the circles
				currX = x;
				currY = y;

				//anime the lines
				// animate the line:
				if (keyPressed == 'W') {
					direction  = 1;
					animate_line(x, y, direction, GREEN_U16, GREEN, board);
					keyGreen = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'A') {
					direction  = 2;
					animate_line(x, y, direction, GREEN_U16, GREEN, board);
					keyGreen = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'S') {
					direction  = 3;
					animate_line(x, y, direction, GREEN_U16, GREEN, board);
					keyGreen = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'D') {
					direction  = 4;
					animate_line(x, y, direction, GREEN_U16, GREEN, board);
					keyGreen = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				}

	//         draw_piece(x, y, WHITE_U16);

			} else if (keyBlue && (keyPressed == 'W' || keyPressed == 'A' || keyPressed == 'S' || keyPressed == 'D')) {

				keyRed = false;
				keyGreen = false;
				keyYellow = false;
				keyOrange = false;

				x = blueCurrentX;
				y = blueCurrentY;

				// for drawing the circles
				currX = x;
				currY = y;

				// animate the line:
				if (keyPressed == 'W') {
					direction  = 1;
					animate_line(x, y, direction, BLUE_U16, BLUE, board);
					keyBlue = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'A') {
					direction  = 2;
					animate_line(x, y, direction, BLUE_U16, BLUE, board);
					keyBlue = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'S') {
					direction  = 3;
					animate_line(x, y, direction, BLUE_U16, BLUE, board);
					keyBlue = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'D') {
					direction  = 4;
					animate_line(x, y, direction, BLUE_U16, BLUE, board);
					keyBlue = false; // turn off the flag
					currDirection = direction; // this is for the drawing of a square
				}

	//            draw_piece(x, y, WHITE_U16);

			} else if (keyYellow && (keyPressed == 'W' || keyPressed == 'A' || keyPressed == 'S' || keyPressed == 'D')) {

				keyRed = false;
				keyGreen = false;
				keyBlue = false;
				keyOrange = false;

				x = yellowCurrentX;
				y = yellowCurrentY;

				// for drawing the circles
				currX = x;
				currY = y;

				// animate the line:
				if (keyPressed == 'W') {
					direction  = 1;
					animate_line(x, y, direction, YELLOW_U16, YELLOW, board);
					keyYellow = false;
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'A') {
					direction  = 2;
					animate_line(x, y, direction,  YELLOW_U16, YELLOW, board);
					keyYellow = false;
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'S') {
					direction  = 3;
					animate_line(x, y, direction,  YELLOW_U16, YELLOW,board);
					keyYellow = false;
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'D') {
					direction  = 4;
					animate_line(x, y, direction,  YELLOW_U16,YELLOW, board);
					keyYellow = false;
					currDirection = direction; // this is for the drawing of a square
				}

	//            draw_piece(x, y, WHITE_U16);

			} else if (keyOrange && (keyPressed == 'W' || keyPressed == 'A' || keyPressed == 'S' || keyPressed == 'D')) {

				keyRed = false;
				keyGreen = false;
				keyBlue = false;
				keyYellow = false;

				x = orangeCurrentX;
				y = orangeCurrentY;

				// for drawing the circles
				currX = x;
				currY = y;

				// animate the line:
				if (keyPressed == 'W') {
					direction  = 1;
					animate_line(x, y, direction, ORANGE_U16, ORANGE, board);
					keyOrange = false;
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'A') {
					direction  = 2;
					animate_line(x, y, direction, ORANGE_U16, ORANGE, board);
					keyOrange = false;
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'S') {
					direction  = 3;
					animate_line(x, y, direction, ORANGE_U16, ORANGE, board);
					keyOrange = false;
					currDirection = direction; // this is for the drawing of a square
				} else if (keyPressed == 'D') {
					direction  = 4;
					animate_line(x, y, direction, ORANGE_U16, ORANGE, board);
					keyOrange = false;
					currDirection = direction; // this is for the drawing of a square
				}

			}

		   draw_piece(currX, currY, currDirection, WHITE_U16);
		   wait_for_vsync(); // swap front and back buffers on VGA vertical sync
		   pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
		}

		while(!reset) {
			if(checkGameOver()) {
				draw_game_over();
				wait_for_vsync(); // swap front and back buffers on VGA vertical sync
				pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
				clear_screen();

			}
		}
		resetGame(reset, board);
	}


    return 0;
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void draw_piece(int piecex, int piecey, int currDirection, short int color) {
    int actualx = piecex * 64 + 23;
    int actualy = piecey * 48 + 16;
    int x = 0;
    int y = 0;
    for (x = 0; x <= 16; x++) {
        for (y = 0; y <= 16; y++) {
            if (currDirection == 1) { //UP
                plot_pixel(actualx + x, actualy + y - 48, color);
            } else if (currDirection == 2) { // LEFT
                plot_pixel(actualx + x - 64, actualy + y, color);
            } else if (currDirection == 3) { // DOWN
                plot_pixel(actualx + x, actualy + y + 48, color);
            } else if (currDirection == 4) { // RIGHT
                plot_pixel(actualx + x + 64, actualy + y, color);
            }
        }
    }
}

// code not shown for clear_screen() and draw_line() subroutines
// takes in argument for the box position on the board, the WASD direction and the U16 color.
void animate_line(int boardX, int boardY, int direction, short int line_color, short int color, int board[][COLS]) {

    //convert board position onto x,y coordinates on the screen
    int startX = boardX * 64;
    int startY = boardY * 48;

    int endXRIGHT = startX + 64;
    int endYUP = startY + 48;
    int endXLEFT = startX - 64;
    int endYDOWN = startY - 48;

    int x0 = 0;  // starting x
    int x1 = 0;  // ending x
    int y0 = 0;  // starting y
    int y1 = 0;  // ending y

	bool isLegal;

    if(gameOver) {
        draw_game_over();
    } else if (direction == 1) {
        isLegal = isLegalMove(color_select, keyPressed);
		if(isLegal) {
			volatile int* RLEDs = (int*) 0xFF200000;
			int LED = 0xFFFF;
			*RLEDs = LED;
			//printf("legal move"); //testing
			x0 = startX;
			x1 = endXRIGHT;
			y0 = startY;
			y1 = startY;
			for (startY = boardY * 48; startY < endYUP; startY++) {
				// Horizontal Line
				y0 -= direction;
				y1 -= direction;
				draw_line(x0, x1, y0, y1, line_color);
				wait_for_vsync();
				//draw_line(x0, x1, y0, y1, 0x0000);
			}
			//fill_color(boardX, boardY, line_color);
			if (boardX <= 5 && boardY <= 5 && boardX >= 0 && boardY >= 0) {
				board[boardX][boardY - 1] = color;
			}
		}

    }
    // A LEFT
    else if (direction == 2) {
		isLegal = isLegalMove(color_select, keyPressed);

		if(isLegal) {
			volatile int* RLEDs = (int*) 0xFF200000;
			int LED = 0xFFFF;
			*RLEDs = LED;
			//printf("legal move"); //testing
			x0 = startX;
			x1 = startX;
			y0 = startY;
			y1 = endYUP; // needsd checking
			for (startX = boardX * 64; startX > endXLEFT; startX--) {
				x0 -= direction;
				x1 -= direction;
				draw_line(x0, x1, y0, y1, line_color);
				wait_for_vsync();
				//draw_line(x0, x1, y0, y1, 0x0000);
			}
			if (boardX <= 5 && boardY <= 5 && boardX >= 0 && boardY >= 0) {
				board[boardX - 1][boardY] = color;
			}
		}


    }
    // S DOWNWARDS
    else if (direction == 3) {
		isLegal = isLegalMove(color_select, keyPressed);

		if(isLegal) {
			volatile int* RLEDs = (int*) 0xFF200000;
			int LED = 0xFFFF;
			*RLEDs = LED;
			//printf("legal move"); //testing
			x0 = startX;
			x1 = endXRIGHT;
			y0 = startY;
			y1 = startY;
			for (startY = boardY * 48; startY > endYDOWN; startY--) {
				y0 += direction;
				y1 += direction;
				draw_line(x0, x1, y0, y1, line_color);
				wait_for_vsync();
				//draw_line(x0, x1, y0, y1, 0x0000);
			}
			if (boardX <= 5 && boardY <= 5 && boardX >= 0 && boardY >= 0) {
				board[boardX][boardY + 1] = color;
			}
		}
    }
    // D RIGHTWARDS
    else if (direction == 4) {
		isLegal = isLegalMove(color_select, keyPressed);

		if(isLegal) {
			volatile int* RLEDs = (int*) 0xFF200000;
			int LED = 0xFFFF;
			*RLEDs = LED;
			//printf("legal move"); //testing
			x0 = startX;
			x1 = x0;
			y0 = startY;
			y1 = endYUP; // needsd checking
			for (startX = boardX * 64; startX < endXRIGHT; startX++) {
				x0 += direction;
				x1 += direction;
				draw_line(x0, x1, y0, y1, line_color);
				wait_for_vsync();
				//draw_line(x0, x1, y0, y1, 0x0000);
			}
			if (boardX <= 5 && boardY <= 5 && boardX >= 0 && boardY >= 0) {
				board[boardX + 1][boardY] = color;
			}
		}
    }
}


void fill_color(int x, int y, short int color) {
//printf("position (x,y): " );
     int xpos =  x * 64;
     int ypos =  y * 48;

    // //printf("position (x,y): ");
     // printf("%d", xpos);
    //  //printf("%d", ypos);
    // //printf("\n");
    //
    int tempX = xpos;
    int tempY = ypos;
    //
    for (xpos = x * 64; xpos < (tempX + 64); xpos++) {
        for (ypos = y * 48; ypos < (tempY + 48); ypos++) {
            plot_pixel(xpos, ypos, color);
        }
    }

}

// v_sync
void wait_for_vsync() {

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    //step 1: write a 1 to the front buffer
    *pixel_ctrl_ptr = 1;

    //setp 2: poll the status, + 3 means 3 bytes.
    register int status = *(pixel_ctrl_ptr + 3);

    //step  3: check conditions
    //while status is 1, status is equal to whatever the S bit is.
    // if status is 0, this means the swap happens and it breaks out of the loop
    while((status & 0x01) != 0) {
		status = *(pixel_ctrl_ptr + 3);
	}
}

//ISS C
void clear_screen () {
    int x, y;
    for (x = 0; x < 320; x++) {
        for (y = 0; y < 240; y++) {
            plot_pixel(x, y, 0x0000);
        }
    }
}

void swap (int* a, int* b) {
    int temp;
    temp = *b;
    *b = *a;
    *a = temp;
}
//alogrithm
void draw_line(int x0, int x1, int y0, int y1, short int line_color)
{
    bool is_steep  = abs(y1 - y0) > abs(x1 - x0);

// might be here:
    if (is_steep) {
        swap(&x0, &y0);
        swap(&x1, &y1);
    }

    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = -(deltax / 2);
    int y = y0;
    int y_step;

    if (y0 < y1) {
        y_step = 1;
    } else {
        y_step = -1;
    }

    int x = x0;
    for (x = x0; x < x1 + 1; x++) {
        if (is_steep) {
            plot_pixel(y, x, line_color);
        } else {
            plot_pixel(x, y, line_color);
        }

        error = error + deltay;

        if (error >= 0) {
            y = y + y_step;
            error = error - deltax;
        }
    }
}


// CODE BELOW ARE INTERUPT RELATED:
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
		reset = pressedAgain;

		if(reset) {
			resetGame(reset, board);
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

// use global variables.

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

		//if(byte1 != data) {
			if(data == 0x1D) {
				//LED = 0x1D;
				keyPressed = 'W';

				//printf("W pressed\n");
			} else if(data == 0x1C) {
				LED = 0x1C;

                //Vertical Line

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
				color_select = RED;
				//color = 'R';
				//printf("1 pressed\n");
			} else if(data == 0x1E) {
				LED = 0x1E;
				//color = 'G';
				keyPressed = '2';
				color_select =  GREEN;
				//printf("2 pressed\n");
			} else if(data == 0x26) {
				LED = 0x26;
				//color = 'B';
				keyPressed = '3';
				color_select = BLUE;
				//printf("3 pressed\n");
			} else if(data == 0x25) {
				LED = 0x25;
				//color = 'Y';
				keyPressed = '4';
				color_select = YELLOW;
				//printf("4 pressed\n");
			} else if(data == 0x2E) {
				LED = 0x2E;
				//color = 'O';
				keyPressed = '5';
				color_select = ORANGE;
				//printf("5 pressed\n");

			//error handling
			} else {
				LED = 0xFFFF;
				//printf("unknown key pressed\n");
				keyPressed = '?';
				//color = 'B';
				color_select = 10;
			}
		//}

	}

	//printf("data: %c\n", data);
	//printf("%c key pressed\n", keyPressed);
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

bool isLegalMove(int color_select, char keyPressed) {
	//initialize gameBoard:
	//lowercase represents start, uppercase represents end
	//1 represents occupied, 0 represents open

	//testing
	/*
	for(x = 0; x < 5; x++) {
		for(int y = 0; y < 5; y++) {
			printf("%c ", gameBoard[x][y]);
		}
		printf("\n");
	}*/

	//bool gameOver = false;

	//while(!gameOver) {
		if(color_select == RED) {
			if(keyPressed == 'W') {
				int tempY = redCurrentY - 1;

				if(tempY < 0 || tempY > 4 || gameBoard[tempY][redCurrentX] != '0' || redPathFound) {
					redCurrentY = redCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][redCurrentX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentY = tempY; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied
					}

					return true;
				}

			} else if(keyPressed == 'A') {
				int tempX = redCurrentX - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[redCurrentY][tempX] != '0' || redPathFound) {
					redCurrentX = redCurrentX;
					return false;
				} else {
					if(gameBoard[redCurrentY][tempX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentX = tempX; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied
					}
					return true;
				}

			} else if(keyPressed == 'S') {
				int tempY = redCurrentY + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][redCurrentX] != '0' || redPathFound) {
					redCurrentY = redCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][redCurrentX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentY = tempY; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'D') {
				int tempX = redCurrentX + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[redCurrentY][tempX] != '0' || redPathFound) {
					redCurrentX = redCurrentX;
					return false;
				} else {
					if(gameBoard[redCurrentY][tempX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentX = tempX; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else {
				redCurrentX = redCurrentX;
				redCurrentY = redCurrentY;
				return false;
			}

		} else if(color_select == GREEN) {
			if(keyPressed == 'W') {
				int tempY = greenCurrentY - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][greenCurrentX] != '0' || greenPathFound) {
					greenCurrentY = greenCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][greenCurrentX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentY = tempY; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'A') {
				int tempX = greenCurrentX - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[greenCurrentY][tempX] != '0' || greenPathFound) {
					greenCurrentX = greenCurrentX;
					return false;
				} else {
					if(gameBoard[greenCurrentY][tempX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentX = tempX; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'S') {
				int tempY = greenCurrentY + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][greenCurrentX] != '0' || greenPathFound) {
					greenCurrentY = greenCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][greenCurrentX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentY = tempY; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'D') {
				int tempX = greenCurrentX + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[greenCurrentY][tempX] != '0' || greenPathFound) {
					greenCurrentX = greenCurrentX;
					return false;
				} else {
					if(gameBoard[greenCurrentY][tempX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentX = tempX; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else {
				greenCurrentX = greenCurrentX;
				greenCurrentY = greenCurrentY;
				return false;
			}

		} else if(color_select == BLUE) {

			if(keyPressed == 'W') {
				int tempY = blueCurrentY - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][blueCurrentX] != '0' || bluePathFound) {
					blueCurrentY = blueCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][blueCurrentX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentY = tempY; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'A') {
				int tempX = blueCurrentX - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[blueCurrentY][tempX] != '0' || bluePathFound) {
					blueCurrentX = blueCurrentX;
					return false;
				} else {
					if(gameBoard[blueCurrentY][tempX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentX = tempX; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'S') {
				int tempY = blueCurrentY + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][blueCurrentX] != '0' || bluePathFound) {
					blueCurrentY = blueCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][blueCurrentX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentY = tempY; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'D') {
				int tempX = blueCurrentX + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[blueCurrentY][tempX] != '0' || bluePathFound) {
					blueCurrentX = blueCurrentX;
					return false;
				} else {
					if(gameBoard[tempX][blueCurrentX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentX = tempX; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else {
				blueCurrentX = blueCurrentX;
				blueCurrentY = blueCurrentY;
				return false;
			}

		} else if(color_select == YELLOW) {

			if(keyPressed == 'W') {
				int tempY = yellowCurrentY - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][yellowCurrentX] != '0' || yellowPathFound) {
					yellowCurrentY = yellowCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][yellowCurrentX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentY = tempY; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'A') {
				int tempX = yellowCurrentX - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[yellowCurrentY][tempX] != '0' || yellowPathFound) {
					yellowCurrentX = yellowCurrentX;
					return false;
				} else {
					if(gameBoard[yellowCurrentY][tempX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentX = tempX; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'S') {
				int tempY = yellowCurrentY + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][yellowCurrentX] != '0' || yellowPathFound) {
					yellowCurrentY = yellowCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][yellowCurrentX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentY = tempY; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'D') {
				int tempX = yellowCurrentX + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[yellowCurrentY][tempX] != '0' || yellowPathFound) {
					yellowCurrentX = yellowCurrentX;
					return false;
				} else {
					if(gameBoard[tempX][yellowCurrentX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentX = tempX; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else {
				yellowCurrentX = yellowCurrentX;
				yellowCurrentY = yellowCurrentY;
				return false;
			}

		} else if(color_select == ORANGE) {

			if(keyPressed == 'W') {
				int tempY = orangeCurrentY - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][orangeCurrentX] != '0' || orangePathFound) {
					orangeCurrentY = orangeCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][orangeCurrentX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentY = tempY; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'A') {
				int tempX = orangeCurrentX - 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[orangeCurrentY][tempX] != '0' || orangePathFound) {
					orangeCurrentX = orangeCurrentX;
					return false;
				} else {
					if(gameBoard[orangeCurrentY][tempX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentX = tempX; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'S') {
				int tempY = orangeCurrentY + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][orangeCurrentX] != '0' || orangePathFound) {
					orangeCurrentY = orangeCurrentY;
					return false;
				} else {
					if(gameBoard[tempY][orangeCurrentX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentY = tempY;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentY = tempY; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else if(keyPressed == 'D') {
				int tempX = orangeCurrentX + 1;

				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[orangeCurrentY][tempX] != '0' || orangePathFound) {
					orangeCurrentX = orangeCurrentX;
					return false;
				} else {
					if(gameBoard[tempX][orangeCurrentX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentX = tempX;
						gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentX = tempX; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied

					}
					return true;
				}

			} else {
				orangeCurrentX = orangeCurrentX;
				orangeCurrentY = orangeCurrentY;
				return false;
			}

		} else {
			return false;
		}

		//game is over if all of these are true
		//gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
	//}
}

void resetGame(bool reset, int board[][COLS]) {
    currX = 10000;
    currY = 10000;
	if(reset) {
		int i = 0;
		int j = 0;
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < COLS; j++) {
				board [i][j] = 0;
			}
		}

		// hardcoding the positions
		// RED
		board [0][0] = 1;
		board [1][4] = 1;
		// GREEN
		board [2][0] = 2;
		board [1][3] = 2;
		// BLUE
		board [2][1] = 3;
		board [2][4] = 3;
		// YELLOW
		board [4][0] = 4;
		board [3][3] = 4;
		// ORANGE
		board [4][1] = 5;
		board [3][4] = 5;

		int row, col;
		//draw original screen
		for (row = 0; row < ROWS; row++) {
			for (col = 0; col < COLS; col++) {
				if (board[row][col] == RED) {
					// printf("Color: Red \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, RED_U16);
				}

				else if (board[row][col] == GREEN) {
					// printf("Color: Green \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, GREEN_U16);
				}

				else if (board[row][col] == BLUE) {
					// printf("Color: Blue \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, BLUE_U16);
				}

				else if (board[row][col] == YELLOW) {
					// printf("Color: Yellow \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, YELLOW_U16);
				}

				else if (board[row][col] == ORANGE) {
					// printf("Color: Orange \n");
					// printf("value: ");
					// printf("%d", board[row][col]);
					// printf("\n");
					// printf("board position: \n");
					// printf("%d ", row);
					// printf("%d \n", col);
					fill_color(row, col, ORANGE_U16);
				}

				// printf("value: ");
				// printf("%d", board[row][col]);
				// printf("\n");
			}
		}

		//reset all global variables
		//global variables for keeping track of game status
		gameOver = false;
		redPathFound = false;
		greenPathFound = false;
		bluePathFound = false;
		yellowPathFound = false;
		orangePathFound = false;

		//global variables for keeping track of position - initialized to the starting positions
		redCurrentX = 0;
		redCurrentY = 0;

		greenCurrentX = 2;
		greenCurrentY = 0;

		blueCurrentX = 2;
		blueCurrentY = 4;

		yellowCurrentX = 4;
		yellowCurrentY = 0;

		orangeCurrentX = 3;
		orangeCurrentY = 4;

		//reset gameBoard
		//initializing the gameBoard variable
		int x = 0, y = 0;
		for(x = 0; x < 5; x++) {
			for(y = 0; y < 5; y++) {
				if(x == 0 && y == 0) { //red start
					gameBoard[x][y] = 'r';
				} else if(y == 1 && x == 4) { //red end
					gameBoard[x][y] = 'R';
				} else if(y == 2 && x == 0) { //green start
					gameBoard[x][y] = 'g';
				} else if(y == 1 && x == 3) { //green end
					gameBoard[x][y] = 'G';
				} else if(y == 2 && x == 4) { //blue start
					gameBoard[x][y] = 'b';
				} else if(y == 2 && x == 1) { //blue end
					gameBoard[x][y] = 'B';
				} else if(y == 4 && x == 0) { //yellow start
					gameBoard[x][y] = 'y';
				} else if(y == 3 && x == 3) { //yellow end
					gameBoard[x][y] = 'Y';
				} else if(y == 3 && x == 4) { //orange start
					gameBoard[x][y] = 'o';
				} else if(y == 4 && x == 1) { //orange end
					gameBoard[x][y] = 'O';
				} else { //all other non-occupied places
					gameBoard[x][y] = '0';
				}
			}
		}


	}
}

bool checkGameOver() {
	int x, y;

	for(x = 0; x < 5; x++) {
		for(y = 0; y < 5; y++) {
			if(board[x][y] == 0) {
				return false;
			}
		}
	}
	return true;
}

void draw_game_over() {
    int i, j;
    for (i = 0; i < 240; i++) {
        volatile int j = 0;
        for (j = 0; j < 320; j++) {
            if (GameOver[i * 320 + j] != 0x0000) {
                plot_pixel(j, i, GameOver[i * 320 + j]);
            }
        }
    }
}
