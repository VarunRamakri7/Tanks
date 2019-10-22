/*
 * Copyright 2013
 *     Marek Fiser <mfiser@purdue.edu>
 */
#include <time.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include "cgt215.h"
#define REGULATE_FPS

namespace cgt215 {
	
	GraphicsReader cgtin;
	std::stringstream cgtout;

	int screenWidth = 800;
	int screenHeight = 800;

	int is3dEnabled = false;
	bool wireframe = true;

	// height of line of text
	const int lineHeight = 20;
	int stdTextScrollOffset = 0;
	
	
	int trackedKeysCount = 512;

	bool* keys = new bool[trackedKeysCount];
	bool* oldKeys = new bool[trackedKeysCount];

	int mouseX = 0;
	int mouseY = 0;
	int mouseButtonState = 0;

	std::function<void(float frameTime)> userDisplayFunc;
	std::function<void()> userMainFunc;
		
	DWORD mainThreadId;

	int lastFrameTimestamp;
	float maxFrameTime = 0.1f;
	
	// =========================================================================
	// Structs and helper classes

	class Mutex {

	protected:
		HANDLE m_mutexHandle;

	public:
		Mutex() {
			m_mutexHandle = CreateMutex( 
				NULL,  // default security attributes
				FALSE,  // initially not owned
				NULL);  // unnamed mutex
		}
		~Mutex() {
			CloseHandle(m_mutexHandle);
		}

		void lock() {
			WaitForSingleObject(m_mutexHandle, INFINITE);
		}

		void unlock() {
			ReleaseMutex(m_mutexHandle);
		}

	};
	
	struct ColorRgb {
		float r;
		float g;
		float b;

		ColorRgb() : r(0.0f), g(0.0f), b(0.0f) {}
		ColorRgb(float red, float green, float blue) : r(red), g(green), b(blue) { }
	};

	struct Vector2f {
		float x;
		float y;

		Vector2f() : x(0.0f), y(0.0f) {}
		Vector2f(float xCoord, float yCoord) : x(xCoord), y(yCoord) {}
	};

	struct Vector3f {
		float x;
		float y;
		float z;
		
		Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
		Vector3f(Vector2f v) : x(v.x), y(v.y), z(0.0f) {}
		Vector3f(float xCoord, float yCoord, float zCoord) : x(xCoord), y(yCoord), z(zCoord) {}
	};

	struct Line2D {
		Vector2f p1;
		Vector2f p2;

		ColorRgb color;

		Line2D(Vector2f pt1, Vector2f pt2, ColorRgb clr) : p1(pt1), p2(pt2), color(clr) {}
	};

	struct Sphere {
		Vector3f center;
		float radius;
		bool is3d;

		ColorRgb color;
		
		Sphere(Vector2f c, float r, ColorRgb clr) : center(Vector3f(c)), radius(r), color(clr), is3d(false) {}
		Sphere(Vector3f c, float r, ColorRgb clr) : center(c), radius(r), color(clr), is3d(true) {}
	};
	
	struct Rectangle2D {
		float leftTopX;
		float leftTopY;
		float width;
		float height;
		
		ColorRgb color;

		Rectangle2D() {}
	};
	
	Mutex threadSyncMutex;

	int oldMouseX;
	int oldMouseY;
	Vector3f camTranslation = Vector3f(0, 0, -600);
	Vector2f camRotation = Vector2f(0, 0);

	ColorRgb currentUserThreadColor = ColorRgb(1, 1, 1);
	ColorRgb cgtTextColor = ColorRgb(1, 1, 1);

	std::vector<Line2D> lines;
	std::vector<Sphere> spheres;
	std::vector<Rectangle2D> rectangles;

	// =========================================================================
	// Implementation of GraphicsReader class
	GraphicsReader::GraphicsReader() : m_readMode(false), m_allowedThreadId(-1) {

		m_ghReadingEvent = CreateEvent( 
			NULL,  // default security attributes
			FALSE,  // automatic-reset event
			FALSE,  // initial state is nonsignaled
			NULL  // object name
		); 

		assert(m_ghReadingEvent != NULL);
	}

