/*
------------------------------------------------------------------------------------------------------------------------------
														Tanks

															By, Varun Ramakrishnan
------------------------------------------------------------------------------------------------------------------------------
*/
#include "cgt215.h"
#include "demo.h"
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <Windows.h>
#include <WinUser.h>

using namespace std;
using namespace cgt215;

// Declare new functions here
void drawTree(double x, double z);
void drawPine(double x, double z);
void trunk(void);

// Global variables for program
long int	score; // Player's score
bool		win; // Variable becomes true if user wins
double		xT, zT; // Variable that stores the coordinates of the target
time_t		startTimeSec = time(NULL); // Keeps track of time

class TankT // This is a class that contains all attributes that are common for all tanks.
{
	public:
			// All elements that are common for all tanks
			unsigned int	Health; // Health of tank
			int				posXYZ[3]; // Coordinates of tank
							TankT(); // Constructor for class TankT
}player[2];

TankT::TankT() // Constructor to initialise the member variables.
{
	Health = 100;
	posXYZ[0] = 0; 
	posXYZ[1] = 10; 
	posXYZ[2] = 200; 
}

void gameOver() // Game Over screen in game
{
	/*
		This function is called when time runs out and the game ends.
	*/

	// Clears screen
	system("cls");

	// Game Over screen
	setBackgroundColor(0, 0, 0);

	setColor(1, 0, 0);

	drawText(0, 0, "Game Over!");
	drawText(0, -20, "Time Up.");
}

void checkShot(double x, double y, double z) // Checks if a target has been shot
{
	/*
		This function checks if a target has been hit or not.
		If it is hit, it increases the player's score.
	*/

	// Checks for shot
	if (((x <= xT + 50) && (x >= xT)) && /*((y <= 50) && (y >= 0)) &&*/ ((z <= zT + 10) && (z >= zT))) // Checks is cannonball is inside target
		score += 5;
}

void randTarget() // Draws rectangular targets randomly
{
	/*
		This is called by drawArena().
		This function makes the target appear in random areas.
	*/

	Sleep(2);
			
	// Assigns random x, z coorddinates
	xT = rand() % 4000;
	zT = rand() % 4000;

	// Draws targets
	if ((xT < 250 && xT > -250) && (zT < 250 && zT > -250)) // Checks if it is inside the arena's dimensions.
	{
		setColor(1, 0, 1);
		drawRectangle(xT, 0, zT, 50, 50, 10); // Bottom right quadrant of arena.
		drawRectangle(-xT, 0, -zT, 50, 50, 10); // Top left quadrant of arena.
		drawRectangle(-xT, 0, zT, 50, 50, 10); // Bottom left quadrant of arena.
		drawRectangle(xT, 0, -zT, 50, 50, 10); // Top right quadrant of arena
	}
}

void menu() // Menu page (Not used)
{
	/*
		The game will open with this which will display the game name on top-center and "Play", "Rules", and "Credits" below that in rectangles.
		The "Play", "Rules", and "Credits" will be a button. 
		It will basically be a text inside a rectangle that checks if the mouse is clicked inside the rectangle, if it is, it calls that respective function.
	*/

	// Clears screen
	system("cls");

	void play();
	void rules();
	void credits();

	//Title
	setBackgroundColor(0, 0, 1);
	setColor(1, 1, 0);
	drawText(-20, 300, 0, "War of Tanks");

	//Menu Items
	setColor(0, 1, 0); // Sets Rectangle's color to green
	drawRectangle(23, 155, -10, 60, 25, 5); // Rect. for Play
	drawRectangle(25, 105, -10, 60, 25, 5); // Rect. for Rules
	drawRectangle(30, 55, -10, 75, 25, 5); // Rect. for Credits
	drawRectangle(25, 5, -10, 50, 25, 5); // Rect. for Exit

	setColor(1, 0, 0);
	drawText(0, 150, 0, "[P]lay"); // Draws "Play"
	drawText(0, 100, 0, "[R]ules"); // Draws "Rules"
	drawText(-5, 50, 0, "[C]redits"); // Draws "Credits"
	drawText(0, 0, 0, "[E]xit"); // Draws "Exit"

	// Options for user to select an option -- Make responsive to mouse!!
	if (GetKeyState('P') || GetKeyState('p')) play();
		else if (GetKeyState('R') || GetKeyState('r')) rules();
			else if (GetKeyState('C') || GetKeyState('c')) credits();
				else if (GetKeyState('E') || GetKeyState('e')) exit(0);
}

