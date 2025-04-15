#include "Window.h"
#include <stdio.h>
#include "resource.h"

Window* win;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			win->onDestroy();
			break;
		}
		case WM_ENTERSIZEMOVE: { win->resizeEnter(); break; }
		case WM_EXITSIZEMOVE : { 
			win->resizeExit(); 
			if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED) {
				win->setSize();
			}		
			break;
		}
		case WM_GETMINMAXINFO: {
            MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;
            pMinMax->ptMinTrackSize.x = 400;
            pMinMax->ptMinTrackSize.y = 300;
            return 0;
        }
		case WM_SIZING: { break; };
		case WM_LBUTTONDOWN: {
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);
			if (win->getIES()) win->getIES()->handleMouseClickDown((float)x, (float)y);
			break;
		}
		case WM_LBUTTONUP: {
			int x = (int)(short)LOWORD(lParam);
			int y = (int)(short)HIWORD(lParam);
			if (win->getIES()) win->getIES()->handleMouseClickUp((float)x, (float)y);
			break;
		}
		case WM_ERASEBKGND: return 1;
		case WM_MOVING: { }
		case WM_MOVE: { win->setPos(LOWORD(lParam), HIWORD(lParam)); break; 	}
		case WM_MOUSEWHEEL: { win->getIES()->handleMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); break; }
		//case WM_KEYDOWN: 	{ win->getIES()->handleKeyDown(wParam); break; }
		case WM_CHAR: { win->getIES()->handleKeyDown((wchar_t)wParam); break; }
		default: { return DefWindowProc(hwnd, msg, wParam, lParam); }
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::~Window() { DestroyWindow(_hwnd); }

void Window::init(const char* windowName, int width, int height, bool fullscreen)
{
	win = this;
	screen_width  = GetSystemMetrics(SM_CXSCREEN);
	screen_height = GetSystemMetrics(SM_CYSCREEN);
	arrowCursor = LoadCursor(NULL, IDC_ARROW);
	noneCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_NONECURSOR));
	
	
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
	wc.hInstance = NULL;
	wc.lpszClassName = windowName;
	wc.lpszMenuName = "";
	wc.lpfnWndProc = WndProc;
	wc.style = 0;

	if (!RegisterClassEx(&wc)) {
		onDestroy();
	}

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_SYSMENU, false);

	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		windowName, windowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
		rc.left,  rc.top,
		rc.right, rc.bottom, NULL, NULL, NULL, NULL);

	if (!_hwnd) {
		onDestroy();
	}
	if(fullscreen) {
		setFullscreen(1);
	}

	ShowWindow(_hwnd, SW_SHOW);
	UpdateWindow(_hwnd);


	//OpenGL
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	
	
	int iFormat;
	hDC = GetDC(_hwnd);
	iFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, iFormat, &pfd);
	
	
	//OpenGL
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	setVSync(vsync);
	
	GetClientRect(_hwnd, &rc);
	rect = {(float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom};
	centerX = rect.left + rect.width()  / 2;
	centerY = rect.top  +  rect.height() / 2;
}

void Window::setRenderContextNULL() {
	wglMakeCurrent(NULL, NULL);
}

void Window::setRenderContext() { 
	wglMakeCurrent(hDC, hRC);
}

void Window::handleInput() {
	MSG msg;
	while(PeekMessage(&msg, _hwnd, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_MOUSEMOVE) continue;
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}
}

//windows size block
void Window::setPos(float x, float y)
{
	float width  = rect.width();
	float height = rect.height();
	rect = {x, y, x + width, y + height };
	// centerX = left + width  / 2;
	// centerY = top  + height / 2;
}


void Window::setFullscreen(const bool state)
{
	if(state) {
		LONG l_WinStyle = GetWindowLong (_hwnd, GWL_STYLE);
		SetWindowLong(_hwnd, GWL_STYLE,(l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~(WS_CAPTION | WS_THICKFRAME));
        SetWindowLong(_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		
		ShowWindow(_hwnd, SW_MAXIMIZE);
		SetWindowPos (_hwnd, HWND_TOP, 0, 0, screen_width, screen_height, 0);
	}
	else
	{
		LONG l_WinStyle = GetWindowLong (_hwnd, GWL_STYLE);
		SetWindowLong(_hwnd, GWL_STYLE,(l_WinStyle | WS_CAPTION | WS_THICKFRAME | WS_BORDER) & ~WS_POPUP & ~WS_MAXIMIZE);
		//SetWindowPos(_hwnd, HWND_TOP, left, top, width, height, 0);
	}
}


void Window::setSize() {
	RECT rc;
	GetClientRect (_hwnd, 	 	   &rc);
	//ClientToScreen(_hwnd, (POINT *)&rc);
	rect = {(float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom};
	
	centerX = rect.left + rect.width()  / 2;
	centerY = rect.top  +  rect.height() / 2;
	
	if(getIES())  getIES()->setSize(rect.width(), rect.height());
}



//system metrics
Rect Window::screenSize() { return Rect(screen_width, screen_height); }
void Window::showCursor(const bool show) { SetCursor(show? arrowCursor : noneCursor); }