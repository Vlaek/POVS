#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>
#include "resource.h"

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
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);

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
        points[i].x = (UINT)rand()%N;
        points[i].y = (UINT)rand()%M;
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

        case WM_COMMAND:

            switch (LOWORD(wParam))
            {
                case MENU_ELLIPSE: 
                {
                    HMENU hMenu = GetMenu(hWnd);

                    EnableMenuItem(hMenu, MENU_RECT, MF_ENABLED);
                    CheckMenuItem(hMenu, MENU_RECT, MF_UNCHECKED);

                    EnableMenuItem(hMenu, MENU_ELLIPSE, MF_GRAYED);
                    CheckMenuItem(hMenu, MENU_ELLIPSE, MF_CHECKED);

                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                }

                case MENU_RECT: 
                {
                    HMENU hMenu = GetMenu(hWnd);

                    EnableMenuItem(hMenu, MENU_RECT, MF_GRAYED);
                    CheckMenuItem(hMenu, MENU_RECT, MF_CHECKED);

                    EnableMenuItem(hMenu, MENU_ELLIPSE, MF_ENABLED);
                    CheckMenuItem(hMenu, MENU_ELLIPSE, MF_UNCHECKED);

                    InvalidateRect(hWnd, NULL, TRUE);

                    return 0;
                }

                case MENU_QUIT:
                    DestroyWindow(hWnd);
                    return 0;
            }

        case WM_KEYDOWN:

            switch (LOWORD(wParam))
            {
                case VK_RIGHT:
                {
                    PostMessage(hWnd, WM_HSCROLL, SB_LINERIGHT, NULL);
                    return 0;
                }

                case VK_LEFT:
                {
                    PostMessage(hWnd, WM_HSCROLL, SB_LINELEFT, NULL);
                    return 0;
                }

                case VK_UP:
                {
                    PostMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
                    return 0;
                }

                case VK_DOWN:
                {
                    PostMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
                    return 0;
                }

                case VK_HOME: 
                {
                    PostMessage(hWnd, WM_HSCROLL, SB_TOP, NULL);
                    return 0;
                }

                case VK_END: 
                {
                    PostMessage(hWnd, WM_HSCROLL, SB_BOTTOM, NULL);
                    return 0;
                }

                case VK_PRIOR: 
                {
                    PostMessage(hWnd, WM_VSCROLL, SB_TOP, NULL);
                    return 0;
                }

                case VK_NEXT: 
                {
                    PostMessage(hWnd, WM_VSCROLL, SB_BOTTOM, NULL);
                    return 0;
                }
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

            PAINTSTRUCT ps;
            HDC hDc = BeginPaint(hWnd, &ps);
            HBRUSH brushOne = CreateSolidBrush(RGB(90, 60, 90));
            HBRUSH brushTwo = CreateSolidBrush(RGB(60, 90, 60));

            RECT rect;
            POINT *points = (POINT*)GetWindowLong(hWnd, GWL_USERDATA);

            HMENU hMenu = GetMenu(hWnd);
            MENUITEMINFO iteminfo = { 0 };
            iteminfo.cbSize = sizeof(MENUITEMINFO);
            iteminfo.fMask = MIIM_STATE;
            GetMenuItemInfo(hMenu, MENU_ELLIPSE, FALSE, &iteminfo);

            UINT figure = iteminfo.fState & MFS_CHECKED ? R * 2 : 0;
            //функции с одинаковым списком параметров 
            for (int i = 0; i < K; i++)
            {
                rect.right = points[i].x + R; - hScroll.nPos;
                rect.left = points[i].x - R - hScroll.nPos;
                rect.top = points[i].y - R - vScroll.nPos;
                rect.bottom = points[i].y + R - vScroll.nPos;

                if (IntersectRect(&rect, &ps.rcPaint, &rect))
                {
                    if (i % 2 == 0) 
                        SelectObject(hDc, brushOne);
                    else 
                        SelectObject(hDc, brushTwo);

                    RoundRect(hDc,
                        points[i].x - R - hScroll.nPos, points[i].y - R - vScroll.nPos,
                        points[i].x + R - hScroll.nPos, points[i].y + R - vScroll.nPos, figure, figure);
                }
            }

            DeleteObject(brushOne);
            DeleteObject(brushTwo);

            EndPaint(hWnd, &ps);
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

            int dx = 0;
            int move = 0;

            switch (LOWORD(wParam))
            {
                case SB_TOP:
                {
                    SCROLLINFO hScroll;
                    hScroll.cbSize = sizeof(SCROLLINFO);
                    hScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
                    GetScrollInfo(hWnd, SB_HORZ, &hScroll);

                    RECT rect;
                    GetClientRect(hWnd, &rect);

                    int dx = hScroll.nPos;

                    if (dx == 0)
                        return 0;

                    dx = -dx;

                    hScroll.nPos += dx;

                    ScrollWindow(hWnd, -dx, 0, NULL, NULL);

                    rect.right = rect.left - dx;

                    SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);

                    InvalidateRect(hWnd, &rect, TRUE);

                    return 0;
                }

                case SB_BOTTOM:
                {
                    SCROLLINFO hScroll;
                    hScroll.cbSize = sizeof(SCROLLINFO);
                    hScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
                    GetScrollInfo(hWnd, SB_HORZ, &hScroll);

                    RECT rect;
                    GetClientRect(hWnd, &rect);

                    int dx = hScroll.nMax - hScroll.nPos;

                    if (dx == 0)
                        return 0;

                    hScroll.nPos += dx;

                    ScrollWindow(hWnd, -dx, 0, NULL, NULL);

                    rect.right = rect.left - dx;

                    SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);

                    InvalidateRect(hWnd, &rect, TRUE);

                    return 0;
                }
                case SB_LINERIGHT:
                {
                    move = 25;

                    dx = hScroll.nMax - hScroll.nPos;

                    if (dx == 0)
                        return 0;

                    if (dx > move)
                        dx = move;

                    hScroll.nPos += dx;

                    break;
                }

                case SB_PAGERIGHT:
                {
                    move = rect.right;

                    dx = hScroll.nMax - hScroll.nPos;

                    if (dx == 0)
                        return 0;

                    if (dx > move)
                        dx = move;

                    hScroll.nPos += dx;

                    break;
                }

                case SB_LINELEFT:
                {
                    move = 25;

                    dx = hScroll.nPos - hScroll.nMin;

                    if (dx == 0)
                        return 0;

                    if (dx > move)
                        dx = move;

                    dx = -dx;

                    hScroll.nPos += dx;

                    break;
                }

                case SB_PAGELEFT:
                {
                    move = rect.right;

                    dx = hScroll.nPos - hScroll.nMin;

                    if (dx == 0)
                        return 0;

                    if (dx > move)
                        dx = move;

                    dx = -dx;

                    hScroll.nPos += dx;

                    break;
                }

                case SB_THUMBTRACK:
                {
                    dx = hScroll.nTrackPos - hScroll.nPos;
                    hScroll.nPos = hScroll.nTrackPos;

                    break;
                }
            }

            ScrollWindow(hWnd, -dx, 0, NULL, NULL);
            
            if (dx > 0)
                rect.left = rect.right - dx;
            else
                rect.right = rect.left - dx;

            SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);

            InvalidateRect(hWnd, &rect, TRUE);

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

            int dy = 0;
            int move = 0;

            switch (LOWORD(wParam))
            {
                case SB_BOTTOM:
                {
                    SCROLLINFO vScroll;
                    vScroll.cbSize = sizeof(SCROLLINFO);
                    vScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
                    GetScrollInfo(hWnd, SB_VERT, &vScroll);

                    RECT rect;
                    GetClientRect(hWnd, &rect);

                    int dy = vScroll.nMax - vScroll.nPos;

                    if (dy == 0)
                        return 0;

                    vScroll.nPos += dy;

                    ScrollWindow(hWnd, 0, -dy, NULL, NULL);

                    rect.left = rect.right - dy;

                    SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

                    InvalidateRect(hWnd, &rect, TRUE);

                    return 0;
                }

                case SB_TOP:
                {
                    SCROLLINFO vScroll;
                    vScroll.cbSize = sizeof(SCROLLINFO);
                    vScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
                    GetScrollInfo(hWnd, SB_VERT, &vScroll);

                    RECT rect;
                    GetClientRect(hWnd, &rect);

                    int dy = vScroll.nPos;

                    if (dy == 0)
                        return 0;

                    dy = -dy;

                    vScroll.nPos += dy;

                    ScrollWindow(hWnd, 0, -dy, NULL, NULL);

                    rect.left = rect.right - dy;

                    SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

                    InvalidateRect(hWnd, &rect, TRUE);

                    return 0;
                }

                case SB_LINEDOWN: 
                {
                    move = 25;

                    dy = vScroll.nMax - vScroll.nPos;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    vScroll.nPos += dy;

                    break;
                }

                case SB_PAGEDOWN:
                {
                    move = rect.bottom;

                    dy = vScroll.nMax - vScroll.nPos;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    vScroll.nPos += dy;

                    break;
                }

                case SB_LINEUP: 
                {
                    move = 25;

                    dy = vScroll.nPos - vScroll.nMin;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    dy = -dy;
                    vScroll.nPos += dy;

                    break;
                }

                case SB_PAGEUP:
                {
                    move = rect.bottom;

                    dy = vScroll.nPos - vScroll.nMin;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    dy = -dy;
                    vScroll.nPos += dy;

                    break;
                }

                case SB_THUMBTRACK:
                {
                    dy = vScroll.nTrackPos - vScroll.nPos;
                    vScroll.nPos = vScroll.nTrackPos;

                    break;
                }
            }

            ScrollWindow(hWnd, 0, -dy, NULL, NULL);

            if (dy > 0)
                rect.top = rect.bottom - dy;
            else
                rect.bottom = rect.top - dy;

            SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

            InvalidateRect(hWnd, &rect, TRUE);

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}