	GraphicsReader::~GraphicsReader() {
		CloseHandle(m_ghReadingEvent);
	}
	 
	void GraphicsReader::writeToReadBuffer(unsigned char c) {
		switch (c) {
		case '\b': {
			std::string str = m_ss.str();
			m_ss = std::stringstream();
			m_ss << str.substr(0, str.length() - 1);
			break;
		}
		case 27:
			m_ss = std::stringstream();
			break;
		default:
			m_ss << c;
			break;
		}
	}

	void GraphicsReader::readingFinished() {
		m_readMode = false;
		SetEvent(m_ghReadingEvent);  // wake up thread which is waiting for read operation
	}

	/// Returns true if calling thread is user's program function.
	/// Returns false if calling thread is main thread (where glut is drawing).
	bool isUserProgramThread() {
		return mainThreadId != GetCurrentThreadId();
	}
	/// Returns true if calling thread is user's program function. It also lockes the synchronization mutex.
	/// Returns false if calling thread is main thread (where glut is drawing).
	bool lockIfInUserProgramThread() {
		if (mainThreadId != GetCurrentThreadId()) {
			threadSyncMutex.lock();
			return true;
		}
		else {
			return false;
		}
	}

	// =========================================================================
	// private cgt215 helper functions

	float clamp(float f) {
		if (f < 0) {
			return 0.0f;
		}
		else if (f > 1) {
			return 1.0f;
		} 
		else {
			return f;
		}
	}
	
	double clamp(double f) {
		if (f < 0) {
			return 0.0;
		}
		else if (f > 1) {
			return 1.0;
		} 
		else {
			return f;
		}
	}

	int countLines(const std::string& str) {
		int lineCount = 1;
		for	(size_t i = 0; i < str.length(); ++i) {
			if (str[i] == '\n') {
				++lineCount;
			}
		}
		return lineCount;
	}

	void setGlColor(const ColorRgb& color) {
		glColor3f(color.r, color.g, color.b);
	}

	void drawCgtText() {
		setGlColor(cgtTextColor);

		const std::string str = cgtout.str() + cgtin.getReadBuffer().str();
		int lineCount = countLines(str);

		int startY = screenHeight - std::min(lineHeight, screenHeight - lineCount * lineHeight);
		
		drawText(10, startY, str);

		if (cgtin.isInReadMode()) {
			time_t t = time(NULL);
			if (t % 2 == 0) {
				glutBitmapCharacter(GLUT_BITMAP_9_BY_15, '_');
			}
		}

		if (startY < lineHeight) {
			// draw scroll bar?
		}
	}
	
	void drawGlLine(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2) {
		glBegin(GL_LINES);
		glVertex3f(x1, y1, (GLfloat)0.0);
		glVertex3f(x2, y2, (GLfloat)0.0);
		glEnd();
	}

	void drawGlCircle(GLfloat x, GLfloat y, GLfloat r) {
		glBegin(GL_POLYGON);
		int segSize = 32;
		for (int i = 0; i <= segSize; i++) {
			double t = 2.0 * 3.14592653589 * (double)i / (double)segSize;
			GLfloat sint = (GLfloat)sin(t);
			GLfloat cost = (GLfloat)cos(t);
			glVertex3f(x + r * cost, y + r * sint, (GLfloat)(0.0));
		}
		glEnd();
	}

	void drawGlRect(GLfloat x, GLfloat y, GLfloat width, GLfloat height) {
		glBegin(GL_POLYGON);
		glVertex3f(x, y, (GLfloat)0.0);
		glVertex3f(x + width, y, (GLfloat)0.0);
		glVertex3f(x + width, y + height, (GLfloat)0.0);
		glVertex3f(x, y + height, (GLfloat)0.0);
		glEnd();
	}

