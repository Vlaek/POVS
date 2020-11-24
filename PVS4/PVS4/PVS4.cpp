#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "resource.h"

#define WINDOWTITLE "MainWindow"
#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define PERCENTX 50
#define PERCENTY 50
#define STR "TEXTTEXTTEXT" 
#define TIMERID 123
#define ANGLE 100
#define MAXHEIGHT 300
#define MINHEIGHT 20
#define PERIOD 100
#define MAXPERIOD 1000
#define MINPERIOD 0

// альтернативу WM_SIZE
// что будет при отрицательном значение lfHeight

typedef struct {
    LOGFONT font;
    UINT period;
} USERDATA;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = VERYMAINWINDOW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wc.style = CS_VREDRAW | CS_HREDRAW;

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

    LOGFONT font = { 0 };
    font.lfHeight = 32;

    USERDATA userdata;
    userdata.font = font;
    userdata.period = PERIOD;

    SetWindowLong(hMainWnd, GWL_USERDATA, LONG(&userdata));

    ShowWindow(hMainWnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

        case WM_CREATE:

            SetTimer(hWnd, TIMERID, PERIOD, NULL);

            return 0;

        case WM_COMMAND:

            switch (LOWORD(wParam)) 
            {
                case MENU_START: 
                {
                    HMENU hMenu = GetMenu(hWnd);

                    EnableMenuItem(hMenu, MENU_STOP, MF_ENABLED);
                    CheckMenuItem(hMenu, MENU_STOP, MF_UNCHECKED);

                    EnableMenuItem(hMenu, MENU_START, MF_GRAYED);
                    CheckMenuItem(hMenu, MENU_START, MF_CHECKED);

                    USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);

                    SetTimer(hWnd, TIMERID, userdata->period, NULL);

                    return 0;
                }

                case MENU_STOP: 
                {
                    KillTimer(hWnd, TIMERID);

                    HMENU hMenu = GetMenu(hWnd);

                    EnableMenuItem(hMenu, MENU_STOP, MF_GRAYED);
                    CheckMenuItem(hMenu, MENU_STOP, MF_CHECKED);

                    EnableMenuItem(hMenu, MENU_START, MF_ENABLED);
                    CheckMenuItem(hMenu, MENU_START, MF_UNCHECKED);

                    return 0;
                }

                case MENU_QUIT:
                    DestroyWindow(hWnd);
                    return 0;
            }

        case WM_TIMER:

            switch (wParam) 
            {
                case TIMERID:
                {
                    KillTimer(hWnd, TIMERID);

                    USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);

                    userdata->font.lfEscapement += ANGLE;

                    InvalidateRect(hWnd, NULL, TRUE);

                    SetTimer(hWnd, TIMERID, userdata->period, NULL);

                    return 0;
                }
            }

        case WM_RBUTTONDOWN: 
        {
            USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);
//            if (userdata->font.lfHeight > MINHEIGHT)
                userdata->font.lfHeight -= 20;
//            else
//                return 0;

            InvalidateRect(hWnd, NULL, TRUE);

            return 0;
        }

        case WM_LBUTTONDOWN: 
        {
            USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);
  //          if (userdata->font.lfHeight < MAXHEIGHT)
                userdata->font.lfHeight += 20;
//            else
//                return 0;

            InvalidateRect(hWnd, NULL, TRUE);

            return 0;
        }

        case WM_KEYDOWN:

            switch (wParam) 
            {
                case VK_ADD: 
                {
                    USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);
                    if (userdata->period > MINPERIOD)
                        userdata->period -= 50;
                    else
                        return 0;

                    return 0;
                }

                case VK_SUBTRACT: 
                {
                    USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);
                    if (userdata->period < MAXPERIOD)
                        userdata->period += 50;
                    else
                        return 0;

                    return 0;
                }
            }

        case WM_PAINT: 
        {
            RECT rect;
            GetClientRect(hWnd, &rect);

            PAINTSTRUCT ps;
            HDC hDc = BeginPaint(hWnd, &ps);

            USERDATA *userdata = (USERDATA*)GetWindowLong(hWnd, GWL_USERDATA);

            HFONT hfont = CreateFontIndirect(&userdata->font);

            HGDIOBJ oldFont = SelectObject(hDc, hfont);

            SetTextAlign(hDc, TA_CENTER | TA_BASELINE);
            TextOut(hDc, rect.right / 2, rect.bottom / 2, STR, lstrlen(STR));

            SelectObject(hDc, oldFont);
            EndPaint(hWnd, &ps);

            DeleteObject(hfont);

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
    // Лаба 6
    // N кнопок, скроллинг, N радио кнопок и надписи их номеров
    // M кнопка зажата и радио кнопка тоже
    // при нажатие на кнопку активировать радио кнопку тоже и наоборот

    // | [Кнопка №N] O Радио кнопка №N |
    // обработка скроллинг, скроллирование на высоту элементов. 1 скроллинг - 1 полосочка (высота кнопки + зазор)
    // WM_CREATE создавать кнопки
    // WM_COMMAND 
    // Без глобальных переменных

    //Лаба 7
    //Создается приложение с 1 главынм окном, в клиентской области MxN пикселей прямоугольник
    //Только в нем производится отрисовка следа мыши при нажатии ЛКМ
    //Линейная интерполяция от предыдущей точки, соединение двух точек
    // делать линии при 2 нажатиях
    // приложение отслуживается изменения размеров своего окна и препятствует
    // в качестве мин. размеров используется MxN + 40%
    // минимальные размеры окна 1.4M x 1.4N
    // WM_SIZING, WM_MOUSEMOVE, WM_PAINT

    // Еще 3 задания