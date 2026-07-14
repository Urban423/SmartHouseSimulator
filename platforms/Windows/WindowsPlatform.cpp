#include "WindowsPlatform.h"

void WindowsPlatform::create() {
    arrowCursor = LoadCursor(NULL, IDC_ARROW);
    handCursor =  LoadCursor(NULL, IDC_HAND);
    beamCursor =  LoadCursor(NULL, IDC_IBEAM);
    noneCursor = LoadCursor(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_NONECURSOR));
}

void WindowsPlatform::update() {
    MSG msg;
    while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg); 
        DispatchMessage(&msg);
    }
}