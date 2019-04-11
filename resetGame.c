#include <stdio.h>
#include <stdbool.h>
#include <configGIC.h>
//function not done yet

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

void resetGame(bool resetGame) {
	if(resetGame) {
		//draw original screen
		
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
	}
}