	void drawGlSphere(GLdouble x, GLdouble y,  GLdouble z, GLdouble r) {
		glPushMatrix();
		glTranslated(x, y, z);
		glutSolidSphere(r, 32, 32);
		glPopMatrix(); 
	}
	
	void drawSavedShapes() {
		
		// 3D shapes
		enable3d();

		// spheres
		for	(auto it = spheres.begin(); it != spheres.end(); ++it) {
			const Sphere& s = *it;
			if (!s.is3d) {
				continue;
			}
			setGlColor(s.color);
			drawGlSphere(s.center.x, s.center.y, s.center.z, s.radius);
		}

		disable3d();

		// 2D shapes
		// lines
		for	(auto it = lines.begin(); it != lines.end(); ++it) {
			const Line2D& l = *it;
			setGlColor(l.color);
			drawGlLine(l.p1.x, l.p1.y, l.p2.x, l.p2.y);
		}
		
		//rectangles
		for	(auto it = rectangles.begin(); it != rectangles.end(); ++it) {
			const Rectangle2D& r = *it;
			setGlColor(r.color);
			drawGlRect(r.leftTopX, r.leftTopY, r.width, r.height);
		}

		// circles
		for	(auto it = spheres.begin(); it != spheres.end(); ++it) {
			const Sphere& s = *it;
			if (s.is3d) {
				continue;
			}
			setGlColor(s.color);
			drawGlCircle(s.center.x, s.center.y, s.radius);
		}
	}
	
	void setup3dView() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, (GLdouble)screenWidth / (GLdouble)screenHeight, 0.1, 1000.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glViewport(0, 0, screenWidth, screenHeight);
		//gluLookAt(eye.x, eye.y, eye.z,
		//	0.0, 0.0, 0.0,
		//	0.0, 1.0, 0.0);
		glTranslatef(camTranslation.x, camTranslation.y, camTranslation.z);
		glRotatef(camRotation.x, 1.0, 0.0, 0.0);
		glRotatef(camRotation.y, 0.0, 1.0, 0.0);

