#include <stdio.h>

//need assembly file image.s to get image as RBG565:
/*
SYNTAX:
.global MY_IMAGE
MY_IMAGE: .incbin "myimage.rgb" //this is the file path

*/
extern short MYIMAGE [240][320];
void importImage() {
    volatile short* pixelbuf = 0xc8000000;
    int i, j;
    for (i=0; i<240; i++)
        for (j=0; j<320; j++)
			*(pixelbuf + (j<<0) + (i<<9)) = MYIMAGE[i][j];
}