void play() // Starts the game
{
	/*
		This function is called if the user clicks on "Play" on the menu screen.             
		This is where all the main logic of the game is.
	*/

	setWireframeRendering(false);

	void drawArena(); // Fn. prototype
	void drawTank(); // Fn. prototype
	void Shoot(TankT a[]); // Fn. prototype
	void move(); // Fn. prototype

	drawArena(); // Draws Arena for battle 

	drawTank(); // Draws tank for player
		
	move(); // Starts moving the tank
}

void drawArena() // Draws Arena for battle
{
	/*
		This function is called by play().
		It draws the arena where the battle takes place.
	*/

	setWireframeRendering(false);

	// Draws the battlefield for the game
	setBackgroundColor(0, 0, 0);

	setColor(1, 0, 0);
	drawRectangle(0, -30, 0, 500, 30, 500);

	// Draws targets
	for (int i = 0; i < 50; i++)
		randTarget();
	
}

void drawTank() // This functions draws the tank
{
	/*
		This is called by play().
		This function is used to draw the tanks.
	*/

	setWireframeRendering(false);

	// Draws Tank's main body
	setColor(0, 1, 1);
	drawRectangle(player[0].posXYZ[0], player[0].posXYZ[1], player[0].posXYZ[2], 50, 50, 50);

	// Draws Tank's wheels
	setColor(1, 1, 0);
	drawRectangle(player[0].posXYZ[0] + 34, player[0].posXYZ[1] - 14, player[0].posXYZ[2], 30, 30, 65);
	drawRectangle(player[0].posXYZ[0] - 34, player[0].posXYZ[1] - 14, player[0].posXYZ[2], 30, 30, 65);

	// Draws Tank's head
	setColor(1, 0, 1);
	drawRectangle(player[0].posXYZ[0], player[0].posXYZ[1] + 25, player[0].posXYZ[2], 30, 30, 30);

	// Draws Tank's cannon
	setColor(0.4, 1, 0.5);
	drawRectangle(player[0].posXYZ[0] + 1, player[0].posXYZ[1] - 5, player[0].posXYZ[2] - 25, 20, 20, 80);
}

void shoot(TankT a[]) // Displays the cannonball when it's shot
{
	/*
		This is called by move().
		This function draws the cannonball if the player shoots.
	*/

	// Coordinates of cannon to shoot from
	double	x, y, z; // Coordinates of center of cannonball

			x = a[0].posXYZ[0] + 1;
			y = a[0].posXYZ[1] - 5;
			z = a[0].posXYZ[2] - 25;

			// Draws Cannonball and checks if a target was hit
			for (int i = 0; i < 100; i++, z-=5)
			{
				setColor(0, 1, 1);
				drawSphere(x, y, z, 10);

				checkShot(x, y, z); // Checks if shot hit a target
			}
}

void move() // Function for moving tank and also for shooting
{
	/*
		This function is called by play().
		It makes the tank move by changing the directional coordinate with the corresponding key that is pressed.
		The tank will move using W, A, S, D.
	*/

	// Shoots if 1 is pressed
	if (isKeyDown('1')) shoot(player);

	// Exit if user presses 'e' or 'E'
	if (GetKeyState('E') || GetKeyState('e')) exit(0);

	// Movement using W, A, S, D

	if (GetKeyState('D') & 0x8000) // Moves Right if D is pressed
		if (player[0].posXYZ[0] < 200)	player[0].posXYZ[0] += 10;
	if (GetKeyState('A') & 0x8000) // Moves Left if A is pressed
		if (player[0].posXYZ[0] > -200)	player[0].posXYZ[0] -= 10;
	if (GetKeyState('W') & 0x8000) // Moves forward if W is pressed
		if (player[0].posXYZ[2] > -200)	player[0].posXYZ[2] -= 10;
	if (GetKeyState('S') & 0x8000) // Moves Backward if S is pressed
		if (player[0].posXYZ[2] < 200)	player[0].posXYZ[2] += 10;
}

