#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>

#define WINDOWTITLE "Paint"
#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define PERCENTX 50
#define PERCENTY 50

#define X 400
#define Y 300

typedef struct {
    BOOL pixel;
    POINT point;
} USERDATA;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void drawLine(HWND hWnd, int x1, int y1, int x2, int y2);

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

    if (windowWidth < 1.4 * X)
        windowWidth = (X * 14) / 10;

    if (windowHeight < 1.4 * Y)
        windowHeight = (Y * 14) / 10;

    HWND hMainWnd = CreateWindow(VERYMAINWINDOW, WINDOWTITLE, WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
    if (!hMainWnd)
        return 0;

    //BOOL **pixels = new BOOL * [X]; //
    //for (UINT x = 0; x < X; x++) {
    //    pixels[x] = new BOOL[Y];
    //    for (UINT y = 0; y < Y; y++)
    //        pixels[x][y] = FALSE;
    //}
    BOOL pixels[X][Y];
    for (UINT x = 0; x < X; x++) {
        for (UINT y = 0; y < Y; y++)
            pixels[x][y] = FALSE;
    }
    USERDATA userdata = { **pixels, 0 };

    SetWindowLong(hMainWnd, GWL_USERDATA, (LONG)&userdata);

    ShowWindow(hMainWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
        case WM_SIZING: 
        {
            RECT &rc = *(LPRECT)lParam;

            if (rc.bottom - rc.top < Y * 14 / 10)
                if ((wParam == WMSZ_BOTTOM) | (wParam == WMSZ_BOTTOMLEFT) | (wParam == WMSZ_BOTTOMRIGHT))
                    rc.bottom = rc.top + Y * 14 / 10;
                else
                    rc.top = rc.bottom - Y * 14 / 10;

            if (rc.right - rc.left < X * 14 / 10)
                if ((wParam == WMSZ_LEFT) | (wParam == WMSZ_TOPLEFT) | (wParam == WMSZ_BOTTOMLEFT))
                    rc.left = rc.right - X * 14 / 10;
                else
                    rc.right = rc.left + X * 14 / 10;

            return 0;
        }

        case WM_LBUTTONDOWN:
        {   
            UINT x = LOWORD(lParam);
            UINT y = HIWORD(lParam);

            if (x < X && y < Y) {

                USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);

                if (userdata->point.x != 0 && userdata->point.y != 0) {

                    drawLine(hWnd, userdata->point.x, userdata->point.y, x, y);

                    InvalidateRect(hWnd, NULL, FALSE);
                }

                userdata->point.x = x;
                userdata->point.y = y;
            }

            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hDc = BeginPaint(hWnd, &ps);

            MoveToEx(hDc, X, 0, NULL);
            LineTo(hDc, X, Y);

            MoveToEx(hDc, 0, Y, NULL);
            LineTo(hDc, X, Y);

            USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);

            for (UINT x = 0; x < X; x++)
                for (UINT y = 0; y < Y; y++)
                    if (userdata->pixel[x][y])
                        SetPixel(hDc, x, y, RGB(0, 0, 0));

            EndPaint(hWnd, &ps);

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void drawLine(HWND hWnd, int x1, int y1, int x2, int y2) {

    USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);

    INT deltaX = abs(x2 - x1);
    INT deltaY = abs(y2 - y1);

    INT signX = x1 < x2 ? 1 : -1;
    INT signY = y1 < y2 ? 1 : -1;

    INT error = deltaX - deltaY;

    while (x1 != x2 || y1 != y2) {

        userdata->pixel[x1][y1] = TRUE;

        INT error2 = error * 2;

        if (error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }

        if (error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
}