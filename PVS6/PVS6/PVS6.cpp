#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>

#define WINDOWTITLE "Button"
#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define PERCENTX 40
#define PERCENTY 40
#define BUFFERMAX 256

#define N 1000
#define ACTIVEBUTTON 1
#define BUTTONWIDTH 200
#define BUTTONHEIGHT 50
#define BUTTONSPACE 25

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct LPCHECK{
    UINT count;
    UINT prevButton;
    UINT shift;
};

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

    HWND hMainWnd = CreateWindow(VERYMAINWINDOW, WINDOWTITLE, WS_OVERLAPPEDWINDOW,
        windowPosX, windowPosY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
    if (!hMainWnd)
        return 0;

    char numberButton[BUFFERMAX] = "\0";

    UINT count = GetSystemMetrics(SM_CYSCREEN) / (BUTTONHEIGHT + BUTTONSPACE);

    for (UINT i = 0; i < (count * 2 - 1) && i < N; i += 2)
    {
        wsprintf(numberButton, "Кнопка №%d", i / 2 + 1);
        HWND pushButton = CreateWindow("BUTTON", numberButton, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            10, i / 2 * (BUTTONHEIGHT + BUTTONSPACE) + BUTTONSPACE, BUTTONWIDTH, BUTTONHEIGHT,
            hMainWnd, (HMENU)(i), hInstance, NULL);
        if (!pushButton)
            return 0;

        wsprintf(numberButton, "Радио кнопка №%d", (i + 1) / 2 + 1);
        HWND radioButton = CreateWindow("BUTTON", numberButton, WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            BUTTONWIDTH + BUTTONSPACE, i / 2 * (BUTTONHEIGHT + BUTTONSPACE) + BUTTONSPACE, BUTTONWIDTH, BUTTONHEIGHT,
            hMainWnd, (HMENU)(i + 1), hInstance, NULL);
        if (!radioButton)
            return 0;

        if (i == ACTIVEBUTTON)
        {
            SendMessage(pushButton, BM_SETSTATE, TRUE, NULL);
            SendMessage(radioButton, BM_SETCHECK, BST_CHECKED, NULL);
        }
    }

    LPCHECK data = { count, ACTIVEBUTTON, 0 };
    SetWindowLong(hMainWnd, GWL_USERDATA, (LONG)&data);

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
        case WM_SIZE:
        {
            RECT rect;
            GetClientRect(hWnd, &rect);

            SCROLLINFO vscroll = { 0 };
            vscroll.cbSize = sizeof(SCROLLINFO);
            vscroll.fMask = SIF_RANGE;
            vscroll.nMax = N * (BUTTONHEIGHT + BUTTONSPACE) - rect.bottom;
            vscroll.nMin = 0;
            SetScrollInfo(hWnd, SB_VERT, &vscroll, TRUE);

            SendMessage(hWnd, WM_VSCROLL, 10, NULL);

            return 0;
        }

        case WM_COMMAND:
        {
            LPCHECK *data = (LPCHECK*)GetWindowLong(hWnd, GWL_USERDATA);
            HWND pushButton, prevPushButton, radioButton, prevRadio;

            data->prevButton = (data->prevButton - data->shift - 1) * 2;

            prevPushButton = GetDlgItem(hWnd, data->prevButton);
            prevRadio = GetDlgItem(hWnd, data->prevButton + 1);

            pushButton = prevPushButton;
            radioButton = prevRadio;

            if (LOWORD(wParam) % 2 == 0 && LOWORD(wParam) != data->prevButton)
            {
                pushButton = GetDlgItem(hWnd, wParam);
                radioButton = GetDlgItem(hWnd, wParam + 1);

                data->prevButton = (UINT)LOWORD(wParam);
            }
            else if (LOWORD(wParam) % 2 == 1 && LOWORD(wParam) != data->prevButton + 1)
            {
                radioButton = GetDlgItem(hWnd, wParam);
                pushButton = GetDlgItem(hWnd, wParam - 1);

                data->prevButton = (UINT)LOWORD(wParam) - 1;
            }

            SendMessage(prevPushButton, BM_SETSTATE, FALSE, NULL);
            SendMessage(prevRadio, BM_SETCHECK, BST_UNCHECKED, NULL);

            SendMessage(pushButton, BM_SETSTATE, TRUE, NULL);
            SendMessage(radioButton, BM_SETCHECK, BST_CHECKED, NULL);

            data->prevButton = data->prevButton / 2 + data->shift + 1;

            return 0;
        }

        case WM_VSCROLL:
        {
            SCROLLINFO vScroll;
            vScroll.cbSize = sizeof(SCROLLINFO);
            vScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
            GetScrollInfo(hWnd, SB_VERT, &vScroll);

            int dy = 0;
            int move = 0;

            LPCHECK *data = (LPCHECK*)GetWindowLong(hWnd, GWL_USERDATA);

            HWND pushButton, radioButton;

            char numberButton[BUFFERMAX];

            switch (LOWORD(wParam))
            {
                case SB_LINEDOWN:
                {
                    move = BUTTONHEIGHT + BUTTONSPACE;

                    dy = vScroll.nMax - vScroll.nPos;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    vScroll.nPos += dy;

                    data->shift++;

                    break;
                }

                case SB_LINEUP:
                {
                    move = BUTTONHEIGHT + BUTTONSPACE;

                    dy = vScroll.nPos - vScroll.nMin;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    dy = -dy;

                    vScroll.nPos += dy;

                    data->shift--;

                    break;
                }

                case SB_PAGEDOWN:
                {
                    move = (BUTTONHEIGHT + BUTTONSPACE) * data->count;

                    dy = vScroll.nMax - vScroll.nPos;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    vScroll.nPos += dy;

                    data->shift += data->count;

                    break;
                }

                case SB_PAGEUP:
                {
                    move = (BUTTONHEIGHT + BUTTONSPACE) * data->count;

                    dy = vScroll.nPos - vScroll.nMin;

                    if (dy == 0)
                        return 0;

                    if (dy > move)
                        dy = move;

                    dy = -dy;

                    vScroll.nPos += dy;

                    data->shift -= data->count;

                    break;
                }

                case SB_THUMBTRACK:
                {
                    move = vScroll.nTrackPos - vScroll.nPos;

                    vScroll.nPos = vScroll.nTrackPos;

                    data->shift = vScroll.nPos / (BUTTONHEIGHT + BUTTONSPACE);

                    if (vScroll.nMax == vScroll.nPos)
                        data->shift++;

                    break;
                }
            }

            for (UINT i = 0; i < data->count * 2 - 1; i += 2)
            {
                wsprintf(numberButton, "Кнопка №%d", i / 2 + 1 + data->shift);
                SetWindowText(GetDlgItem(hWnd, i), numberButton);

                wsprintf(numberButton, "Радио кнопка №%d", (i + 1) / 2 + 1 + data->shift);
                SetWindowText(GetDlgItem(hWnd, i + 1), numberButton);

                if (i / 2 + 1 + data->shift == data->prevButton)
                {
                    pushButton = GetDlgItem(hWnd, i);
                    radioButton = GetDlgItem(hWnd, i + 1);

                    SendMessage(pushButton, BM_SETSTATE, TRUE, NULL);
                    SendMessage(radioButton, BM_SETCHECK, BST_CHECKED, NULL);
                }
                else
                {
                    pushButton = GetDlgItem(hWnd, i);
                    radioButton = GetDlgItem(hWnd, i + 1);

                    SendMessage(pushButton, BM_SETSTATE, FALSE, NULL);
                    SendMessage(radioButton, BM_SETCHECK, BST_UNCHECKED, NULL);
                }
            }

            SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}