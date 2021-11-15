#include <stdio.h>
#include <ctime> // for time()
#include <math.h>
#include <random>
//#include "TextureBuilder.h"
#include <glut.h>
#include <Windows.h>
#include <mmsystem.h>

/*
1. draw the bird [done]
2. make the bird position change to input (space key) [done]
3. animate the change in position [done]
4. construct 3 obstacles [done]
5. make the set move to the left [done]
6. randomize the order of the sets [done]
7. collision detection [done]
8. score calculation [done]
9. power-ups [done]
10. lives counter [done]
11. detect when game ends. [done]
12. Fix gameplay speed. [done]
13. add background
14. add music
*/

// method signatures
void drawBird(int xp, int yp, int w, int h, float r);
void Key(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Timer(int value);
void print(int x, int y, char* string);
bool gameOver();
void drawPowerup();
void collectPowerup();
void endPowerup();

// for barriers
void drawBarriers();
void set1(float);
void set2(float);
void set3(float);
void lowBar();
void midBar();
void highBar();
void setBarTip(int, float);
void drawRect(int x, int y, int w, int h);
bool checkBackCollision();
bool checkFrontCollision();

// global variables
int width = 720, height = 720;
bool loser = false;
int score = 0, lives = 3;
int ypos = 160;
int barrierPos = 0;
int lastTime = 0; // time since last frame
int velo = 1; // velocity of the barrier and the bird
int setNo1, setNo2, setNo3, dispFactor = 1000;
int lowerBarTip[9][3] = { 0 }; // total 9 barriers: 4 visible tips for each barrier {x1,y, x2}
int upperBarTip[9][3] = { 0 }; // total 9 barriers: 4 visible tips for each barrier {x1,y, x2}

// for powerup generation
bool collected = false;
int g; // set no.
int start; // start of the powerup collection - should last for 40 (?) score points
int x_pow = -1, y_pow = -1; // coordinates of powerup
int cnt = 0;
// for texture
GLuint texID;
int rep = 1;

void Display() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (lives == 0 && cnt == 0) {
		PlaySound(TEXT("sound/die.wav"), NULL, SND_ASYNC);
		cnt++;
	}
	drawPowerup();
	// check if game is over
	if (gameOver()) {
		if (loser) {
			// print text "You lost!"
			glColor3f(1, 1, 1);
			char* str[20];
			sprintf((char*)str, "You lost");
			print(100, 170, (char*)str);
		}
		else {
			// print text "Winner Winner Chicken Dinner"
			glColor3f(1, 1, 1);
			char* str[20];
			sprintf((char*)str, "Winner Winner Chicken Dinner");
			print(80, 170, (char*)str);
		}
		velo = 0;
	}
	else {
		// draw bird
		glPushMatrix();
		glTranslatef(5.0, ypos, 0.0);
		drawBird(0, 0, 20, 20, 2);
		glPopMatrix();

		// draw barriers 
		// each barrier contsists of a rectangle and a line marking its borders
		glPushMatrix();
		glTranslatef(barrierPos, 0.0, 0.0);
		drawBarriers();
		glPopMatrix();
		glPushMatrix();
		glColor3f(1, 0, 0);
		for (int i = 0; i < 9; i++) {

			glBegin(GL_LINES);
			glVertex2f(lowerBarTip[i][0], lowerBarTip[i][1]);
			glVertex2f(lowerBarTip[i][2], lowerBarTip[i][1]);
			glEnd();

		}
		glColor3f(0, 0, 1);

		for (int i = 0; i < 9; i++) {
			glBegin(GL_LINES);
			glVertex2f(upperBarTip[i][0], upperBarTip[i][1]);
			glVertex2f(upperBarTip[i][2], upperBarTip[i][1]);
			glEnd();
		}

		// draw powerup (if any) in its randomly generated set
		drawPowerup();

		glPopMatrix();

		//score
		glColor3f(1, 1, 1);
		char* str[20];
		int tmp = score / 2;
		sprintf((char*)str, "Score: %d", tmp);
		print(1, 290, (char*)str);

		// lives
		glColor3f(1, 1, 1);
		sprintf((char*)str, "Lives: %d", lives);
		print(265, 290, (char*)str);
		glPushMatrix();
		
		glPopMatrix();
	}
	glFlush();
}

