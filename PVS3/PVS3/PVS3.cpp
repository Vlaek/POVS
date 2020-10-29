#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>

#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define PERCENTX 50
#define PERCENTY 50
#define N 5000 
#define M 3000
#define K 500
#define R 100

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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

    POINT points[K];
    for (int i = 0; i < K; i++)
    {
        points[i].x = N * rand() / RAND_MAX;
        points[i].y = M * rand() / RAND_MAX;
    }
    SetWindowLong(hMainWnd, GWL_USERDATA, (LONG)&points);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    
    switch (uMsg)
    {
        case WM_SIZE:
        {
            RECT rect;
            GetClientRect(hWnd, &rect);

            SCROLLINFO hScroll = { 0 };
            hScroll.cbSize = sizeof(SCROLLINFO);
            hScroll.fMask = SIF_RANGE;
            hScroll.nMax = N - rect.right;
            hScroll.nMin = 0;
            SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);

            SCROLLINFO vScroll = { 0 };
            vScroll.cbSize = sizeof(SCROLLINFO);
            vScroll.fMask = SIF_RANGE;
            vScroll.nMax = M - rect.bottom;
            vScroll.nMin = 0;
            SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

            InvalidateRect(hWnd, NULL, TRUE);

            return 0;
        }

        case WM_PAINT:
        {
            SCROLLINFO hScroll = { 0 };
            hScroll.cbSize = sizeof(SCROLLINFO);
            hScroll.fMask = SIF_POS;
            GetScrollInfo(hWnd, SB_HORZ, &hScroll);

            SCROLLINFO vScroll = { 0 };
            vScroll.cbSize = sizeof(SCROLLINFO);
            vScroll.fMask = SIF_POS;
            GetScrollInfo(hWnd, SB_VERT, &vScroll);

            PAINTSTRUCT paint;
            HDC hDc = BeginPaint(hWnd, &paint);
            HBRUSH brushOne = CreateSolidBrush(RGB(0, 0, 255));
            HBRUSH brushTwo = CreateSolidBrush(RGB(255, 0, 0));

            RECT rect;
            POINT* points = (POINT*)GetWindowLong(hWnd, GWL_USERDATA);

            for (int i = 0; i < K; i++)
            {
                rect.right = points[i].x + R - hScroll.nPos;
                rect.left = points[i].x - R - hScroll.nPos;
                rect.top = points[i].y - R - vScroll.nPos;
                rect.bottom = points[i].y + R - vScroll.nPos;

                if (IntersectRect(&rect, &paint.rcPaint, &rect))
                {
                    if (i % 2 == 0) 
                        SelectObject(hDc, brushOne);
                    else 
                        SelectObject(hDc, brushTwo);
                    Ellipse(hDc,
                        points[i].x - R - hScroll.nPos, points[i].y - R - vScroll.nPos,
                        points[i].x + R - hScroll.nPos, points[i].y + R - vScroll.nPos);
                }
            }

            DeleteObject(brushOne);
            DeleteObject(brushTwo);

            EndPaint(hWnd, &paint);
            return 0;
        }

        case WM_HSCROLL:
        {
            SCROLLINFO hScroll;
            hScroll.cbSize = sizeof(SCROLLINFO);
            hScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
            GetScrollInfo(hWnd, SB_HORZ, &hScroll);

            RECT rect;
            GetClientRect(hWnd, &rect);

            int b = 0;
            int k = 0;

            switch (LOWORD(wParam))
            {
                case SB_LINERIGHT:
                {
                    k = 25;

                    b = hScroll.nMax - hScroll.nPos;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    hScroll.nPos += b;

                    break;
                }
                case SB_PAGERIGHT:
                {
                    k = rect.right;

                    b = hScroll.nMax - hScroll.nPos;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    hScroll.nPos += b;

                    break;
                }
                case SB_LINELEFT:
                {
                    k = 25;

                    b = hScroll.nPos - hScroll.nMin;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    b = -b;

                    hScroll.nPos += b;

                    break;
                }
                case SB_PAGELEFT:
                {
                    k = rect.right;

                    b = hScroll.nPos - hScroll.nMin;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    b = -b;

                    hScroll.nPos += b;

                    break;
                }

                case SB_THUMBTRACK:
                {
                    b = hScroll.nTrackPos - hScroll.nPos;
                    hScroll.nPos = hScroll.nTrackPos;

                    break;
                }
            }

            ScrollWindowEx(hWnd, -b, 0, NULL, NULL, NULL, NULL, SW_ERASE);
            
            if (b > 0)
                rect.left = rect.right - b;
            else
                rect.right = rect.left - b;

            SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);

            InvalidateRect(hWnd, &rect, TRUE);
            UpdateWindow(hWnd);

            return 0;
        }

        case WM_VSCROLL:
        {
            SCROLLINFO vScroll;
            vScroll.cbSize = sizeof(SCROLLINFO);
            vScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
            GetScrollInfo(hWnd, SB_VERT, &vScroll);

            RECT rect;
            GetClientRect(hWnd, &rect);

            int b = 0;
            int k = 0;

            switch (LOWORD(wParam))
            {
                case SB_LINEDOWN: 
                {
                    k = 25;

                    b = vScroll.nMax - vScroll.nPos;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    vScroll.nPos += b;

                    break;
                }
                case SB_PAGEDOWN:
                {
                    k = rect.bottom;

                    b = vScroll.nMax - vScroll.nPos;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    vScroll.nPos += b;

                    break;
                }

                case SB_LINEUP: 
                {
                    k = 25;

                    b = vScroll.nPos - vScroll.nMin;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    b = -b;
                    vScroll.nPos += b;

                    break;
                }
                case SB_PAGEUP:
                {
                    k = rect.bottom;

                    b = vScroll.nPos - vScroll.nMin;

                    if (b == 0)
                        return 0;

                    if (b > k)
                        b = k;

                    b = -b;
                    vScroll.nPos += b;

                    break;
                }

                case SB_THUMBTRACK:
                {
                    b = vScroll.nTrackPos - vScroll.nPos;
                    vScroll.nPos = vScroll.nTrackPos;

                    break;
                }
            }

            ScrollWindowEx(hWnd, 0, -b, NULL, NULL, NULL, NULL, SW_ERASE);

            if (b > 0)
                rect.top = rect.bottom - b;
            else
                rect.bottom = rect.top - b;

            SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

            InvalidateRect(hWnd, &rect, TRUE);
            UpdateWindow(hWnd);

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}