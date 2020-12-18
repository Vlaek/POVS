#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define PERCENTX 50
#define PERCENTY 50

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Line(HDC hDc, int x1, int y1, int x2, int y2);
BOOL Rect(HDC hDc, int x1, int y1, int x2, int y2, HBRUSH hBrush);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = VERYMAINWINDOW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return 0;

    int systemWidth = GetSystemMetrics(SM_CXSCREEN);
    int systemHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = (systemWidth * PERCENTX) / 100;
    int windowHeight = (systemHeight * PERCENTY) / 100;

    int windowPosX = (systemWidth - windowWidth) / 2;
    int windowPosY = (systemHeight - windowHeight) / 2;

    HWND hMainWnd = CreateWindow(VERYMAINWINDOW, "MainWindow", WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
    if (!hMainWnd)
        return 0;

    ShowWindow(hMainWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

    case WM_SIZE:

        InvalidateRect(hWnd, NULL, true);
        return 0;

    case WM_MOUSEMOVE: {

        LONG x = LOWORD(lParam);
        LONG y = HIWORD(lParam);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int rectWidth = rect.right / 3;
        int rectHeight = rect.bottom / 3;

        int rectColumnNum = x / rectWidth + 1;
        int rectRowNum = y / rectHeight + 1;

        DWORD rectRowColumnNums = DWORD MAKELONG(rectColumnNum, rectRowNum);

        if (GetWindowLong(hWnd, GWL_USERDATA) != rectRowColumnNums) {

            SetWindowLong(hWnd, GWL_USERDATA, rectRowColumnNums);
            InvalidateRect(hWnd, &rect, true);
        }

        SetCapture(hWnd);
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(hWnd, &p);

        if (p.x >= rect.right or p.x <= 0 or p.y >= rect.bottom or p.y < 0) {
            ReleaseCapture();
        }

        return 0;
    }

    case WM_PAINT: {

        RECT rect;
        GetClientRect(hWnd, &rect);

        int windowWidth = rect.right;
        int windowHeight = rect.bottom;

        int rectWidth = windowWidth / 3;
        int rectHeight = windowHeight / 3;

        PAINTSTRUCT ps;
        HDC hDc = BeginPaint(hWnd, &ps);

        LONG rectCell = GetWindowLong(hWnd, GWL_USERDATA);

        if (rectCell) {

            int rectColumnNum = LOWORD(rectCell) - 1;
            int rectRowNum = HIWORD(rectCell) - 1;

            Rect(hDc, rectWidth * rectColumnNum, rectHeight * rectRowNum,
                rectWidth * (rectColumnNum + 1), rectHeight * (rectRowNum + 1),
                HBRUSH(COLOR_WINDOW + 2));
        }

        Line(hDc, 0, windowHeight / 3, windowWidth, windowHeight / 3);
        Line(hDc, 0, windowHeight / 3 * 2, windowWidth, windowHeight / 3 * 2);
        Line(hDc, windowWidth / 3, 0, windowWidth / 3, windowHeight);
        Line(hDc, windowWidth / 3 * 2, 0, windowWidth / 3 * 2, windowHeight);

        EndPaint(hWnd, &ps);

        return 0;
    }

    case WM_DESTROY:

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL Line(HDC hDc, int x1, int y1, int x2, int y2) {

    MoveToEx(hDc, x1, y1, NULL);
    return LineTo(hDc, x2, y2);
}

BOOL Rect(HDC hDc, int x1, int y1, int x2, int y2, HBRUSH hBrush) {

    RECT rect;

    rect.top = y1;
    rect.bottom = y2;
    rect.left = x1;
    rect.right = x2;

    return FillRect(hDc, &rect, hBrush);
}
