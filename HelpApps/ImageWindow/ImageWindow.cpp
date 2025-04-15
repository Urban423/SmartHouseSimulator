#include <windows.h>
#include "../ImageEditor.h"

// Global Variables
HBITMAP memBitmap = NULL;
int imgWidth = 0, imgHeight = 0;

// Function to update the buffer with image data
void setBuffer(HWND hwnd, int width, int height, int* image) {
    HDC hdc = GetDC(hwnd);
    HDC src = CreateCompatibleDC(hdc);

    // Create a DIB section for memory-mapped bitmap
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative for correct top-down orientation
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 4 bytes per pixel (ARGB)
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bitmapMemory = nullptr;
    memBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bitmapMemory, NULL, 0);
    
    if (!memBitmap) {
        MessageBox(hwnd, "Failed to create DIB section!", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Copy image data to the bitmap memory
    memcpy(bitmapMemory, image, width * height * 4);

    // Select the bitmap into the memory DC
    SelectObject(src, memBitmap);

    // Copy the bitmap to the window
    BitBlt(hdc, 0, 0, width, height, src, 0, 0, SRCCOPY);

    // Cleanup
    DeleteDC(src);
    ReleaseDC(hwnd, hdc);
}

// Window Procedure (Handles Messages)
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            if (memBitmap) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                HDC src = CreateCompatibleDC(hdc);
                SelectObject(src, memBitmap);
                BitBlt(hdc, 0, 0, imgWidth, imgHeight, src, 0, 0, SRCCOPY);
                DeleteDC(src);
                EndPaint(hwnd, &ps);
            }
            break;
        }

        case WM_DESTROY:
            if (memBitmap) {
                DeleteObject(memBitmap);
                memBitmap = NULL;
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Standard main() function entry point
int main(int argc, char* argv[]) {
    if (argc < 2) {
        MessageBox(NULL, "Usage: program.exe <image_path>", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    // Load Image
    Image image(argv[1]);
	remove(argv[1]);
    imgWidth = image.getWidth();
    imgHeight = image.getHeight();

    // Get the instance handle
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Register Window Class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ImageWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    // Adjust the window size to fit the image exactly
    RECT rc = { 0, 0, imgWidth, imgHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE);
    int winWidth = rc.right - rc.left;
    int winHeight = rc.bottom - rc.top;

    // Create Window
    HWND hwnd = CreateWindowEx(
        WS_EX_NOACTIVATE,  // Prevents window from stealing focus
        "ImageWindow", "Image Viewer",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, winWidth, winHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return -1;

    // Show Window (without stealing focus)
    ShowWindow(hwnd, SW_SHOWNA);
    UpdateWindow(hwnd);

    // Set Buffer (call only after window creation)
    setBuffer(hwnd, imgWidth, imgHeight, image.getImage());

    // Message Loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
