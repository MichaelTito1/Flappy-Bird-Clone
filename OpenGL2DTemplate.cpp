
#include <stdio.h>
#include <ctime> // for time()
#include <math.h>
#include <random>
#include <glut.h>
/*
1. draw the bird [done]
2. make the bird position change to input (space key) [done]
3. animate the change in position [done]
4. construct 3 sets of obstacles
5. make the set move to the left
6. randomize the order of the sets
7. collision detection
8. score calculation
9. power-ups
10. lives counter
11. detect when game ends.
12. add background
13. add music
*/

// method signatures
void drawBird(int xp, int yp, int w, int h, float r);
void drawBarriers();
void Key(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Timer(int value);
void Anim();
void set1();
void set2();
void set3();
void drawRect(int x, int y, int w, int h);

// global variables
int width = 720, height = 720;
bool gameOver = false;
int ypos = 0;
int barrierPos = width;
int lastTime = 0; // time since last frame
float velo = 0.0; // falling velocity of the bird

void Display() {
	glClear(GL_COLOR_BUFFER_BIT);

	// draw bird
	glTranslatef(0.0, ypos, 0.0);
	drawBird(120, 160, 20, 20, 2);

	//drawBarriers();

	//set1();

	glFlush();
}

void main(int argc, char** argr) {
	srand((unsigned) time(0));
	glutInit(&argc, argr);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("Flappy Bird");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);
	glutTimerFunc(0, Timer, 0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	gluOrtho2D(0.0, 300, 0.0, 300);

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
	ypos = 0;
	if (key == ' ') {
		// bird rises
		if(ypos < height)
			ypos+=6;
	}
	// ask OpenGL to recall the display function to reflect the changes on the window
	glutPostRedisplay();
}

void KeyUp(unsigned char key, int x, int y) {
	// bird starts falling
	/*while (ypos > 0) {
		ypos--;
		_sleep(20);
	}*/

	//int time = glutGet(GLUT_ELAPSED_TIME); // Time since the start of the program
	//int deltaTime= 0;
	//if (lastTime > 0) { // Don't move anything the first frame
	//	deltaTime = time - lastTime; // millis elapsed since the last frame
	//	velo += -0.005 * deltaTime; // Gravity accelerates downwards
	//	ypos += velo * deltaTime; // Position updated by velocity
	//	//ypos *= -1;
	//	//printf(ypos+"");
	//	//printf("\n");
	//}
	//lastTime = deltaTime;
	_sleep(100);
	while (ypos > 0) {
		ypos = 0;
		ypos -=3;
		//ypos = ypos < 0 ? 0 : ypos;
		glutPostRedisplay();
	}
}

void Timer(int value) {
	// translate the barriers to the left
	barrierPos -= 10;

	glutPostRedisplay();

	// recall the Timer function after 20 ms.
	glutTimerFunc(200, Timer, 0);
}

void drawBarriers() {
	// generate random number between 1 and 3
	int setNo = (rand()%3)+1; 
	switch (setNo) {
	case 1:
		set1();
		break;
	case 2:
		set2();
		break;
	case 3:
		set3();
		break;
	}
	glutPostRedisplay();
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

void set1() {
	glPushMatrix();

	glColor3f(0.5, 0.5, 0.5);
	drawRect(width*0.3, 0, 0.1 * width, 0.4*height); // first bottom rect

	//glColor3f(1, 1, 1);
	//drawRect(width*0.6, 0, 0.1*width, 0.4*height); // second bottom rect

	glColor3f(0, 0, 1);
	drawRect(width*0.3, 0.6*height, 0.1*width, 0.4*height); // first top rect
	
	//glColor3f(0, 0.5, 0);
	//drawRect(width*0.6, 0.6*height, 0.1*width, 0.4 * height); // second top rect

	glPopMatrix();
}

void set2() {
	
}

void set3() {

}