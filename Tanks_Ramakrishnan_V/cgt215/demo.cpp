/*
 * Copyright 2013
 *     Marek Fiser <mfiser@purdue.edu>
 */
#include "demo.h"
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
/// CgtConsole demo
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {

	namespace cgtConsoleDemo {

		void cgtConsoleDemo_renderFrame(float lastFrameDuration) {
			// This demo do not use renderFrame function at all.
			// All logic is in processCgtInOut function.
		}
	
		void cgtConsoleDemo_processCgtInOut() {
			cgtout << "This program will draw circle or rectangle at mouse cursor's position" << std::endl << std::endl;
		
			// Main cycle, keeps asking for drawing circle or rectangle until [e] is pressed and cycle is breaked 
			while (true) {
				cgtout << "Draw [c]ircle, [r]ectangle or [e]xit: ";
				char c;
				cgtin >> c;  // read a character as user option

				if (c == 'c') {
					// Circle was selected, keep asking user for radius until he enters positive value
					double r;
					while (true) {
						cgtout << "Radius: ";
						cgtin >> r;
						if (r > 0) {
							break;  // user entered positive value, break the "asking" cycle
						}
						cgtout << "Keep it positive!" << std::endl;
					}

					// set color to yellow and draw a circle centered at mouse cursor's position
					setColor(1, 1, 0);
					drawCircle(getMouseX(), getMouseY(), r);
				}
				else if (c == 'r') {
					// Rectangle was selected, keep asking user for width and height in same manner as in cirlce branch
					double wid, hei;
					while (true) {
						cgtout << "Width: ";
						cgtin >> wid;
						if (wid > 0) {
							break;
						}
						cgtout << "Keep it positive!" << std::endl;
					}
					while (true) {
						cgtout << "Height: ";
						cgtin >> hei;
						if (hei > 0) {
							break;
						}
						cgtout << "Keep it positive!" << std::endl;
					}

					// Both width and height were entered and are positive, draw rectangle with center at mouse cursor's position
					setColor(0, 1, 0);
					drawRectangle(getMouseX() - wid / 2, getMouseY() - hei / 2, wid, hei);
				}
				else if (c == 'e') {
					break;  // break the main cycle
				}
				else {
					// unknown command, cycle will continue and ask for a command again
					cgtout << "Unknown command!" << std::endl;
				}
			}

			// End program when main cycle ends
			exit(EXIT_SUCCESS);
		}
	}

	void runCgtConsoleDemo(int argc, char** argv, const std::string& title) {
		initializeCgt215(argc, argv, cgtConsoleDemo::cgtConsoleDemo_processCgtInOut, cgtConsoleDemo::cgtConsoleDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}
}


////////////////////////////////////////////////////////////////////////////////
/// Animation demo
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {

	namespace animDemo {

		// Total amount of time elapsed from start of program
		float totalTime;

		void animationDemo_renderFrame(float lastFrameDuration) {
			// Adds last frame duration to total -- counts total time from start of program
			totalTime += lastFrameDuration;

			////////////////////////////////////////////////////////////////////////
			// First part -- blue teapot rotating around red wired sphere
			////////////////////////////////////////////////////////////////////////

			// Enables 3D rendering
			enable3d();
	
			// Enables wireframe rendering -- rendering of edges, not faces
			setWireframeRendering(true);
			// Draws red sphere in the center of coordinate space (and screen)
			setColor(1, 0, 0);
			drawSphere(0, 0, 0, 80);
	
			// Disables wireframe rendering -- rendered objects will be solid and shaded
			setWireframeRendering(false);
		
			// Draws green sphere inside red one
			setColor(0, 1, 0);
			drawSphere(0, 0, 0, 50);

			// Pushes new transformation matrix on the stack
			// Matrix operations are applied from the last to the first (in code)
			glPushMatrix();
				// 3) Translates scaled teapot on circular trajectory (based on time)
				glTranslated(std::sin(totalTime) * 300, 0.0, std::cos(totalTime + 3.14159) * 300);
				// 2) Scales rotated teapot
				glScaled(2.0, 2.0, 2.0);
				// 1) Rotates teapot around its own axis once per second
				glRotated(totalTime * 360, 0.0, 1.0, 0.0);

				// Draws blue teapot using selected transformations
				setColor(0, 0, 1);
				drawTeapot();
			glPopMatrix();  // Pop (delete) transformation matrix from stack
		

			////////////////////////////////////////////////////////////////////////
			// Second part -- flat 2d code and cube moving horizontally
			////////////////////////////////////////////////////////////////////////

			// Disables 3D rendering, 2D renderings are always flat color (not shaded)
			disable3d();

			glPushMatrix();
				// 2) Translate cone according to total time
				glTranslated(150 + std::sin(totalTime) * 50, 50.0, 0.0);
				// 1) Scale cone
				glScaled(2.0, 2.0, 2.0);
				// Draw wireframe of yellow cone
				setWireframeRendering(true);
				setColor(1, 1, 0);
				drawCone();
			glPopMatrix();

			glPushMatrix();
				// 2) Translate cube according to total time
				glTranslated(150 - std::sin(totalTime) * 50, 50.0, 0.0);
				// 1) Scale cube
				glScaled(2.0, 2.0, 2.0);
				// Draw solid yellow cube
				setWireframeRendering(false);
				setColor(1, 1, 0);
				drawCube();
			glPopMatrix();


			////////////////////////////////////////////////////////////////////////
			// Third part -- spinning toruses
			////////////////////////////////////////////////////////////////////////
		
			enable3d();
			setWireframeRendering(false);
				
			float radius = 15;
			float sizeOuter = 200;
			float sizeMiddle = 170;
			float sizeInner = 140;

			glPushMatrix();
				// Torus 1 -- red outer ring spinning around x axis once per 4 seconds
				glRotated(totalTime * 90, 1.0, 0.0, 0.0);
				setColor(1, 0, 0);
				drawTorus(radius, sizeOuter);

				// Torus 2 -- green middle ring spinning additionally around y axis once per 3 seconds
				glRotated(totalTime * 120, 0.0, 1.0, 0.0);
				// Draw solid green torus
				setColor(0, 1, 0);
				drawTorus(radius, sizeMiddle);

				// Torus 3 -- blue inner ring spinning additionally around x axis once per 1 second
				glRotated(-totalTime * 180, 1.0, 0.0, 0.0);
				// Draw solid green torus
				setColor(0, 0, 1);
				drawTorus(radius, sizeInner);
			glPopMatrix();
		}
	
		void animationDemo_processCgtInOut() {
			cgtout << "Use mouse to rotate the camera.";
		}
	}

	void runAnimationDemo(int argc, char** argv, const std::string& title) {
		initializeCgt215(argc, argv, animDemo::animationDemo_processCgtInOut, animDemo::animationDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}

}

////////////////////////////////////////////////////////////////////////////////
/// Fractal tree demo
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {
	
	namespace fractTreeDemo {

		float branchLength = 100;
		float angleInc = 30;

		void drawTree(float x, float y, float len, float angle) {
	
			// Step at given angle
			float newX = x + len * (float)std::cos(angle / 180.0 * 3.141592); 
			float newY = y + len * (float)std::sin(angle / 180.0 * 3.141592); 
	
			// Shorter branches will be more red.
			setColor(1, 2 * len / branchLength, 2 * len / branchLength);
			// Draw branch as a line
			drawLine(x, y, newX, newY);

			// If branch is long enough, draw child branches using recursion
			if (len > 5) {
				drawTree(newX, newY, len * 0.8f, angle);
				drawTree(newX, newY, len * 0.7f, angle + angleInc);
			}
		}


		void fractalTreeDemo_renderFrame(float lastFrameDuration) {

			// Testing of pressed keys
			// Values are multiplied by lastFrameDuration to ensure that change will not be dependent
			// on frame rate of application but on time.
			if (isKeyDown('q')) {
				branchLength += 30.0f * lastFrameDuration;  // change by 30 pixels per second
			}
			if (isKeyDown('w')) {
				branchLength -= 30.0f * lastFrameDuration;  // change by 30 pixels per second
			}
	
			if (isKeyDown('e')) {
				angleInc += 25.0f * lastFrameDuration;  // change by 25 degrees per second
			}
			if (isKeyDown('r')) {
				angleInc -= 25.0f * lastFrameDuration;  // change by 25 degrees per second
			}

			// Draw fractal tree using recursion
			drawTree(getScreenWidth() / 2.0f, 0.0f, branchLength, 90);
		}
	
		void fractalTreeDemo_processCgtInOut() {
			cgtout << "Use keys [q] and [w] to adjust branch length" << std::endl;
			cgtout << "Use keys [e] and [r] to adjust branch angle" << std::endl;
		}
	}

	void runFractalTreeDemo(int argc, char** argv, const std::string& title) {
		initializeCgt215(argc, argv, fractTreeDemo::fractalTreeDemo_processCgtInOut, fractTreeDemo::fractalTreeDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}

}


////////////////////////////////////////////////////////////////////////////////
/// Simple snake demo
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {
	
	namespace simpleSnakeDemo {

		float bodyPartPosX[1024];
		float bodyPartPosY[1024];

		int bodyPartCount = 0;

		float appleX = -1;
		float appleY = -1;

		float angle = 0.0f;
		float speed = 100.0f;
		float bodySize = 20.0f;

		float speedInc = 5.0f;

	
		void moveSnake(float lastFrameDuration) {
	
			float actSpeed = speed + bodyPartCount * speedInc;

			for	(int i = 0; i < bodyPartCount; ++i) {

				if (i == 0) {
					bodyPartPosX[i] += (float)(actSpeed * cos(angle / 180 * 3.14159) * lastFrameDuration);
					bodyPartPosY[i] += (float)(actSpeed * sin(angle / 180 * 3.14159) * lastFrameDuration);
				}
				else {
					float diffVectX = bodyPartPosX[i] - bodyPartPosX[i - 1];
					float diffVectY = bodyPartPosY[i] - bodyPartPosY[i - 1];

					float distance = (float)sqrt(diffVectX * diffVectX + diffVectY * diffVectY);
					if (distance < bodySize) {
						continue;
					}
			
					// move in direction of diff vector by bodySize
					float moveVectX = (diffVectX / distance) * bodySize;
					float moveVectY = (diffVectY / distance) * bodySize;
			
					bodyPartPosX[i] = bodyPartPosX[i - 1] + moveVectX;
					bodyPartPosY[i] = bodyPartPosY[i - 1] + moveVectY;
				}

			}
		}

		void addBodyPart() {
			bodyPartPosX[bodyPartCount] = bodyPartPosX[bodyPartCount - 1];
			bodyPartPosY[bodyPartCount] = bodyPartPosY[bodyPartCount - 1];
		
			++bodyPartCount;
		}

		void drawSnakeAndApple() {

			for	(int i = 0; i < bodyPartCount; ++i) {
				setColor(1, 0, 0);
				drawCircle(bodyPartPosX[i], bodyPartPosY[i], bodySize / 2.0f);
			}

			if (appleX > 0) {
				setColor(0, 1, 0);
				drawSphere(appleX, appleY, 0, bodySize / 2.0f);
			}
		}

		void killSnake() {
			angle = 0.0f;
			bodyPartCount = 0;
		}

		void checkSnakeCollisions() {

			float radius = bodySize / 2.0f;

			if (bodyPartPosX[0] - radius < 0 || bodyPartPosY[0] - radius < 0 ||
					bodyPartPosX[0] + radius >= getScreenWidth() || bodyPartPosY[0] + radius >= getScreenHeight()) {
				// snake hits the wall
				killSnake();
			}
	
			for	(int i = 2; i < bodyPartCount; ++i) {
				float dx = bodyPartPosX[0] - bodyPartPosX[i];
				float dy = bodyPartPosY[0] - bodyPartPosY[i];
				float dist = (float)sqrt(dx * dx + dy * dy);
		
				if (dist < bodySize) {
					// snake hits itself
					killSnake();
				}
			}
	
			float dx = bodyPartPosX[0] - appleX;
			float dy = bodyPartPosY[0] - appleY;

			float appleDistance = (float)sqrt(dx * dx + dy * dy);
			if (appleDistance < bodySize) {
				appleX = -1;
				addBodyPart();
			}

		}
	
		void simpleSnakeDemo_renderFrame(float lastFrameDuration) {
			
			checkSnakeCollisions();

			if (bodyPartCount == 0) {
				bodyPartPosX[0] = getScreenWidth() / 2.0f;
				bodyPartPosY[0] = getScreenHeight() / 2.0f;
				bodyPartPosX[1] = bodyPartPosX[0] - bodySize;
				bodyPartPosY[1] = bodyPartPosY[0] - bodySize;
				bodyPartCount = 2;
			}

			if (appleX < 0 || appleX + 50 > getScreenWidth() || appleY + 50 > getScreenHeight()) {
				appleX = 50.0f + (rand() % (int)(getScreenWidth() - 100));
				appleY = 50.0f + (rand() % (int)(getScreenHeight() - 100));
			}
	
			if (isKeyDown('a') || isSpecialKeyDown(GLUT_KEY_LEFT)) {
				angle += (180.0f + bodyPartCount * speedInc) * lastFrameDuration;
			}
			if (isKeyDown('d') || isSpecialKeyDown(GLUT_KEY_RIGHT)) {
				angle -= (180.0f + bodyPartCount * speedInc) * lastFrameDuration;
			}
			// cheat key
			if (keyPressed('+')) {
				addBodyPart();
			}
	
			moveSnake(lastFrameDuration);
			drawSnakeAndApple();

			setColor(1, 1, 1);
			drawText(10, 10, 0, std::to_string((long long int)bodyPartCount));
		}
	
		void simpleSnakeDemo_processCgtInOut() {

		}
	}

	void runSimpleSnakeDemo(int argc, char** argv, const std::string& title) {
		initializeCgt215(argc, argv, simpleSnakeDemo::simpleSnakeDemo_processCgtInOut, simpleSnakeDemo::simpleSnakeDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}

}
	

////////////////////////////////////////////////////////////////////////////////
/// Advanced snake demo
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {
	
	namespace advSnakeDemo {

		float bodyPartPosX[1024];
		float bodyPartPosY[1024];
		float bodyPartSize[1024];
		int bodyPartCount = 0;

		float eatenApples[64];
		int eatenApplesFirstIndex = 0;
		int eatenApplesCount = 0;


		float appleX = -1;
		float appleY = -1;

		float angle = 0.0f;
		float speed = 100.0f;
		float bodySize = 20.0f;
		float appliSize = 30.0f;

		float speedInc = 5.0f;
	

		void killSnake() {
			bodyPartCount = 0;
			eatenApplesCount = 0;
			angle = 0.0f;
		}

		void eatApple() {
			eatenApples[eatenApplesFirstIndex + eatenApplesCount] = -1;
			++eatenApplesCount;
		}

		void addBodyPart() {
			bodyPartPosX[bodyPartCount] = bodyPartPosX[bodyPartCount - 1];
			bodyPartPosY[bodyPartCount] = bodyPartPosY[bodyPartCount - 1];
		
			++bodyPartCount;
		}

		void moveSnakeAndEatenApples(float lastFrameDuration) {

			float snakeStep = (speed + bodyPartCount * speedInc) * lastFrameDuration;
			float applesStep = snakeStep / bodySize;

			// move apples (inside a snake)
			for	(int i = 0; i < eatenApplesCount; ++i)  {
				int appleIndex = (i + eatenApplesFirstIndex) % 64;
				eatenApples[appleIndex] += applesStep;
			}

			// apple is at end of snake, consume it and add body part
			if (eatenApplesCount > 0) {
				if (eatenApples[eatenApplesFirstIndex] > bodyPartCount + 1) {
					addBodyPart();
					eatenApplesFirstIndex = (eatenApplesFirstIndex + 1) % 64;
					--eatenApplesCount;
				}
			}

			// recount body parts sizes
			for	(int i = 0; i < bodyPartCount; ++i) {
				float closestApple = 10;
				for	(int j = 0; j < eatenApplesCount; ++j)  {
					int appleIndex = (j + eatenApplesFirstIndex) % 64;
					float dist = abs(i - eatenApples[appleIndex]);
					closestApple = std::min(closestApple, dist);
				}

				bodyPartSize[i] = bodySize;

				if (closestApple < 2) {
					float normCoef = 1.0f - closestApple / 2.0f;
					bodyPartSize[i] *= 1 + (float)sin(normCoef * 3.14159 / 2) * 0.6f;
				}
			}

			// move a snake
			for	(int i = 0; i < bodyPartCount; ++i) {

				if (i == 0) {
					bodyPartPosX[i] += snakeStep * (float)cos(angle / 180 * 3.14159);
					bodyPartPosY[i] += snakeStep * (float)sin(angle / 180 * 3.14159);
				}
				else {
					float diffVectX = bodyPartPosX[i] - bodyPartPosX[i - 1];
					float diffVectY = bodyPartPosY[i] - bodyPartPosY[i - 1];

					float distance = (float)sqrt(diffVectX * diffVectX + diffVectY * diffVectY);
					float desiredDistance = (bodyPartSize[i - 1] + bodyPartSize[i]) / 2;

					if (distance < desiredDistance) {
						continue;
					}

					// move in direction of diff vector by bodySize
					float moveVectX = (diffVectX / distance) * desiredDistance;
					float moveVectY = (diffVectY / distance) * desiredDistance;
			
					bodyPartPosX[i] = bodyPartPosX[i - 1] + moveVectX;
					bodyPartPosY[i] = bodyPartPosY[i - 1] + moveVectY;
				}

			}
		}

		void drawSnakeAndApple() {

			int sWid = getScreenWidth();
			int sHei = getScreenHeight();
	
			setColor(1, 0, 0);
			for	(int i = 0; i < bodyPartCount; ++i) {
				float x = bodyPartPosX[i];
				float y = bodyPartPosY[i];

				float size =  bodyPartSize[i] / 2.0f;

				drawCircle(x, y, size);

			}

			if (appleX > 0) {
				setColor(0, 1, 0);
				drawSphere(appleX, appleY, 0, appliSize / 2.0f);
			}
		}

		void checkSnakeCollisions() {

			if (bodyPartCount < 1) {
				return;
			}

			float radius = bodySize / 2.0f;
		
			float headX = bodyPartPosX[0];
			float headY = bodyPartPosY[0];

			if (headX < radius || headY < radius || headX + radius > getScreenWidth() || headY + radius > getScreenHeight()) {
				// snake hits wall
				killSnake();
				return;
			}


			for	(int i = 2; i < bodyPartCount; ++i) {
				float x = bodyPartPosX[i];
				float y = bodyPartPosY[i];

				float dx = headX - x;
				float dy = headY - y;
				float dist = sqrt(dx * dx + dy * dy);
		
				if (dist < bodySize) {
					// snake hits itself
					killSnake();
					return;
				}
			}
	

			float dx = headX - appleX;
			float dy = headY - appleY;

			float appleDistance = (float)sqrt(dx * dx + dy * dy);
			if (appleDistance < appliSize) {
				appleX = -1;
				eatApple();
			}

		}
	
		void advancedSnakeDemo_renderFrame(float lastFrameDuration) {
			
			checkSnakeCollisions();

			if (bodyPartCount == 0) {
				bodyPartPosX[0] = getScreenWidth() / 2.0f;
				bodyPartPosY[0] = getScreenHeight() / 2.0f;
				bodyPartPosX[1] = bodyPartPosX[0] - bodySize;
				bodyPartPosY[1] = bodyPartPosY[0] - bodySize;
				bodyPartCount = 2;
			}

			if (appleX < 0 || appleX + 50 > getScreenWidth() || appleY + 50 > getScreenHeight()) {
				appleX = 50.0f + (rand() % (int)(getScreenWidth() - 100));
				appleY = 50.0f + (rand() % (int)(getScreenHeight() - 100));
			}
	
			if (isKeyDown('a') || isSpecialKeyDown(GLUT_KEY_LEFT)) {
				angle += (180.0f + bodyPartCount * speedInc) * lastFrameDuration;
			}
			if (isKeyDown('d') || isSpecialKeyDown(GLUT_KEY_RIGHT)) {
				angle -= (180.0f + bodyPartCount * speedInc) * lastFrameDuration;
			}

			// cheat key
			if (keyPressed('+')) {
				addBodyPart();
			}
	
	
			moveSnakeAndEatenApples(lastFrameDuration);
			drawSnakeAndApple();

			setColor(1, 1, 1);
			drawText(10, 10, 0, std::to_string((long long int)bodyPartCount));
		}
	
		void advancedSnakeDemo_processCgtInOut() {

		}
	}

	void runAdvancedSnakeDemo(int argc, char** argv, const std::string& title) {
		initializeCgt215(argc, argv, advSnakeDemo::advancedSnakeDemo_processCgtInOut, advSnakeDemo::advancedSnakeDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}

}
	

////////////////////////////////////////////////////////////////////////////////
/// Full featured snake demo
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {
	
	namespace fullSnakeDemo{

		
		float bodyPartPosX[1024];
		float bodyPartPosY[1024];
		float bodyPartSize[1024];
		int bodyPartCount = 0;

		float obstaclePosX[128];
		float obstaclePosY[128];
		float obstacleSize[128];
		int obstaclesCount = 0;

		float eatingApples[64];
		int eatingApplesFirstIndex = 0;
		int eatingApplesCount = 0;

		int eatenApplesCount = 0;


		float appleX = -1;
		float appleY = -1;

		float angle = 0.0f;
		float speed = 120.0f;
		float turningSpeed = 200.0f;
		float headSize = 25.0f;
		float bodySize = 20.0f;
		float appleSize = 30.0f;

		float speedInc = 0.0f;

		bool gamePaused = false;

		bool snakeBorning = false;
		float snakeBorningTime = 1.0f;
		float snakeBorningActualTime = 0.0f;

		bool snakeDying = false;
		float snakeDyingFlashTime = 0.1f;
		float snakeDyingTime = 1.0f;
		float snakeDyingActualTime = 0.0f;

		float lastCollisionAmount = 0.0f;

		bool invertColors = false;
		float darkenMult = 1.0;

		bool showDebugInfo = true;

		bool godMode = false;


		void setColorControlled(float r, float g, float b) {
			if (invertColors) {
				r = 1 - r;
				g = 1 - g;
				b = 1 - b;
			}

			r *= darkenMult;
			g *= darkenMult;
			b *= darkenMult;

			setColor(r, g, b);
		}

		float getDistToApple(float x, float y) {
			float dx = x - appleX;
			float dy = y - appleY;
			return (float)sqrt(dx * dx + dy * dy);
		}

		float getDistToObstacle(int obstacleIndex, float x, float y) {
			float dx = x - obstaclePosX[obstacleIndex];
			float dy = y - obstaclePosY[obstacleIndex];
			return (float)sqrt(dx * dx + dy * dy);
		}

		void resetGame() {
			eatenApplesCount = 0;
			eatingApplesCount = 0;
			obstaclesCount = 0;

			godMode = false;

			angle = 0;

			bodyPartPosX[0] = getScreenWidth() / 2.0f;
			bodyPartPosY[0] = getScreenHeight() / 2.0f;
			bodyPartSize[0] = headSize;

			bodyPartPosX[1] = bodyPartPosX[0] - (bodySize + headSize) / 2.0f;
			bodyPartPosY[1] = bodyPartPosY[0];
			bodyPartSize[1] = bodySize;

			bodyPartCount = 2;

			snakeBorning = true;
			snakeBorningActualTime = 0.0f;
			darkenMult = 0.0f;
		}

		void killSnake() {
			snakeDying = true;
			snakeDyingActualTime = 0.0f;
		}

		void addObstacle() {
	
			for (int tryNum = 0; tryNum < 3; ++tryNum) {
				float x = (float)(rand() % getScreenWidth());
				float y = (float)(rand() % getScreenHeight());
				float size = 10.0f + (rand() % ((obstaclesCount + 1) * 10));

				// cehck if new obstacle is not on apple
				if (getDistToApple(x, y) < (appleSize + size) / 2.0f) {
					continue;
				}

				for	(int i = 0; i < bodyPartCount; ++i) {
					float dx = x - bodyPartPosX[i];
					float dy = y - bodyPartPosY[i];
					float dist = (float)sqrt(dx * dx + dy * dy);
			
					if (dist < bodyPartSize[i] + size) {
						goto continueObstacleSearch;
					}
				}
		
				// avoid placing two obstacles too close to each other
				for	(int o = 0; o < obstaclesCount; ++o) {
					if (getDistToObstacle(o, x, y) < obstacleSize[o] + size) {
						goto continueObstacleSearch;
					}
				}

				obstaclePosX[obstaclesCount] = x;
				obstaclePosY[obstaclesCount] = y;
				obstacleSize[obstaclesCount] = size;
				++obstaclesCount;
				return;

		continueObstacleSearch:
				continue;
			};
	
		}

		void eatApple() {
			eatingApples[eatingApplesFirstIndex + eatingApplesCount] = -1;
			++eatingApplesCount;
			++eatenApplesCount;

			//if (eatenApplesCount % 2 == 0) {
				addObstacle();
			//}
		}

		void addBodyPart() {
			bodyPartPosX[bodyPartCount] = bodyPartPosX[bodyPartCount - 1];
			bodyPartPosY[bodyPartCount] = bodyPartPosY[bodyPartCount - 1];
		
			++bodyPartCount;
		}

		void snapBodyPartToPreviousOne(int bodyPartIndex) {
			float diffVectX = bodyPartPosX[bodyPartIndex] - bodyPartPosX[bodyPartIndex - 1];
			float diffVectY = bodyPartPosY[bodyPartIndex] - bodyPartPosY[bodyPartIndex - 1];

			float distance = (float)sqrt(diffVectX * diffVectX + diffVectY * diffVectY);
			float desiredDistance = (bodyPartSize[bodyPartIndex - 1] + bodyPartSize[bodyPartIndex]) / 2.0f;

			if (distance < desiredDistance) {
				return;
			}

			// move in direction of diff vector
			float moveVectX = (diffVectX / distance) * desiredDistance;
			float moveVectY = (diffVectY / distance) * desiredDistance;
			
			bodyPartPosX[bodyPartIndex] = bodyPartPosX[bodyPartIndex - 1] + moveVectX;
			bodyPartPosY[bodyPartIndex] = bodyPartPosY[bodyPartIndex - 1] + moveVectY;
		}

		// moves body part from obstacle and returns distance how much body part was colliding
		float moveBodypartFromObstacle(int bodyPartIndex, int obstacleIndex) {

			float diffVectX = bodyPartPosX[bodyPartIndex] - obstaclePosX[obstacleIndex];
			float diffVectY = bodyPartPosY[bodyPartIndex] - obstaclePosY[obstacleIndex];

			float distance = (float)sqrt(diffVectX * diffVectX + diffVectY * diffVectY);
			float minDistance = (bodyPartSize[bodyPartIndex] + obstacleSize[obstacleIndex]) / 2.0f;
	
			if (distance >= minDistance) {
				return 0.0f;
			}

			// move in direction of diff vector
			float moveVectX = (diffVectX / distance) * minDistance;
			float moveVectY = (diffVectY / distance) * minDistance;
			
			bodyPartPosX[bodyPartIndex] = obstaclePosX[obstacleIndex] + moveVectX;
			bodyPartPosY[bodyPartIndex] = obstaclePosY[obstacleIndex] + moveVectY;

			return minDistance - distance;
		}

		void moveSnakeAndEatingApples(float lastFrameDuration) {

			float frictionMult = 1.0f / (1.0f + lastCollisionAmount / 10);

			float snakeStep = (speed + bodyPartCount * speedInc) * lastFrameDuration * frictionMult;
			float applesStep = snakeStep / bodySize;

			// move apples (inside a snake)
			for	(int i = 0; i < eatingApplesCount; ++i)  {
				int appleIndex = (i + eatingApplesFirstIndex) % 64;
				eatingApples[appleIndex] += applesStep;
			}

			// apple is at end of snake, consume it and add body part
			if (eatingApplesCount > 0) {
				if (eatingApples[eatingApplesFirstIndex] > bodyPartCount + 1) {
					addBodyPart();
					eatingApplesFirstIndex = (eatingApplesFirstIndex + 1) % 64;
					--eatingApplesCount;
				}
			}

			// recount body parts sizes
			for	(int i = 0; i < bodyPartCount; ++i) {
				float closestApple = 10;
				for	(int j = 0; j < eatingApplesCount; ++j)  {
					int appleIndex = (j + eatingApplesFirstIndex) % 64;
					float dist = abs(i - eatingApples[appleIndex]);
					closestApple = std::min(closestApple, dist);
				}

				bodyPartSize[i] = i == 0 ? headSize : bodySize;

				if (closestApple < 2) {
					float normCoef = 1.0f - closestApple / 2.0f;
					bodyPartSize[i] *= 1 + (float)sin(normCoef * 3.14159 / 2) * 0.6f;
				}
			}

			lastCollisionAmount = 0.0f;
			// move a snake
			for	(int i = 0; i < bodyPartCount; ++i) {

				if (i == 0) {
					bodyPartPosX[i] += snakeStep * (float)cos(angle / 180 * 3.14159);
					bodyPartPosY[i] += snakeStep * (float)sin(angle / 180 * 3.14159);

					if (godMode) {
						for	(int o = 0; o < obstaclesCount; ++o) {
							lastCollisionAmount += moveBodypartFromObstacle(0, o);
						}
					}
				}
				else {
					snapBodyPartToPreviousOne(i);

					for	(int o = 0; o < obstaclesCount; ++o) {
						float col = moveBodypartFromObstacle(i, o);
						lastCollisionAmount += col;
				
						for (int x = 0; x < 3 && col > 0.1f; ++x) {
							snapBodyPartToPreviousOne(i);
							col = moveBodypartFromObstacle(i, o);
							lastCollisionAmount += col;
						}
					}
			
					snapBodyPartToPreviousOne(i);
				}

			}
		}

		void drawSnakeAndApple() {

			int sWid = getScreenWidth();
			int sHei = getScreenHeight();
	
			if (godMode) {
				setColorControlled(1, 0, 1);
			}
			else {
				setColorControlled(1, 0, 0);
			}

			for	(int i = 0; i < bodyPartCount; ++i) {
				drawCircle(bodyPartPosX[i], bodyPartPosY[i], bodyPartSize[i] / 2.0f);
			}

			if (appleX > 0) {
				setColorControlled(0, 1, 0);
				drawSphere(appleX, appleY, 0, appleSize / 2.0f);
			}
	
			setColorControlled(1, 1, 1);
			for	(int i = 0; i < obstaclesCount; ++i) {
				drawCircle(obstaclePosX[i], obstaclePosY[i], obstacleSize[i] / 2.0f);
			}
		}

		void checkApplePos() {

			for (int tryNum = 0; tryNum < 3; ++tryNum) {
				bool isInObstacle = false;
				for	(int o = 0; o < obstaclesCount; ++o) {
					if (getDistToObstacle(o, appleX, appleY) < (obstacleSize[o] + appleSize) / 2.0f) {
						isInObstacle = true;
						break;
					}
				}

				if (isInObstacle || appleX < 0 || appleX + 50 > getScreenWidth() || appleY + 50 > getScreenHeight()) {
					appleX = 50.0f + (rand() % (int)(getScreenWidth() - 100));
					appleY = 50.0f + (rand() % (int)(getScreenHeight() - 100));
				}
				else {
					return;  // apple is OK
				}
			}
	
			// hide apple since we failed to position it somewhere
			appleX = -1;
			appleY = -1;

		}

		void checkSnakeCollisions() {

			if (bodyPartCount < 1) {
				return;
			}
		
			float headX = bodyPartPosX[0];
			float headY = bodyPartPosY[0];

			if (!godMode) {
				// headSize is used fo collision instead of bodySize[0] --
				// do not take into account growth of head when eaten an appale
				float radius = (headSize - 2) / 2.0f;

				if (headX < radius || headY < radius || headX + radius > getScreenWidth() || headY + radius > getScreenHeight()) {
					// snake hits wall
					killSnake();
					return;
				}


				for	(int i = 2; i < bodyPartCount; ++i) {
					float x = bodyPartPosX[i];
					float y = bodyPartPosY[i];

					float dx = headX - x;
					float dy = headY - y;
					float dist = sqrt(dx * dx + dy * dy);
		
					if (dist < (headSize + bodyPartSize[i] - 2) / 2.0f) {
						// snake hits itself
						killSnake();
						return;
					}
				}
	

				for	(int o = 0; o < obstaclesCount; ++o) {
					if (getDistToObstacle(o, headX, headY) < (obstacleSize[o] + headSize - 2) / 2.0f) {
						// snake hits an obstacle
						killSnake();
						return;
					}
				}
	
			}

			if (getDistToApple(headX, headY) < (appleSize + headSize - 2) / 2.0f) {
				appleX = -1;
				eatApple();
			}

		}

		void handleControls(float lastFrameDuration) {
			if (isKeyDown('a') || isSpecialKeyDown(GLUT_KEY_LEFT)) {
				angle += (turningSpeed + (bodyPartCount * speedInc) / 2) * lastFrameDuration;
			}
			if (isKeyDown('d') || isSpecialKeyDown(GLUT_KEY_RIGHT)) {
				angle -= (turningSpeed + (bodyPartCount * speedInc) / 2) * lastFrameDuration;
			}

			if (keyPressed('+')) {
				addBodyPart();
			}

			if (keyPressed('x')) {
				showDebugInfo ^= true;
			}

			// cheat mode
			if (keyPressed('g')) {
				godMode ^= true;
			}
		}

		void handleAnimations(float lastFrameDuration) {
	
			if (snakeDying) {
				invertColors = snakeDyingActualTime < snakeDyingFlashTime;
	
				if (snakeDyingActualTime > snakeDyingTime) {
					snakeDying = false;
					resetGame();
					darkenMult = 0.0f;
				}
				else {
					darkenMult = 1.0f - (snakeDyingActualTime / snakeDyingTime);
				}

				snakeDyingActualTime += lastFrameDuration;
			}

			if (snakeBorning) {
				if (snakeBorningActualTime > snakeBorningTime) {
					snakeBorning = false;
					darkenMult = 1.0f;
				}
				else {
					darkenMult = snakeBorningActualTime / snakeBorningTime;
				}

				snakeBorningActualTime += lastFrameDuration;
			}
	

		}

		void drawBackground() {
			setColorControlled(0, 0, 0);
			glBegin(GL_QUADS);
				glVertex3f(0, 0, -1);
				glVertex3f((float)getScreenWidth(), 0, -1);
				glVertex3f((float)getScreenWidth(), (float)getScreenHeight(), -1);
				glVertex3f(0, (float)getScreenHeight(), -1);
			glEnd();
		}

		void debugInfo() {
			setColor(1, 1, 1);

			std::stringstream ss;
			ss << "Colision amount: " << lastCollisionAmount << "\n";

			drawText(20, getScreenHeight() - 20, 0, ss.str());
		}
	

		void fullSnakeDemo_renderFrame(float lastFrameDuration) {
			if (keyPressed('p')){
				gamePaused ^= true;
			}

			if (snakeDying || snakeBorning) {
				handleAnimations(lastFrameDuration);
			}
			else if (!gamePaused) {

				checkSnakeCollisions();

				checkApplePos();
				handleControls(lastFrameDuration);

				moveSnakeAndEatingApples(lastFrameDuration);
			}

			drawBackground();

			drawSnakeAndApple();
	
			if (showDebugInfo) {
				debugInfo();
			}
			drawText(10, 10, 0, std::to_string((long long int)bodyPartCount));
		}
	
		void fullSnakeDemo_processCgtInOut() {

		}

	}

	void runFullSnakeDemo(int argc, char** argv, const std::string& title) {
		fullSnakeDemo::resetGame();
		initializeCgt215(argc, argv, fullSnakeDemo::fullSnakeDemo_processCgtInOut, fullSnakeDemo::fullSnakeDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}

}


////////////////////////////////////////////////////////////////////////////////
/// Empty demo template -- for internal use
////////////////////////////////////////////////////////////////////////////////
namespace cgt215 {
	
	namespace putDemoNameHereDemo {

		// global variables go here

	
		void putDemoNameHereDemo_renderFrame(float lastFrameDuration) {

		}
	
		void putDemoNameHereDemo_processCgtInOut() {

		}
	}

	void runPutDemoNameHereDemo(int argc, char** argv, const std::string& title) {

		// demo initialization goes here

		initializeCgt215(argc, argv, putDemoNameHereDemo::putDemoNameHereDemo_processCgtInOut, putDemoNameHereDemo::putDemoNameHereDemo_renderFrame, title);
		exit(EXIT_SUCCESS);
	}

}