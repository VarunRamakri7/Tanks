/*
 * Copyright 2013
 *     Marek Fiser <mfiser@purdue.edu>
 */
#pragma once

#include <iostream>
#include <GL/freeglut.h>
#include <functional>
#include <string>
#include <sstream>
#include <iostream>
#include <windows.h>
#include <assert.h>

namespace cgt215 {

	class GraphicsReader {

		protected:
			bool m_readMode;
			HANDLE m_ghReadingEvent; 
			std::stringstream m_ss;
			DWORD m_allowedThreadId;

		public:
	
			GraphicsReader();
			~GraphicsReader();

			template <typename T>
			GraphicsReader& operator>> (T& val) {
				if (m_allowedThreadId != GetCurrentThreadId()) {
					std::cerr << "CgtIn can not read in this scope" << std::endl;
					return *this;
				}

				m_readMode = true;
				// suspend calling thread and wait for read end
				DWORD dwWaitResult = WaitForSingleObject(m_ghReadingEvent, INFINITE);

				m_ss >> val;
				m_ss = std::stringstream();  // reset string stream
				return *this;
			}
	
			inline bool isInReadMode() { return m_readMode; }

			inline const std::stringstream& getReadBuffer() { return m_ss; }
	
			inline void setAllowedThread(DWORD allowedThreadId) { m_allowedThreadId = allowedThreadId; }

			void writeToReadBuffer(unsigned char c);

			void readingFinished();

	};


	extern GraphicsReader cgtin;
	extern std::stringstream cgtout;
	inline void clearCgtText() { cgtout = std::stringstream(); }

	void enable3d();
	void disable3d();
	void setWireframeRendering(bool enabled);

	
	void clearShapes();
	// 2D shapes
	void drawLine(double x1, double y1, double x2, double y2);
	void drawCircle(double x, double y, double radius);
	void drawRectangle(double x, double y, double width, double height);

	// TODO: create drawMesh
	void drawTeapot();
	void drawTorus(double innerRadius, double outerRadius);
	inline void drawTorus() {
		drawTorus(10.0, 50);
	}
	void drawCube();
   void drawRectangle(double x, double y, double z, double width, double height, double depth);
	void drawCone();
	void drawTriangle();

	// 3D shapes
	void drawSphere(double x, double y, double z, double radius);
	inline void drawSphere(double radius) {
		drawSphere(0, 0, 0, radius);
	}

	void clearText();
	void drawText(double x, double y, double z, const std::string& text);
	inline void drawText(double x, double y, const std::string& text) {
		drawText(x, y, 0, text);
	}
	
	void setColor(double r, double g, double b);
	void setCgtTextColor(double r, double g, double b);
	void setBackgroundColor(double r, double g, double b);

	bool isLeftMouseButtonPressed();
	bool isRightMouseButtonPressed();

	int getMouseX();
	int getMouseY();

	int getScreenWidth();
	int getScreenHeight();
	
	/// Returns true if key is down (pressed).
	bool isKeyDown(int key);
	inline bool isSpecialKeyDown(int specialKey) {
		return isKeyDown(256 + specialKey);
	}
	/// Returns true if key was just pressed this frame.
	/// Returns false for frames where state of key is not changed.
	bool keyPressed(int key);
	inline bool specialKeyPressed(int specialKey) {
		return keyPressed(256 + specialKey);
	}
	/// Returns true if key was just released this frame.
	/// Returns false for frames where state of key is not changed.
	bool keyReleased(int key);
	inline bool specialKeyReleased(int specialKey) {
		return keyReleased(256 + specialKey);
	}

	void initializeCgt215(int argc, char** argv, std::function<void()> mainProgramFunc,
		std::function<void(float frameTime)> displayFunc, const std::string& projName);

}
