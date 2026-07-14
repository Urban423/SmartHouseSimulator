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
	inline void showCursor(const bool show) { 
		if(isVisible == show) return; 
		isVisible = show; 
		applyCursor(); 
	}
    inline Vector2 getCursorPosition() { 
		POINT p;
		GetCursorPos(&p);
		return Vector2((float)p.x, (float)p.y); 
	}
    inline void setCursor(const CursorType cursorType) {
		if(current == cursorType) return;
		current = cursorType;
        applyCursor();
	}
private:
	void applyCursor() {
		if (!isVisible) {
            SetCursor(noneCursor);
            return;
        }
		switch(current) {
			case CursorType::Arrow:
				SetCursor(arrowCursor);
				break;

			case CursorType::Hand:
				SetCursor(handCursor);
				break;

			case CursorType::Text:
				SetCursor(beamCursor);
				break;

			case CursorType::None:
				SetCursor(noneCursor);
				break;
		}
	}
private:
	bool isVisible;
	CursorType current = CursorType::UNIDENTIFIED;
	HCURSOR arrowCursor;
	HCURSOR handCursor;
	HCURSOR beamCursor;
	HCURSOR noneCursor;
};




class keyBoard: public IKeyBoard {
public:
	void getKeyboardState(unsigned char* keyboard) { if(GetKeyboardState(keyboard) == 0) {} };
};