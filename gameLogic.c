#include <stdio.h>
#include <stdbool.h>
#include "configGIC.h"
//#include "address_map_arm.h"

#define ROWS 5
#define COLS 5

// defining the colors
#define RED 1
#define GREEN 2
#define BLUE 3
#define YELLOW 4
#define ORANGE 5

extern char keyPressed;
extern char color_select;

//create a 5x5 array
//array stores the start and end position of each color as an uppercase letter 
	//ie: start for red is 'R', end for red is 'R'
//array also stores direction of movement as lowercase letters
	//ie: moving for color red is stored as 'r'
	

void gameLogic(char gameBoard[5][5], int color_select, char keyPressed);

/* int main() { //testing
	char board[5][5];
	int color_select = BLUE;
	char keyPressed = 'W';
	
	gameLogic(board, color_select, keyPressed);
	
	return 0;
} */
	
void gameLogic(char gameBoard[5][5], int color_select, char keyPressed) {
	//initialize gameBoard:
	//lowercase represents start, uppercase represents end
	//1 represents occupied, 0 represents open
	
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
	for(x = 0; x < 5; x++) {
		for(int y = 0; y < 5; y++) {
			printf("%c ", gameBoard[x][y]);
		}
		printf("\n");
	} */
        
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
	
	bool gameOver = false;
	bool redPathFound = false;
	bool greenPathFound = false;
	bool bluePathFound = false;
	bool yellowPathFound = false;
	bool orangePathFound = false;
	
	while(!gameOver) {
		if(color_select == RED) {
			if(keyPressed == 'W') {
				int tempY = redCurrentY - 1;
				
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][redCurrentX] != '0'' || redPathFound) { 
					redCurrentY = redCurrentY;
				} else {
					if(gameBoard[tempY][redCurrentX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentY = tempY; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'A') {
				int tempX = redCurrentX - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[redCurrentY][tempX] != '0'' || redPathFound) { 
					redCurrentX = redCurrentX;
				} else {
					if(gameBoard[redCurrentY][tempX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentX = tempX; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'S') {
				int tempY = redCurrentY + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][redCurrentX] != '0'' || redPathFound) { 
					redCurrentY = redCurrentY;
				} else {
					if(gameBoard[tempY][redCurrentX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentY = tempY; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'D') {
				int tempX = redCurrentX + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[redCurrentY][tempX] != '0'' || redPathFound) { 
					redCurrentX = redCurrentX;
				} else {
					if(gameBoard[redCurrentY][tempX] == 'R') { //reached the end
						redPathFound = true;
						redCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						redCurrentX = tempX; //change Y accordingly
						gameBoard[redCurrentY][redCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else {
				redCurrentX = redCurrentX;
				redCurrentY = redCurrentY;
			}
			
		} else if(color_select == GREEN) {
			if(keyPressed == 'W') {
				int tempY = greenCurrentY - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][greenCurrentX] != '0'' || greenPathFound) { 
					greenCurrentY = greenCurrentY;
				} else {
					if(gameBoard[tempY][greenCurrentX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentY = tempY; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'A') {
				int tempX = greenCurrentX - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[greenCurrentY][tempX] != '0'' || greenPathFound) { 
					greenCurrentX = greenCurrentX;
				} else {
					if(gameBoard[greenCurrentY][tempX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentX = tempX; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'S') {
				int tempY = greenCurrentY + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][greenCurrentX] != '0'' || greenPathFound) { 
					greenCurrentY = greenCurrentY;
				} else {
					if(gameBoard[tempY][greenCurrentX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentY = tempY; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'D') {
				int tempX = greenCurrentX + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[greenCurrentY][tempX] != '0'' || greenPathFound) { 
					greenCurrentX = greenCurrentX;
				} else {
					if(gameBoard[greenCurrentY][tempX] == 'G') { //reached the end
						greenPathFound = true;
						greenCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						greenCurrentX = tempX; //change Y accordingly
						gameBoard[greenCurrentY][greenCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else {
				greenCurrentX = greenCurrentX;
				greenCurrentY = greenCurrentY;
			}
			
		} else if(color_select == BLUE) {
			
			if(keyPressed == 'W') {
				int tempY = blueCurrentY - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][blueCurrentX] != '0'' || bluePathFound) { 
					blueCurrentY = blueCurrentY;
				} else {
					if(gameBoard[tempY][blueCurrentX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentY = tempY; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'A') {
				int tempX = blueCurrentX - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[blueCurrentY][tempX] != '0'' || bluePathFound) { 
					blueCurrentX = blueCurrentX;
				} else {
					if(gameBoard[blueCurrentY][tempX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentX = tempX; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'S') {
				int tempY = blueCurrentY + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][blueCurrentX] != '0'' || bluePathFound) { 
					blueCurrentY = blueCurrentY;
				} else {
					if(gameBoard[tempY][blueCurrentX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentY = tempY; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'D') {
				int tempX = blueCurrentX + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[blueCurrentY][tempX] != '0'' || bluePathFound) { 
					blueCurrentX = blueCurrentX;
				} else {
					if(gameBoard[tempX][blueCurrentX] == 'B') { //reached the end
						bluePathFound = true;
						blueCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						blueCurrentX = tempX; //change Y accordingly
						gameBoard[blueCurrentY][blueCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else {
				blueCurrentX = blueCurrentX;
				blueCurrentY = blueCurrentY;
			}
			
		} else if(color_select == YELLOW) {
			
			if(keyPressed == 'W') {
				int tempY = yellowCurrentY - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][yellowCurrentX] != '0'' || yellowPathFound) { 
					yellowCurrentY = yellowCurrentY;
				} else {
					if(gameBoard[tempY][yellowCurrentX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentY = tempY; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'A') {
				int tempX = yellowCurrentX - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[yellowCurrentY][tempX] != '0'' || yellowPathFound) { 
					yellowCurrentX = yellowCurrentX;
				} else {
					if(gameBoard[yellowCurrentY][tempX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentX = tempX; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'S') {
				int tempY = yellowCurrentY + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][yellowCurrentX] != '0'' || yellowPathFound) { 
					yellowCurrentY = yellowCurrentY;
				} else {
					if(gameBoard[tempY][yellowCurrentX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentY = tempY; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'D') {
				int tempX = yellowCurrentX + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[yellowCurrentY][tempX] != '0'' || yellowPathFound) { 
					yellowCurrentX = yellowCurrentX;
				} else {
					if(gameBoard[tempX][yellowCurrentX] == 'Y') { //reached the end
						yellowPathFound = true;
						yellowCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						yellowCurrentX = tempX; //change Y accordingly
						gameBoard[yellowCurrentY][yellowCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else {
				yellowCurrentX = yellowCurrentX;
				yellowCurrentY = yellowCurrentY;
			}
			
		} else if(color_select == ORANGE) {
			
			if(keyPressed == 'W') {
				int tempY = orangeCurrentY - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][orangeCurrentX] != '0'' || orangePathFound) { 
					orangeCurrentY = orangeCurrentY;
				} else {
					if(gameBoard[tempY][orangeCurrentX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentY = tempY; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'A') {
				int tempX = orangeCurrentX - 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[orangeCurrentY][tempX] != '0'' || orangePathFound) { 
					orangeCurrentX = orangeCurrentX;
				} else {
					if(gameBoard[orangeCurrentY][tempX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentX = tempX; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'S') {
				int tempY = orangeCurrentY + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempY < 0 || tempY > 4 || gameBoard[tempY][orangeCurrentX] != '0'' || orangePathFound) { 
					orangeCurrentY = orangeCurrentY;
				} else {
					if(gameBoard[tempY][orangeCurrentX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentY = tempY;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentY = tempY; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else if(keyPressed == 'D') {
				int tempX = orangeCurrentX + 1;
				
				//out of bounds, trying to move to an already occupied spot, or game over: don't change position
				if(tempX < 0 || tempX > 4 || gameBoard[orangeCurrentY][tempX] != '0'' || orangePathFound) { 
					orangeCurrentX = orangeCurrentX;
				} else {
					if(gameBoard[tempX][orangeCurrentX] == 'O') { //reached the end
						orangePathFound = true;
						orangeCurrentX = tempX;
						//don't want to change the array at [currentX][currentY]
					} else { //didn't reach the end, but is a valid move
						orangeCurrentX = tempX; //change Y accordingly
						gameBoard[orangeCurrentY][orangeCurrentX] = '1'; //mark position as occupied
					}
				}
				
			} else {
				orangeCurrentX = orangeCurrentX;
				orangeCurrentY = orangeCurrentY;
			}
			
		}
		
		//game is over if all of these are true
		gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
	}
}