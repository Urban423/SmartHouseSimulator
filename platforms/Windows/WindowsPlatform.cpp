#include "WindowsPlatform.h"

void WindowsPlatform::create() {
    arrowCursor = LoadCursor(NULL, IDC_ARROW);
    noneCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_NONECURSOR));
}

void WindowsPlatform::update() {
    MSG msg;
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
    }
}