		glEnable(GL_DEPTH_TEST);
		//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);		
		glEnable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
	}

	void setup2dView() {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, screenWidth, 0, screenHeight, -500.0, 500.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
	}

	void setupVeiw() {
		if (is3dEnabled) {
			setup3dView();
		}
		else {
			setup2dView();
		}
	}

	// =========================================================================
	// public cgt215 members implementation

	void enable3d() {
		if (is3dEnabled) {
			return;
		}

		is3dEnabled = true;
		setup3dView();
	}

	void disable3d() {
		if (!is3dEnabled) {
			return;
		}

		is3dEnabled = false;
		setup2dView();
	}
	
	void setWireframeRendering(bool enabled) {
		wireframe = enabled;
		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);
	}


	void clearShapes() {
		bool locked = lockIfInUserProgramThread();

		lines.clear();
		spheres.clear();
		rectangles.clear();

		if (locked) {
			threadSyncMutex.unlock();
		}
	}
	
	void drawLine(double x1, double y1, double x2, double y2) {
		if (lockIfInUserProgramThread()) {
			lines.push_back(Line2D(Vector2f((float)x1, (float)y1), Vector2f((float)x2, (float)y2), currentUserThreadColor));
			threadSyncMutex.unlock();
		}
		else {
			drawGlLine((GLfloat)x1, (GLfloat)y1, (GLfloat)x2, (GLfloat)y2);
		}
	}

	void drawCircle(double x, double y, double radius) {
		if (lockIfInUserProgramThread()) {
			spheres.push_back(Sphere(Vector2f((float)x, (float)y), (float)radius, currentUserThreadColor));
			threadSyncMutex.unlock();
		}
		else {
			drawGlCircle((GLfloat)x, (GLfloat)y, (GLfloat)radius);
		}
	}

	void drawRectangle(double x, double y, double width, double height) {
		if (lockIfInUserProgramThread()) {
			rectangles.push_back(Rectangle2D());
			Rectangle2D& r = rectangles.back();
			r.leftTopX = (float)x;
			r.leftTopY = (float)y;
			r.width = (float)width;
			r.height = (float)height;
			r.color = currentUserThreadColor;
			threadSyncMutex.unlock();
		}
		else {
			drawGlRect((GLfloat)x, (GLfloat)y, (GLfloat)width, (GLfloat)height);
		}
	}

	// TODO: Integrate Mesh vector on these!
	void drawTeapot() {
		if (isUserProgramThread()) {
			std::cerr << "Teapot can not be drawn in this scope." << std::endl;
		}
		else {
			glutSolidTeapot(25.0);
		}
	}

	void drawTorus(double innerRadius, double outerRadius) {
		if (isUserProgramThread()) {
			std::cerr << "Torus can not be drawn in this scope." << std::endl;
		}
		else {
			glutSolidTorus(innerRadius, outerRadius, 16, 48);
		}
	}

	void drawCube() {
		if (isUserProgramThread()) {
			std::cerr << "Cube can not be drawn in this scope." << std::endl;
		}
		else {
			glutSolidCube(25.0);
		}
	}

   void drawRectangle(double x, double y, double z, double width, double height, double depth) {
      if (isUserProgramThread()) {
         std::cerr << "Rectangle3d can not be drawn in this scope." << std::endl;
      }
      else {
         // 1 = lower right, front
         // 2 = upper right, front
         // 3 = upper left, front
         // 4 = lower left, front
         // 5 = lower right, back
         // 6 = upper right, back
         // 7 = upper left, back
         // 8 = lower left, back
         GLfloat x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
         GLfloat x5, y5, z5, x6, y6, z6, x7, y7, z7, x8, y8, z8;
         GLfloat half = .5;
         // Front
         x1 = (GLfloat)x + (GLfloat)width * (GLfloat)half;
         y1 = (GLfloat)y - (GLfloat)height * (GLfloat)half;
         z1 = (GLfloat)z - (GLfloat)depth * (GLfloat)half;
         x2 = (GLfloat)x + (GLfloat)width * (GLfloat)half;
         y2 = (GLfloat)y + (GLfloat)height * (GLfloat)half;
         z2 = (GLfloat)z - (GLfloat)depth * (GLfloat)half;
         x3 = (GLfloat)x - (GLfloat)width * (GLfloat)half;
         y3 = (GLfloat)y + (GLfloat)height * (GLfloat)half;
         z3 = (GLfloat)z - (GLfloat)depth * (GLfloat)half;
         x4 = (GLfloat)x - (GLfloat)width * (GLfloat)half;
         y4 = (GLfloat)y - (GLfloat)height * (GLfloat)half;
         z4 = (GLfloat)z - (GLfloat)depth * (GLfloat)half;
         
         // Back
         x5 = (GLfloat)x + (GLfloat)width * (GLfloat)half;
         y5 = (GLfloat)y - (GLfloat)height * (GLfloat)half;
         z5 = (GLfloat)z + (GLfloat)depth * (GLfloat)half;
         x6 = (GLfloat)x + (GLfloat)width * (GLfloat)half;
         y6 = (GLfloat)y + (GLfloat)height * (GLfloat)half;
         z6 = (GLfloat)z + (GLfloat)depth * (GLfloat)half;
         x7 = (GLfloat)x - (GLfloat)width * (GLfloat)half;
         y7 = (GLfloat)y + (GLfloat)height * (GLfloat)half;
         z7 = (GLfloat)z + (GLfloat)depth * (GLfloat)half;
         x8 = (GLfloat)x - (GLfloat)width * (GLfloat)half;
         y8 = (GLfloat)y - (GLfloat)height * (GLfloat)half;
         z8 = (GLfloat)z + (GLfloat)depth * (GLfloat)half;

			// Front face
         glBegin(GL_POLYGON);
			glVertex3f(x1, y1, z1);
         glVertex3f(x2, y2, z2);
         glVertex3f(x3, y3, z3);
         glVertex3f(x4, y4, z4);
         glEnd();

         // Back face
         glBegin(GL_POLYGON);
			glVertex3f(x5, y5, z5);
         glVertex3f(x6, y6, z6);
         glVertex3f(x7, y7, z7);
         glVertex3f(x8, y8, z8);
         glEnd();

         // Left face
			glBegin(GL_POLYGON);
         glVertex3f(x4, y4, z4);
         glVertex3f(x3, y3, z3);
         glVertex3f(x7, y7, z7);
         glVertex3f(x8, y8, z8);
         glEnd();

         // Right face
			glBegin(GL_POLYGON);
         glVertex3f(x5, y5, z5);
         glVertex3f(x6, y6, z6);
         glVertex3f(x2, y2, z2);
         glVertex3f(x1, y1, z1);
         glEnd();

         // Top face
			glBegin(GL_POLYGON);
         glVertex3f(x2, y2, z2);
         glVertex3f(x6, y6, z6);
         glVertex3f(x7, y7, z7);
         glVertex3f(x3, y3, z3);
         glEnd();

         // Bottom face
			glBegin(GL_POLYGON);
         glVertex3f(x8, y8, z8);
         glVertex3f(x4, y4, z4);
         glVertex3f(x1, y1, z1);
         glVertex3f(x5, y5, z5);
         glEnd();
      }
   }

	void drawCone() {
		if (isUserProgramThread()) {
			std::cerr << "Cone can not be drawn in this scope." << std::endl;
		}
		else {
			glPushMatrix();
			glRotatef(-90.0,1.0,0.0,0.0);
			glutSolidCone(25,75,20,10);
			glPopMatrix();
		}
	}

	void drawTriangle() {
		if (isUserProgramThread()) {
			std::cerr << "Triangle can not be drawn in this scope." << std::endl;
		}
		else {
			float halfWidth = 25.0f;
			glBegin(GL_POLYGON);
			glVertex3f(0.0, halfWidth, 0.0);
			glVertex3f(halfWidth, -halfWidth, 0.0);
			glVertex3f(-halfWidth, -halfWidth, 0.0);
			glEnd();
		}
	}

	// TODO: Integrate Mesh vector on these!
	void drawSphere(double x, double y, double z, double radius) {
		if (lockIfInUserProgramThread()) {
			spheres.push_back(Sphere(Vector3f((float)x, (float)y, (float)z), (float)radius, currentUserThreadColor));
			threadSyncMutex.unlock();
		}
		else {
			drawGlSphere(x, y, z, radius);
		}
	}

	
	void clearText() {
		bool locked = lockIfInUserProgramThread();

		cgtout = std::stringstream();

		if (locked) {
			threadSyncMutex.unlock();
		}
	}

	void drawText(double x, double y, double z, const std::string& text) {
		glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);
		int line = 0;
		for (size_t i = 0; i < text.size(); i++) {
			if (text[i] == '\n') {
				++line;
				glRasterPos3f((GLfloat)x, (GLfloat)(y - line * lineHeight), (GLfloat)z);
				continue;
			}
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
		}
	}
	
	void setColor(double r, double g, double b) {
		if (lockIfInUserProgramThread()) {
			currentUserThreadColor.r = (float)clamp(r);
			currentUserThreadColor.g = (float)clamp(g);
			currentUserThreadColor.b = (float)clamp(b);
			threadSyncMutex.unlock();
		}
		else {
			glColor3f(clamp((GLfloat)r), clamp((GLfloat)g), clamp((GLfloat)b));
		}
	}
	void setCgtTextColor(double r, double g, double b) {
		cgtTextColor.r = (float)clamp(r);
		cgtTextColor.g = (float)clamp(g);
		cgtTextColor.b = (float)clamp(b);
	}

	void setBackgroundColor(double r, double g, double b) {
		glClearColor(clamp((GLfloat)r), clamp((GLfloat)g), clamp((GLfloat)b), (GLfloat)0.0);
	}

	bool isLeftMouseButtonPressed() {
		return (mouseButtonState & (1 << GLUT_LEFT_BUTTON)) != 0;
	}
	bool isRightMouseButtonPressed() {
		return (mouseButtonState & (1 << GLUT_RIGHT_BUTTON)) != 0;
	}

	int getMouseX() {
		return mouseX;
	}
	int getMouseY() {
		return screenHeight - mouseY;
	}
	
	int getScreenWidth() {
		return screenWidth;
	}
	int getScreenHeight() {
		return screenHeight;
	}
		
	bool isKeyDown(int key) {
		if (key < trackedKeysCount) {
			return keys[key];
		}
		else {
			return false;
		}
	}

	bool keyPressed(int key) {
		if (key < trackedKeysCount) {
			return keys[key] && !oldKeys[key];
		}
		else {
			return false;
		}
	}
	
	bool keyReleased(int key) {
		if (key < trackedKeysCount) {
			return !keys[key] && oldKeys[key];
		}
		else {
			return false;
		}
	}
	
	// =========================================================================
	// Glut events
	
	void glutDisplay() {
		threadSyncMutex.lock();

		int time = glutGet(GLUT_ELAPSED_TIME);
		float frameTime = (time - lastFrameTimestamp) / 1000.0f;
		assert(frameTime >= 0);
		lastFrameTimestamp = time;
		//wireframe = true;

		if (frameTime > maxFrameTime) {
			frameTime = maxFrameTime;
		}
		
		// clear window
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, wireframe? GL_LINE : GL_FILL);

		drawSavedShapes();
		
		// switch matrix mode to modelview and push new matrix in case that user will mess up with it
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		userDisplayFunc(frameTime);
		glPopMatrix();

		disable3d();
		drawCgtText();

		for (int i = 0; i < trackedKeysCount;++i) {
			oldKeys[i] = keys[i];
		}

		glutSwapBuffers();
		threadSyncMutex.unlock();
	}

	void glutResize(int width, int height) {
		screenWidth = width;
		screenHeight = height;
		
		glMatrixMode(GL_MODELVIEW);
		glViewport(0, 0, screenWidth, screenHeight);

		setupVeiw();
	}

	void glutKeyDown(unsigned char key, int x, int y) {

		if (cgtin.isInReadMode()) {
			if (key == '\r' || key == '\n') {
				// new line (or return), process given value
				cgtout << cgtin.getReadBuffer().str();
				cgtout << std::endl;
				cgtin.readingFinished();
			}
			else {
				cgtin.writeToReadBuffer(key);
			}
			return;
		}

		keys[key] = true;

		if (key == 27) {
			exit(EXIT_SUCCESS);
		}
	}
	
	void glutKeyUp(unsigned char key, int x, int y) {
		keys[key] = false;
	}
	
	void glutKeySpecial(int key, int x, int y) {
		key += 256;
		assert(key < trackedKeysCount);
		keys[key] = true;
	}

	void glutKeySpecialUp(int key, int x, int y) {
		key += 256;
		assert(key < trackedKeysCount);
		keys[key] = false;
	}
	
	void glutMouse(int button, int state, int x, int y) {
		mouseX = x;
		mouseY = y;

		if (state == GLUT_DOWN) {
			mouseButtonState |= 1 << button;
		}
		else {
			mouseButtonState &= ~(1 << button);
		}
	}
	
	void glutMouseMove(int x, int y) {
		mouseX = x;
		mouseY= y;

		float dx = (float)(x - oldMouseX);
		float dy = (float)(y - oldMouseY);
		
		if (mouseButtonState == (1 << GLUT_LEFT_BUTTON)) {
			camRotation.x += dy * 0.2f;
			camRotation.y += dx * 0.2f;
		}
		else if (mouseButtonState == (1 << GLUT_MIDDLE_BUTTON)) {
			camTranslation.x += dx;
			camTranslation.y -= dy;
		}
		else if (mouseButtonState == (1 << GLUT_RIGHT_BUTTON)) {
			camTranslation.z += dy;
		}

		oldMouseX = x;
		oldMouseY = y;
	}
	
	void glutIdle() {
		glutPostRedisplay();
	}
	



	DWORD WINAPI mainProgramFunction(LPVOID lpParam) {
		cgtin.setAllowedThread(GetCurrentThreadId());
		userMainFunc();
		return 0; 
	} 

	void initializeCgt215(int argc, char** argv, std::function<void()> mainProgramFunc,
		std::function<void(float frameTime)> displayFunc, const std::string& projName) {
	
		userMainFunc = mainProgramFunc;
		userDisplayFunc = displayFunc;

		for (int i = 0; i < trackedKeysCount; ++i) {
			keys[i] = false;
			oldKeys[i] = false;
		}

		srand((unsigned int)time(NULL));
		glutInit(&argc,argv);
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		//glutInitWindowPosition(10,10);
		glutInitWindowSize(screenWidth, screenHeight);
		glutCreateWindow(projName.c_str());

		glutDisplayFunc(glutDisplay);
		glutReshapeFunc(glutResize);

		glutKeyboardFunc(glutKeyDown);
		glutKeyboardUpFunc(glutKeyUp);
		glutSpecialFunc(glutKeySpecial);
		glutSpecialUpFunc(glutKeySpecialUp);
		//glutIgnoreKeyRepeat(true);

		glutMouseFunc(glutMouse);
		glutMotionFunc(glutMouseMove);
		glutPassiveMotionFunc(glutMouseMove);

		glutIdleFunc(glutIdle);
		
		glShadeModel (GL_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glDepthMask(GL_TRUE);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

		// good old-fashioned fixed function lighting
		float black[]    = { 0.0f, 0.0f, 0.0f, 1.0f };
		float white[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
		float ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
		float diffuse[]  = { 0.9f, 0.9f, 0.9f, 1.0f };
		float lightPos1[] = { 0.0f, 300.0f, 600.0f, 0.0f };
      float lightPos2[] = { 0.0f, 300.0f, -600.0f, 0.0f };
		float lightPos3[] = { 600.0f, 300.0f, 0.0f, 0.0f };
      float lightPos4[] = { -600.0f, 300.0f, 0.0f, 0.0f };

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse); -- comes from color
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);

		glLightfv(GL_LIGHT0, GL_AMBIENT, white);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);

		glLightfv(GL_LIGHT1, GL_AMBIENT, white);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
		glLightfv(GL_LIGHT1, GL_SPECULAR, white);
		glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);

		glLightfv(GL_LIGHT2, GL_AMBIENT, white);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
		glLightfv(GL_LIGHT2, GL_SPECULAR, white);
		glLightfv(GL_LIGHT2, GL_POSITION, lightPos3);

		glLightfv(GL_LIGHT3, GL_AMBIENT, white);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, white);
		glLightfv(GL_LIGHT3, GL_SPECULAR, white);
		glLightfv(GL_LIGHT3, GL_POSITION, lightPos4);

      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);

		glEnable(GL_LIGHT0);
  		glEnable(GL_LIGHT1);
      glEnable(GL_LIGHT2);
      glEnable(GL_LIGHT3);
		glEnable(GL_NORMALIZE);
		
		glutResize(screenWidth, screenHeight);
		setupVeiw();

		lastFrameTimestamp = glutGet(GLUT_ELAPSED_TIME);

		mainThreadId = GetCurrentThreadId();

		// run main program in separate thread
		DWORD dwThreadId;
		HANDLE thread = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			mainProgramFunction,    // thread function name
			0,						// argument to thread function 
			0,                      // use default creation flags 
			&dwThreadId);			// returns the thread identifier 

		glutMainLoop();
	}

}