#pragma once
#include <windows.h>
#include "umath.h"
#include "Rect.h"
#include "IOSystem.h"

typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);

class Window: public IWindow {
public:
	~Window();
	void create(const char* windowName, int width, int height, bool fullscreen, bool vsync);
	void close();
	
	Rect getInnerSize();
	std::pair<int, int> getScreenSize();
	void setFullscreen(const bool state);
	void setSize(int width, int height);
	void setPosition(int x, int y);
	std::pair<int, int> getSize();
	
    inline Vector2 screenToClient(Vector2 pos) { POINT p; p.x = (LONG)pos.x; p.y = (LONG)pos.y; ScreenToClient(_hwnd, &p); return {(float)p.x, (float)p.y}; }
	inline bool focus() 					{ return GetForegroundWindow() == _hwnd; }
	inline void swapBuffers() 				{ SwapBuffers(hDC); }
	inline void setVSync(const bool vsync) 	{ 
		this->vsync = vsync;  
		if (wglSwapIntervalEXT) wglSwapIntervalEXT(vsync ? 1 : 0);
	}
	inline bool getVSync() 					{ return vsync; }
	inline bool isRunning() 				{ return _running; }
	inline void onDestroy() 				{ _running = false; };
private:
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
	
	HGLRC hRC;
	HWND _hwnd; 
	HDC hDC;
	bool vsync = true;
	bool _running = true;
    bool windowRectSaved = false;
    bool wasFullscreen = false;
};