#include <stdio.h>
#include <stdbool.h>

//removed for testing purposes
//#include "configGIC.h"
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
	
bool isLegalMove(int color_select, char keyPressed);	

//game board
char gameBoard[5][5];

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

int main() { //testing
	//char board[5][5];
	int color_select = BLUE;
	char keyPressed = 'W';
	
	bool isValidMove = isLegalMove(color_select, keyPressed);
        
		//testing
		/*
        if(isValidMove) {
            printf("that was a valid move\n");
        } else {
            printf("that was an invalid move\n");
        }
        
        printf("after move has been made: \n");
        int x,y;
        for(x = 0; x < 5; x++) {
			for(int y = 0; y < 5; y++) {
				printf("%c ", gameBoard[x][y]);
			}
			printf("\n");
		}*/
	
	return 0;
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
			
		}
		
		//game is over if all of these are true
		//gameOver = redPathFound && greenPathFound && bluePathFound && yellowPathFound && orangePathFound;
	//}
}