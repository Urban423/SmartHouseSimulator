#pragma once
#include <Windows.h>
#include <umath.h>
#include "Rect.h"

typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);

class Window
{
public:
	~Window();
	
	//size
	inline Rect getCenter() { return Rect(centerX, centerY); }
	inline Rect getInnerSize() { RECT rc; GetClientRect(_hwnd, &rc); POINT topLeft = {rc.left, rc.top}; POINT bottomRight = {rc.right, rc.bottom}; ClientToScreen(_hwnd, &topLeft); ClientToScreen(_hwnd, &bottomRight); rect = { (float)topLeft.x, (float)topLeft.y, (float)bottomRight.x, (float)bottomRight.y };; return rect; }
	void setPos(float x, float y);
	void setFullscreen(const bool state);
	
	//main
	Rect screenSize();
	void showCursor(const bool show);
	void init(const char* windowName, int width, int height, bool fullscreen);
	void handleInput();
	void setRenderContext();
	void setRenderContextNULL();
	void setSize();
	
	
	inline void 				broadcast() 								{ swapBuffers(); 				}
	inline void 				swapBuffers() 								{ SwapBuffers(hDC); 			}
	inline void 				setVSync(const bool vsync) 					{ this->vsync = vsync; wglSwapIntervalEXT(vsync); }
	inline bool 				getVSync() 									{ return vsync; 				}
	inline void 				setIES(InputEventSystemI* ies) 				{ inputEventSystem = ies; 		}
	inline void 				setApp(AppI* app) 							{ this->app = app; 				}
	inline void 				resizeEnter() 								{ app->enterResizingMode(); 	}
	inline void 				resizeExit() 								{  app->exitResizingMode(); 	}
	inline InputEventSystemI* 	getIES() 									{ return inputEventSystem; 		}
	inline bool 				isRunning() 								{ return _running; 				}
	inline void 				onDestroy() 								{ _running = false; 			};
private:
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;
	InputEventSystemI* inputEventSystem = nullptr;
	AppI* app = nullptr;
	
	HGLRC hRC;
	HWND _hwnd; 
	HDC hDC;
	HCURSOR arrowCursor;
	HCURSOR  noneCursor;
	bool vsync = true;
	bool _running = true;
	
	int screen_width;
	int screen_height;
	Rect rect;
	int centerX;
	int centerY;
};

