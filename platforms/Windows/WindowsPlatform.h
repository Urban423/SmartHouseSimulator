#pragma once
#include "IOSystem.h"
#include "resource.h"
#include <windows.h>
#include <chrono>

class WindowsPlatform: public IPlatform {
public:
	void create();
	void update();

	inline double getTime() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
	inline void setCursorPosition(int x, int y) { SetCursorPos(x, y); }
	inline void showCursor(const bool show) { SetCursor(show? arrowCursor : noneCursor); }
    inline Vector2 getCursorPosition() { 
		POINT p;
		GetCursorPos(&p);
		return Vector2((float)p.x, (float)p.y); 
	}
private:
	HCURSOR arrowCursor;
	HCURSOR  noneCursor;
};




class keyBoard: public IKeyBoard {
public:
	void getKeyboardState(unsigned char* keyboard) { if(GetKeyboardState(keyboard) == 0) {} };
};