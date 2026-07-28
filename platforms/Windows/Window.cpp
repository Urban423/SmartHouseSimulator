#include "Window.h"
#include <stdio.h>
#include "resource.h"

Window* win;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
        case WM_EXITSIZEMOVE: {
            IOSystem::onWindowResizeFinished();
            break;
        }
		case WM_DESTROY: {
			PostQuitMessage(0);
			win->onDestroy();
			break;
		}
		case WM_GETMINMAXINFO: {
            MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;
            pMinMax->ptMinTrackSize.x = 480;
            pMinMax->ptMinTrackSize.y = 270;
            return 0;
        }
		case WM_CHAR: {
			IOSystem::getPlatform().addText((int)wParam);
			break;
		}
		case WM_SIZE: {
			switch (wParam) {
			case SIZE_MAXIMIZED:
				IOSystem::onWindowResizeFinished();
				break;

			case SIZE_RESTORED:
				IOSystem::onWindowResizeFinished();
				break;

			case SIZE_MINIMIZED:
				break;
			}

			break;
		}
		case WM_KEYDOWN: {
			IOSystem::getPlatform().addKeyEvent((int)wParam);
			break;
		}
		default: { return DefWindowProc(hwnd, msg, wParam, lParam); }
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::~Window() { DestroyWindow(_hwnd); }

void Window::close() {}

void Window::create(const char* windowName, int width, int height, bool fullscreen, bool vsync) {
	win = this;
	this->vsync = vsync;
	
	
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

	if (!RegisterClassEx(&wc)) onDestroy();

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_SYSMENU, false);

	HWND existing = FindWindowA(windowName, nullptr);
	int posX = 3;
	int posY = 30;
	if (existing) {
		posX = 1000;
		posY = 30;
	}

	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		windowName, windowName,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME,
		posX, posY,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, NULL, NULL, NULL
	);

	if (!_hwnd) onDestroy();
	setFullscreen(fullscreen);

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
}

Rect Window::getInnerSize() { 
	RECT rc; 
	GetClientRect(_hwnd, &rc); 
	POINT topLeft = {rc.left, rc.top}; 
	POINT bottomRight = {rc.right, rc.bottom}; 
	ClientToScreen(_hwnd, &topLeft); 
	ClientToScreen(_hwnd, &bottomRight); 
	return { (float)topLeft.x, (float)topLeft.y, (float)bottomRight.x, (float)bottomRight.y };
}

std::pair<int, int> Window::getScreenSize() { 
	int screen_width  = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	return {screen_width, screen_height};
}

void Window::setFullscreen(const bool state){
	IOSystem::onWindowResizeFinished();
	if(state) {
		LONG l_WinStyle = GetWindowLong (_hwnd, GWL_STYLE);
		SetWindowLong(_hwnd, GWL_STYLE,(l_WinStyle | WS_POPUP | WS_MAXIMIZE) & ~(WS_CAPTION | WS_THICKFRAME));
        SetWindowLong(_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		
		ShowWindow(_hwnd, SW_MAXIMIZE);
		auto [screen_width, screen_height] = getScreenSize();
		SetWindowPos (_hwnd, HWND_TOP, 0, 0, screen_width, screen_height, 0);
	}
	else
	{
		LONG l_WinStyle = GetWindowLong (_hwnd, GWL_STYLE);
		SetWindowLong(_hwnd, GWL_STYLE,(l_WinStyle | WS_CAPTION | WS_THICKFRAME | WS_BORDER) & ~WS_POPUP & ~WS_MAXIMIZE);
		//SetWindowPos(_hwnd, HWND_TOP, left, top, width, height, 0);
	}
}

void Window::setSize(int width, int height) {
    RECT rc = {0, 0, width, height};

    DWORD style = GetWindowLong(_hwnd, GWL_STYLE);
    DWORD exStyle = GetWindowLong(_hwnd, GWL_EXSTYLE);

    AdjustWindowRectEx(&rc, style, FALSE, exStyle);
    int windowWidth = rc.right - rc.left;
    int windowHeight = rc.bottom - rc.top;

    SetWindowPos(_hwnd, nullptr, 0, 0, windowWidth, windowHeight, SWP_NOMOVE | SWP_NOZORDER);
}

void Window::setPosition(int x, int y) {

}

std::pair<int, int> Window::getSize() {
	RECT rc; 
	GetClientRect(_hwnd, &rc); 
	POINT topLeft = {rc.left, rc.top}; 
	POINT bottomRight = {rc.right, rc.bottom}; 
	ClientToScreen(_hwnd, &topLeft); 
	ClientToScreen(_hwnd, &bottomRight); 
	return {bottomRight.x - topLeft.x, bottomRight.y - topLeft.y};
}


// void Window::setRenderContextNULL() { wglMakeCurrent(NULL, NULL); }
// void Window::setRenderContext() {  wglMakeCurrent(hDC, hRC); }