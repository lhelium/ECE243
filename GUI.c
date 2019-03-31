volatile int pixel_buffer_start; // global variable
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void clear_screen();
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x0, int x1, int y0, int y1, short int line_color);
void swap(int* a, int*b); // you gotta use pointers
void fill_color(int fill_x[], int fill_y[], int color_display[]);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; // pixel buffer address.
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
    int N = 4; // have N declared objects
    // colors to choose from:
    short int color[10] = {0xF800, 0xFFE0, 0xFBE0, 0x79E0, 0x7E00, 0x7FF0, 0xF81F, 0x7800, 0xFFFF, 0x7BE0};
    //  xpos    ypos     dx incre   dy incre  color
    int x_pos[N], y_pos[N], dx[N], dy[N], color_display[N];
    int fill_x [5] = {0, 64, 128, 192, 256};
    int fill_y [5] = {0, 48, 96, 144, 192};

    int i = 0;
    for (i = 0; i < N; i++) {
          dx[i] = rand() %2 *2 - 1; // set to 1 or -1
          dy[i] = rand() %2 *2 - 1; // set to 1  or -1
          color_display[i] = color[rand()%10]; // array of 10 colors
          x_pos[i] = rand() % 319; // set to a random x coordinate
          y_pos[i] = rand() % 239; // set to a random y coordinate
    }

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

    // Background
    // Horizontal Line
    clear_screen();
    draw_line(64, 64, 0, 239, 0xFFFF);
    draw_line(128, 128, 0, 239, 0xFFFF);
    draw_line(192, 192, 0, 239, 0xFFFF);
    draw_line(256, 256, 0, 239, 0xFFFF);
    //Vertical Line
    draw_line(0, 319, 48, 48, 0xFFFF);
    draw_line(0, 319, 96, 96, 0xFFFF);
    draw_line(0, 319, 144, 144, 0xFFFF);
    draw_line(0, 319, 192, 192, 0xFFFF);

    //coloring a square (testing code for now)
    fill_color(fill_x, fill_y, color_display);
    //plotting squares
    return 0;
}

// code not shown for clear_screen() and draw_line() subroutines
void fill_color(int fill_x[], int fill_y[], int color_display[]) {

    int rand_x = 0;
    int rand_y = 0;
    int rand_color = 0;

    int i, j, k;
    for (i = 0; i < 3; i++) {
        rand_x = rand()%5;
        rand_y = rand()%5;
        rand_color = rand()%10;

        for (j = 1; j < 64; j++) {
            for (k = 1; k < 48; k++) {
                plot_pixel(fill_x[rand_x] + j, fill_y[rand_y] + k, color_display[rand_color]);
            }
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

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
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
    for (x; x < x1 + 1; x++) {
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