void rules() // Diplays rules (Not used)
{
	/*
		This function is called if the user clicks on "Rules" in the menu.
		It displays the objective, rules, how-to's.
	*/

	// Clears screen
	system("cls");

	// Rules
	setBackgroundColor(1, 0, 0);

	setColor(1, 1, 1);
	drawText(0, 300, 0, "Rules");
	drawText(-300, 250, 0, "1. You, the player, plays as a tank.");
	drawText(-300, 230, 0, "2. You use W, A, S, D to move and TAb to shoot.");
	drawText(-300, 210, 0, "3. The targets appear randomly and must be shot before they dissapears.");
	drawText(-300, 190, 0, "4. The objective of the game is to shoot the minimum number of targets");
		drawText(-270, 170, 0, "before time runs out.");
	drawText(-300, 150, 0, "5. You can use the mouse to rotate the arena and to zoom in and out.");
		drawText(-270, 130, 0, "-> Right click to rotate the arena and left click to zoom in and out.");
	drawText(-20, 90, 0, "Goodluck!");
}

void credits() // Displayes credits (Not used)
{
	/*
		This function is called when the user clicks on "Credits" in the menu.
		It displays the names of people who we credit in finishing this project.
	*/

	// Clears screen
	system("cls");

	// Credits
	setBackgroundColor(1, 0, 0);

	setColor(1, 1, 1);
	drawText(0, 300, 0, "Credits");
	drawText(-300, 200, 0, "Team: ");
		drawText(-290, 180, 0, "Varun Ramakrishnan");
		drawText(-290, 160, 0, "Tim Huber");
		drawText(-290, 140, 0, "Logan Leiter");
		drawText(-290, 120, 0, "Mohit Patel");
	drawText(-300, 50, 0, "We would like to thank Ms. Alexa Sears and Prof. Bedrich Benes for");
	drawText(-300, 30, 0, "their invaluable advice and guidance for making this project");
}

/// This method is called automatically every frame of animation (usually 60 times per second).
/// Parameter lastFrameDuration is amount of time (in seconds) which was needed to render previous frame.
void renderFrame(float lastFrameDuration) 
{
		// Place interactive/animated drawing code here
		enable3d();
		setWireframeRendering(false);

	int	gameTime = 120; // VAriable holds the seconds that the game runs for

		cout << "Time: " << time(NULL) - startTimeSec << endl;
		cout << "Score: " << score << endl;

		if (time(NULL) - startTimeSec <= gameTime) play();
			else gameOver();
}

/// This is a special method where cgtin and cgtout can be used.
/// It is invoked once at the beginning of program.
void cgtConsole() 
{
    // Place CGT Console code here

}

/// Main method of the program which initializes the framework.
int main(int argc, char** argv) 
{
	srand((unsigned int)time(NULL));

	/// INITIALIZES THE CGT 215 FRAMEWORK - DO NOT CHANGE
	initializeCgt215(argc, argv, cgtConsole, renderFrame, "War of Tanks");
}

void drawPine(double x, double z) {
	glPushMatrix();
	glTranslated(x, 0.0, z);
		trunk();
		glPushMatrix();
			glTranslated(0.0, 15.0, 0.0);
			glScaled(1.0, 0.75, 1.0);
			drawCone();
		glPopMatrix();
	glPopMatrix();
	return;
}

void drawTree(double x, double z) {
	glPushMatrix();
		glTranslated(x, 0.0, z);
		trunk();
		glPushMatrix();
			glTranslated(0.0, 40.0, 0.0);
			drawSphere(25.0);
		glPopMatrix();
	glPopMatrix();
	return;
}

void trunk(void) {
	glPushMatrix();
		glScaled(1.0, 4.0, 1.0);
		glRotated(90.0, 1.0, 0.0, 0.0);
		drawTorus(5.0, 6.0);
	glPopMatrix();
}