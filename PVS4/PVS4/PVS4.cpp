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
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    static int size = 20;
    switch (message) {

    case WM_SIZE:

        InvalidateRect(hWnd, NULL, true);
        return 0;

    case WM_KEYDOWN: {
        switch (wParam) {
            case VK_ADD:

            break;
            case VK_SUBTRACT:

            break;
        }
    }

    case WM_LBUTTONDOWN: {
        RECT rect;
        GetClientRect(hWnd, &rect);

        if (size < 100) {
            size += 10;
            int useless = 0;
            DWORD newSize = DWORD MAKELONG(useless, size);

            SetWindowLong(hWnd, GWL_USERDATA, newSize);
            InvalidateRect(hWnd, &rect, true);
        }
        return 0;
    }

    case WM_RBUTTONDOWN: {
        RECT rect;
        GetClientRect(hWnd, &rect);

        if (size > 10) {
            size -= 10;
            int useless = 0;
            DWORD newSize = DWORD MAKELONG(useless, size);

            SetWindowLong(hWnd, GWL_USERDATA, newSize);
            InvalidateRect(hWnd, &rect, true);
        }
        return 0;
    }

    case WM_PAINT: {

        RECT rect;
        GetClientRect(hWnd, &rect);

        PAINTSTRUCT ps;
        HDC hDc = BeginPaint(hWnd, &ps);

        TCHAR text[] = "TEXT IST HIER";
        //size_t            sz;

        //int width, height = GetTextExtentPoint32A(hDc, text, 1, &sz);

        //DrawText(hDc, text, strlen(text), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        //XFORM xform; //создаём экземпляр структуры XFORM

        //xform.eM11 = (FLOAT)1.0; //масштаб по х ;
        //xform.eM22 = (FLOAT)1.0; //масштаб по у
        //xform.eM21 = (FLOAT)2.0; //наклон по х
        //xform.eM12 = (FLOAT)1.0; //наклон по у
        //xform.eDx = (FLOAT)1.0; //смещение по х
        //xform.eDy = (FLOAT)1.0; //смещение по y

        //SetWorldTransform(hDc, &xform);

        //HFONT fnt = CreateFont(10, 10, 0, 0, 400, 0, 0, 0, ANSI_CHARSET, 400, 0, 0, 0, "Ar ial");
        //HFONT CreateFont( 1, 1, GM_COMPATIBLE, );

        LONG sizeNew = GetWindowLong(hWnd, GWL_USERDATA);

        LOGFONT font;
        font.lfHeight = HIWORD(sizeNew);// Устанавливает высоту шрифта или символа
        font.lfWidth = 0;// Устанавливает среднюю ширину символов в шрифте
        font.lfEscapement = 0;// Устанавливает угол, между вектором наклона и осью X устройства
        font.lfOrientation = 0;// Устанавливает угол, между основной линией каждого символа и осью X устройства
        //font.lfWeight = 100;// Устанавливает толщину шрифта в диапазоне от 0 до 1000
        font.lfItalic = 0;// Устанавливает курсивный шрифт
        font.lfUnderline = 0;// Устанавливает подчеркнутый шрифт
        font.lfStrikeOut = 0;// Устанавливает зачеркнутый шрифт
        font.lfCharSet = RUSSIAN_CHARSET;// Устанавливает набор символов
        font.lfOutPrecision = 0;// Устанавливает точность вывода
        font.lfClipPrecision = 0;// Устанавливает точность отсечения
        font.lfQuality = 0;// Устанавливает качество вывода
        font.lfPitchAndFamily = 0;// Устанавливает ширину символов и семейство шрифта

        HFONT hfont;
        hfont = ::CreateFontIndirect(&font);

        SelectObject(ps.hdc, hfont);
        TextOut(hDc, (rect.right)/2, (rect.bottom)/2, text, ARRAYSIZE(text));

        EndPaint(hWnd, &ps);

        return 0;
    }

    case WM_DESTROY:

        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// + увеличить скорость
// - уменьшить скорость
// ПКМ уменьшить размер текста
// ЛКМ увеличить размер текста
// МЕНЮ: файл - выход; опции - запуск/выкл (2 опции, 1 активная, другая нет)

// WM_SIZE надо, перерисовка
// WM_PAINT надо; textOut() выдать заданную строку в точке (середине); исследуем шрифты font
// TIMER надо; WM_CREATE
// При создании шрифта есть возможность задания угла к горизонтали

// Timer задан на условную единицу, плюсик значит увеличиваем в 2, 3 раза или прибавляем