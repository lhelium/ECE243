volatile int pixel_buffer_start; // global variable
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

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



void clear_screen();
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int x1, int y0, int y1, short int line_color);
void swap(int* a, int*b); // you gotta use pointers
void fill_color(int x, int y, short int color);
void initializeBoard (int board[][COLS]);

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

}

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; // pixel buffer address.
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
    int N = 4; // have N declared objects
    // board of arrays:
    int board [5][5];
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
    // /* set front pixel buffer to start of FPGA On-chip memory */
    // *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the
    //                                     // back buffer
    // /* now, swap the front/back buffers, to set the front buffer location */
    // wait_for_vsync();
    // /* initialize a pointer to the pixel buffer, used by drawing functions */
    // pixel_buffer_start = *pixel_ctrl_ptr;
    // clear_screen(); // pixel_buffer_start points to the pixel buffer
    //
    // /* set back pixel buffer to start of SDRAM memory */
    // *(pixel_ctrl_ptr + 1) = 0xC0000000;
    // pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen();

    //coloring a square (testing code for now)
    int row = 0;
    int col = 0;
    //this doesn't want to work:
    for (row = 0; row < ROWS; row++) {
        for (col = 0; col < COLS; col++) {
            if (board[row][col] == RED) {
                printf("Color: Red \n");
                printf("value: ");
                printf("%d", board[row][col]);
                printf("\n");
                printf("board position: \n");
                printf("%d ", row);
                printf("%d \n", col);
                fill_color(row, col, RED_U16);
            }

            else if (board[row][col] == GREEN) {
                printf("Color: Green \n");
                printf("value: ");
                printf("%d", board[row][col]);
                printf("\n");
                printf("board position: \n");
                printf("%d ", row);
                printf("%d \n", col);
                fill_color(row, col, GREEN_U16);
            }

            else if (board[row][col] == BLUE) {
                printf("Color: Blue \n");
                printf("value: ");
                printf("%d", board[row][col]);
                printf("\n");
                printf("board position: \n");
                printf("%d ", row);
                printf("%d \n", col);
                fill_color(row, col, BLUE_U16);
            }

            else if (board[row][col] == YELLOW) {
                printf("Color: Yellow \n");
                printf("value: ");
                printf("%d", board[row][col]);
                printf("\n");
                printf("board position: \n");
                printf("%d ", row);
                printf("%d \n", col);
                fill_color(row, col, YELLOW_U16);
            }

            else if (board[row][col] == ORANGE) {
                printf("Color: Orange \n");
                printf("value: ");
                printf("%d", board[row][col]);
                printf("\n");
                printf("board position: \n");
                printf("%d ", row);
                printf("%d \n", col);
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
    draw_line(64, 64, 0, 239, WHITE_U16);
    draw_line(128, 128, 0, 239, WHITE_U16);
    draw_line(192, 192, 0, 239, WHITE_U16);
    draw_line(256, 256, 0, 239, WHITE_U16);
    //Vertical Line
    draw_line(0, 319, 48, 48, WHITE_U16);
    draw_line(0, 319, 96, 96, WHITE_U16);
    draw_line(0, 319, 144, 144, WHITE_U16);
    draw_line(0, 319, 192, 192, WHITE_U16);

    //fill_color(fill_x, fill_y, color_display);
    //plotting squares
    return 0;
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// code not shown for clear_screen() and draw_line() subroutines
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