void main(int argc, char** argr) {
	srand((unsigned) time(0));
	glutInit(&argc, argr);


	setNo1 = (rand() % 3) + 1;
	setNo2 = (rand() % 3) + 1;
	setNo3 = (rand() % 3) + 1;

	g = rand() % 4;
	//g = 1;

	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Flappy Bird");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);
	glutTimerFunc(0, Timer, 0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//glEnable(GL_TEXTURE_2D);
	gluOrtho2D(0.0, 300, 0.0, 300);
	//loadBMP(&texID, "back.bmp", false);

	glutMainLoop();
}

void drawBird(int xp, int yp, int w, int h, float r) {
	
	glPushMatrix();
	// draw the rectangle
	glColor3f(1, 0, 0);
	drawRect(xp, yp, w, h);

	// draw the circle
	glColor3f(0, 0, 1);
	glPushMatrix();
	glTranslatef(0.7*w+xp, 0.7*h+yp, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();

	// draw triangle
	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(xp+0.7*w , 0.3*h+yp);
	glVertex2f(xp+0.7*w , yp+0.5*h);
	glVertex2f(xp+0.8*w, yp+0.4*h);
	glEnd();

	glPopMatrix();
}

// Keyboard handler function
//   key: the key pressed on the keyboard
//   y  : the y coordinate of the bird at the moment of pressing the key
void Key(unsigned char key, int x, int y) {
	//ypos = 0;
	if (key == ' ') {
		// bird rises
		ypos = (ypos + 15) > height ? height - 1 : ypos + 15;
	}

	PlaySound(TEXT("sound/point.wav"), NULL, SND_ASYNC);

	// ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();
}

void KeyUp(unsigned char key, int x, int y) {
	_sleep(100); // 100ms delay

	glutPostRedisplay();
}

void Timer(int value) {
	// translate the barriers to the left
	barrierPos -= 10*velo;

	ypos -= 5 * velo;
	if (ypos <= 0) { // bird hits the ground
		lives = 0;
	}
	else if (ypos >= 280) { // bird hits the ceiling
		lives = 0;
	}

	score+=2 * velo;

	// check if power up is collected
	collectPowerup();

	// check if power up time is up
	endPowerup();

	// if front of bird is colliding with any barrier
	if (!collected && (checkFrontCollision() || checkBackCollision())) {
		lives--;
		PlaySound(TEXT("sound/hit.wav"), NULL, SND_ASYNC);
	}

	lives = lives < 0 ? 0 : lives;
	glutPostRedisplay();

	// recall the Timer function after 20 ms.
	glutTimerFunc(200, Timer, 0);
}

void drawBarriers() {
	// generate 3 sets using
	// random number between 1 and 3
	float displacement = 0.3;
	switch (setNo1) {
	case 1:
		set1(displacement);
		break;
	case 2:
		set2(displacement);
		break;
	case 3:
		set3(displacement);
		break;
	}
	setBarTip(0, displacement);

	displacement += 0.45;
	switch (setNo2) {
	case 1:
		set1(displacement);
		break;
	case 2:
		set2(displacement);
		break;
	case 3:
		set3(displacement);
		break;
	}
	setBarTip(3, displacement);

	displacement += 0.45;
	switch (setNo3) {
	case 1:
		set1(displacement);
		break;
	case 2:
		set2(displacement);
		break;
	case 3:
		set3(displacement);
		break;
	}
	setBarTip(6, displacement);
}

// draws rectangles using the (x,y) of the bottom left vertex, width (w) and height (h)
void drawRect(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

void lowBar() {
	if (collected)
		glColor3f(1, 1, 1);
	else
		glColor3f(0, 1, 0);
	drawRect(0, 0, 0.05 * width, 0.1 * height); // first bottom rect

	if (collected)
		glColor3f(1, 1, 1);
	else
		glColor3f(0, 1, 0);
	drawRect(0, 0.18 * height, 0.05 * width, 0.29 * height); // first top rect
}

void midBar() {
	if (collected)
		glColor3f(1, 1, 1);
	else
		glColor3f(0, 1, 0);
	drawRect(0, 0, 0.05 * width, 0.2 * height); // first bottom rect

	if (collected)
		glColor3f(1, 1, 1);
	else
		glColor3f(0, 1, 0);
	drawRect(0, 0.28 * height, 0.05 * width, 0.19 * height); // first top rect
}

void highBar(){
	if (collected)
		glColor3f(1, 1, 1);
	else
		glColor3f(0, 1, 0);
	drawRect(0, 0, 0.05 * width, 0.25 * height); // first bottom rect

	if (collected)
		glColor3f(1, 1, 1);
	else
		glColor3f(0, 1, 0);
	drawRect(0, 0.33 * height, 0.05 * width, 0.1 * height); // first top rect
}

// mid high low
void set1(float disp) {
	glPushMatrix();

	glPushMatrix();
	glTranslatef(disp*dispFactor, 0, 0);
	midBar();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((disp + 0.15)*dispFactor, 0, 0);
	highBar();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((disp + 0.3)*dispFactor, 0, 0);
	lowBar();
	glPopMatrix();

	glPopMatrix();
}

// low mid high
void set2(float disp) {
	glPushMatrix();

	glPushMatrix();
	glTranslatef((disp) * dispFactor, 0, 0);
	lowBar();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((disp + 0.15) * dispFactor, 0, 0);
	midBar();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((disp + 0.3) * dispFactor, 0, 0);
	highBar();
	glPopMatrix();

	glPopMatrix();
	//glutPostRedisplay();
}

// high mid low
void set3(float disp) {
	glPushMatrix();

	glPushMatrix();
	glTranslatef((disp) * dispFactor, 0, 0);
	highBar();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((disp + 0.15) * dispFactor, 0, 0);
	midBar();
	glPopMatrix();

	glPushMatrix();
	glTranslatef((disp + 0.3) * dispFactor, 0, 0);
	lowBar();
	glPopMatrix();

	glPopMatrix();
}

void setBarTip(int i, float disp) {
	int compSet = 0;
	if (i == 0) compSet = setNo1;
	else if (i == 3) compSet = setNo2;
	else compSet = setNo3;

	switch (compSet) {
	case 1:
		// first lower bar of the set
		lowerBarTip[i][0] = disp * dispFactor + barrierPos; // x1
		lowerBarTip[i][1] = 0.2 * height; // y
		lowerBarTip[i][2] = disp * dispFactor + barrierPos + 0.05 * width; // x2

		// 2nd lower bar of the set
		lowerBarTip[i + 1][0] = (disp + 0.15) * dispFactor + barrierPos; // x1
		lowerBarTip[i + 1][1] = 0.25 * height; // y
		lowerBarTip[i + 1][2] = (disp + 0.15) * dispFactor + barrierPos + 0.05 * width; // x2

		// 3rd lower bar of the set
		lowerBarTip[i + 2][0] = (disp + 0.3) * dispFactor + barrierPos; // x1
		lowerBarTip[i + 2][1] = 0.1 * height; // y
		lowerBarTip[i + 2][2] = (disp + 0.3) * dispFactor + barrierPos + 0.05 * width; // x2

		// first upper bar of the set
		upperBarTip[i][0] = disp * dispFactor + barrierPos; // x1
		upperBarTip[i][1] = 0.28 * height; // y
		upperBarTip[i][2] = disp * dispFactor + barrierPos + 0.05 * width; // x2

		// 2nd upper bar of the set
		upperBarTip[i + 1][0] = (disp + 0.15) * dispFactor + barrierPos; // x1
		upperBarTip[i + 1][1] = 0.33 * height; // y
		upperBarTip[i + 1][2] = (disp + 0.15) * dispFactor + barrierPos + 0.05 * width; // x2

		// 3rd upper bar of the set
		upperBarTip[i + 2][0] = (disp + 0.3) * dispFactor + barrierPos; // x1
		upperBarTip[i + 2][1] = 0.18 * height; // y
		upperBarTip[i + 2][2] = (disp + 0.3) * dispFactor + barrierPos + 0.05 * width; // x2
		break;
	case 2:
		// first lower bar of the set
		lowerBarTip[i][0] = disp * dispFactor + barrierPos; // x1
		lowerBarTip[i][1] = 0.1 * height; // y
		lowerBarTip[i][2] = disp * dispFactor + barrierPos + 0.05 * width; // x2

		// 2nd lower bar of the set
		lowerBarTip[i + 1][0] = (disp + 0.15) * dispFactor + barrierPos; // x1
		lowerBarTip[i + 1][1] = 0.2 * height; // y
		lowerBarTip[i + 1][2] = (disp + 0.15) * dispFactor + barrierPos + 0.05 * width; // x2

		// 3rd lower bar of the set
		lowerBarTip[i + 2][0] = (disp + 0.3) * dispFactor + barrierPos; // x1
		lowerBarTip[i + 2][1] = 0.25 * height; // y
		lowerBarTip[i + 2][2] = (disp + 0.3) * dispFactor + barrierPos + 0.05 * width; // x2

		// first upper bar of the set
		upperBarTip[i][0] = disp * dispFactor + barrierPos; // x1
		upperBarTip[i][1] = 0.18 * height; // y
		upperBarTip[i][2] = disp * dispFactor + barrierPos + 0.05 * width; // x2

		// 2nd upper bar of the set
		upperBarTip[i + 1][0] = (disp + 0.15) * dispFactor + barrierPos; // x1
		upperBarTip[i + 1][1] = 0.28 * height; // y
		upperBarTip[i + 1][2] = (disp + 0.15) * dispFactor + barrierPos + 0.05 * width; // x2

		// 3rd upper bar of the set
		upperBarTip[i + 2][0] = (disp + 0.3) * dispFactor + barrierPos; // x1
		upperBarTip[i + 2][1] = 0.33 * height; // y
		upperBarTip[i + 2][2] = (disp + 0.3) * dispFactor + barrierPos + 0.05 * width; // x2
		break;
	case 3:
		// first lower bar of the set
		lowerBarTip[i][0] = disp * dispFactor + barrierPos; // x1
		lowerBarTip[i][1] = 0.25 * height; // y
		lowerBarTip[i][2] = disp * dispFactor + barrierPos + 0.05 * width; // x2

		// 2nd lower bar of the set
		lowerBarTip[i + 1][0] = (disp + 0.15) * dispFactor + barrierPos; // x1
		lowerBarTip[i + 1][1] = 0.2 * height; // y
		lowerBarTip[i + 1][2] = (disp + 0.15) * dispFactor + barrierPos + 0.05 * width; // x2

		// 3rd lower bar of the set
		lowerBarTip[i + 2][0] = (disp + 0.3) * dispFactor + barrierPos; // x1
		lowerBarTip[i + 2][1] = 0.1 * height; // y
		lowerBarTip[i + 2][2] = (disp + 0.3) * dispFactor + barrierPos + 0.05 * width; // x2

		// first upper bar of the set
		upperBarTip[i][0] = disp * dispFactor + barrierPos; // x1
		upperBarTip[i][1] = 0.33 * height; // y
		upperBarTip[i][2] = disp * dispFactor + barrierPos + 0.05 * width; // x2

		// 2nd upper bar of the set
		upperBarTip[i + 1][0] = (disp + 0.15) * dispFactor + barrierPos; // x1
		upperBarTip[i + 1][1] = 0.28 * height; // y
		upperBarTip[i + 1][2] = (disp + 0.15) * dispFactor + barrierPos + 0.05 * width; // x2

		// 3rd upper bar of the set
		upperBarTip[i + 2][0] = (disp + 0.3) * dispFactor + barrierPos; // x1
		upperBarTip[i + 2][1] = 0.18 * height; // y
		upperBarTip[i + 2][2] = (disp + 0.3) * dispFactor + barrierPos + 0.05 * width; // x2
		break;
	}
}

//this is the method used to print text in OpenGL
//there are three parameters,
//the first two are the coordinates where the text is display,
//the third coordinate is the string containing the text to display
void print(int x, int y, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

// checks if the game should continue
bool gameOver() {
	if (lives < 1) {
		loser = true;
		return true;
	}
	if (score >= 316) // end of game reached
		return true;

	return false;
}

bool checkFrontCollision() {
	int xpos = 25;

	// check bottom collision
	for (int i = 0; i < 9; i++) {
		if (xpos >= lowerBarTip[i][0] && xpos <= lowerBarTip[i][2] && ypos <= lowerBarTip[i][1]) {
			ypos = lowerBarTip[i][1] + 7;
			return true; // collision occurred with lower barrier
		}
	
		if (xpos >= upperBarTip[i][0] && xpos <= upperBarTip[i][2] && ypos+20 >= upperBarTip[i][1]) {
			ypos = upperBarTip[i][1] - 20;
			return true; // collision occurred with upper barrier
		}
	}
	return false;
}

bool checkBackCollision() {
	int xpos = 5;

	// check bottom collision
	for (int i = 0; i < 9; i++) {
		if (xpos >= lowerBarTip[i][0] && xpos <= lowerBarTip[i][2] && ypos <= lowerBarTip[i][1]) {
			ypos = lowerBarTip[i][1] + 7;
			return true; // collision occurred with lower barrier
		}

		if (xpos >= upperBarTip[i][0] && xpos <= upperBarTip[i][2] && ypos + 20 >= upperBarTip[i][1]) {
			ypos = upperBarTip[i][1] - 20;
			return true; // collision occurred with upper barrier
		}
	}
	return false;
}

// draws the powerup shape (square+triangle) in its respective set
void drawPowerup(){
	if (g == 0) { // draw nothing
		return;
	}
	else if (g == 1) { // draw in the first set
		x_pow = lowerBarTip[0][0]+5;
		y_pow = lowerBarTip[0][1] + 5;
	}
	else if (g == 2) { // draw in the 2nd set
		x_pow = lowerBarTip[3][0] + 5;
		y_pow = lowerBarTip[3][1] + 5;
	}
	else { // draw in the 3rd set
		x_pow = lowerBarTip[6][0] + 5;
		y_pow = lowerBarTip[6][1] + 5;
	}
	glColor3f(1, 0, 1);
	drawRect(x_pow, y_pow, 10, 10);
	glColor3f(1, 1, 1);
	glBegin(GL_TRIANGLES);
	glVertex2f(x_pow+2,y_pow+2);
	glVertex2f(x_pow+5,y_pow+5);
	glVertex2f(x_pow+3,y_pow+5);
	glEnd();
}

void collectPowerup() {
	int xpos = 25;

	// check bottom collision
	if (xpos >= x_pow && xpos <= x_pow + 10 && ypos <= y_pow) {
		// powerup activated
		collected = true;
		return; // collision occurred with lower barrier
	}


	xpos = 5;

	// check bottom collision

	if (xpos >= x_pow && xpos <= x_pow + 10 && ypos <= y_pow) {
		// power up activated
		collected = true;
		return;
	}
}

void endPowerup() {
	if (!collected) {
		return;
	}

	// if the last bar of the powered set is skipped, deactivate.
	if (lowerBarTip[3 * (g - 1) + 2][2] < 5)
		collected = false;
}
