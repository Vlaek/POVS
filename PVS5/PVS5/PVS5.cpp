#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <stdlib.h>
#include "resource.h"

#define WINDOWTITLE "MainWindow"
#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define PERCENTX 50
#define PERCENTY 50
#define BUFFERMAX 256

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    HBRUSH hBrushes[2] = {
        CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1))),
        CreatePatternBrush(LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP2)))
    };

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = VERYMAINWINDOW;
    wc.hbrBackground = hBrushes[1];
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

    if (!RegisterClass(&wc))
        return 0;

    int systemWidth = GetSystemMetrics(SM_CXSCREEN);
    int systemHeight = GetSystemMetrics(SM_CYSCREEN);

    int windowWidth = (systemWidth * PERCENTX) / 100;
    int windowHeight = (systemHeight * PERCENTY) / 100;

    int windowPosX = (systemWidth - windowWidth) / 2;
    int windowPosY = (systemHeight - windowHeight) / 2;

    HWND hMainWnd = CreateWindow(VERYMAINWINDOW, WINDOWTITLE, WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
    if (!hMainWnd)
        return 0;

    SetWindowLong(hMainWnd, GWL_USERDATA, LONG(&hBrushes));

    char string[BUFFERMAX], *end;

    LoadString(hInstance, IDS_STRINGBEGIN, string, BUFFERMAX);
    ULONG stringIdBegin = strtoul(string, &end, 10);

    LoadString(hInstance, IDS_STRINGEND, string, BUFFERMAX);
    ULONG stringIdEnd = strtoul(string, &end, 10);

    for (UINT i = stringIdBegin; i <= stringIdEnd; i++)
        if (LoadString(hInstance, i, string, BUFFERMAX))
            break;

    ShowWindow(hMainWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) 
    {
        case WM_RBUTTONDOWN: 
        {
            POINT p;
            GetCursorPos(&p);

            HMENU hPopupMenu = CreatePopupMenu();

            HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

            char string[BUFFERMAX], *end;

            LoadString(hInstance, IDS_STRINGBEGIN, string, BUFFERMAX);
            ULONG stringIdBegin = strtoul(string, &end, 10);

            LoadString(hInstance, IDS_STRINGEND, string, BUFFERMAX);
            ULONG stringIdEnd = strtoul(string, &end, 10);

            for (UINT i = stringIdBegin; i <= stringIdEnd; i++)
                if (LoadString(hInstance, i, string, BUFFERMAX))
                    AppendMenu(hPopupMenu, MF_STRING, i, string);

            AppendMenu(hPopupMenu, MF_SEPARATOR, 0, NULL);

            LoadString(hInstance, ID_BMP1, string, BUFFERMAX);
            AppendMenu(hPopupMenu, MF_STRING, IDB_BITMAP1, string);
            LoadString(hInstance, ID_BMP2, string, BUFFERMAX);
            AppendMenu(hPopupMenu, MF_STRING, IDB_BITMAP2, string);

            TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_TOPALIGN, p.x, p.y, 0, hWnd, NULL);

            return 0;
        }

        case WM_COMMAND: 
        {
            switch (wParam)
            {
                case IDB_BITMAP1:
                {
                    HBRUSH *hBrush = (HBRUSH*)GetWindowLong(hWnd, GWL_USERDATA);

                    SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)hBrush[0]);

                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                }

                case IDB_BITMAP2:
                {
                    HBRUSH *hBrush = (HBRUSH*)GetWindowLong(hWnd, GWL_USERDATA);

                    SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)hBrush[1]);

                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                }

                default:
                {
                    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

                    char wndTitle[BUFFERMAX];

                    LoadString(hInstance, wParam, wndTitle, BUFFERMAX);

                    SetWindowText(hWnd, wndTitle);

                    return 0;
                }

                case MENU_QUIT:
                    DestroyWindow(hWnd);
                    return 0;
            }
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}