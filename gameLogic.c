#include <stdio.h>
#include "configGIC.h"
#include "address_map_arm.h"

void gameLogic(int gameBoard[5][5]);
extern char keyPressed;
extern char color;

//create a 5x5 array
//array stores the start and end position of each color as an uppercase letter 
	//ie: start for red is 'R', end for red is 'R'
//array also stores direction of movement as lowercase letters
	//ie: moving for color red is stored as 'r'
	
void gameLogic(int gameBoard[5][5]) {
	const int redStartIndex = 0;
	const int redEndIndex = 0;
	
	const int greenStartIndex = 0;
	const int greenEndIndex = 0;
	
	const int blueStartIndex = 0;
	const int blueEndIndex = 0;
	
	const int yellowStartIndex = 0;
	const int yellowEndIndex = 0;
	
	const int orangeStartIndex = 0;
	const int orangeEndIndex = 0;
	
	int moveXRed;
	int moveYRed;
	
	int moveXGreen
	int moveYGreen;
	
	int moveXBlue;
	int moveYBlue;
	
	int moveXYellow;
	int moveYYellow;
	
	int moveXOrange;
	int moveYOrange;
	
	bool xInBounds = true;
	bool yInBounds = true;
	
	bool gameOver = false;
	
	/*
	- game over condition: gameBoard[colorXPosition][colorYPosition] == R/G/B/Y/O
	- otherwise, game isn't over
	- 
	*/
	
	while(!gameOver) {
		
	}
}