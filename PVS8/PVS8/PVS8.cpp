#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include "resource.h"

#define WINDOWTITLE "Tray"
#define VERYMAINWINDOW "overladsdasdasdasdasd"
#define BUFFERMAX 16
#define TRAYID 1001
#define TRAYMESSAGE 1002
#define IDMENUSHOW 1003
#define IDMENUEXIT 1004

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ltostr(UINT64 n, char *s, int b);
bool checkChar(char c, int radix);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = VERYMAINWINDOW;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc))
        return 0;

    HWND hMainWnd = CreateWindow(VERYMAINWINDOW, WINDOWTITLE, WS_OVERLAPPEDWINDOW,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (!hMainWnd)
        return 0;

    NOTIFYICONDATA nid = { 0 };
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hMainWnd;
    nid.uID = TRAYID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE;
    nid.uCallbackMessage = TRAYMESSAGE;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);

    Shell_NotifyIcon(NIM_ADD, &nid);

    HMENU hPopupMenu = CreatePopupMenu();

    AppendMenu(hPopupMenu, MF_STRING, IDMENUSHOW, "Показать");
    AppendMenu(hPopupMenu, MF_STRING, IDMENUEXIT, "Выход");

    SetWindowLong(hMainWnd, GWL_USERDATA, (LONG)hPopupMenu);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
        case TRAYMESSAGE:
            switch (LOWORD(lParam))
            {
                case WM_RBUTTONDOWN:
                {
                    POINT p;
                    GetCursorPos(&p);

                    SetForegroundWindow(hWnd);

                    HMENU hPopupMenu = (HMENU)GetWindowLong(hWnd, GWL_USERDATA);

                    TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_TOPALIGN, p.x, p.y, 0, hWnd, NULL);

                    break;
                }

                case WM_LBUTTONDOWN:
                    SetForegroundWindow(hWnd);
                    break;
            }
            return 0;

        case WM_COMMAND:
            switch (wParam)
            {
                case IDMENUSHOW:
                {
                    EnableMenuItem((HMENU)GetWindowLong(hWnd, GWL_USERDATA), IDMENUSHOW, MF_GRAYED);

                    DialogBox((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
                        MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);

                    break;
                }

                case IDMENUEXIT:
                    DestroyWindow(hWnd);
                    break;
            }
            return 0;

        case WM_DESTROY: 
        {
            NOTIFYICONDATA nid = { 0 };
            nid.cbSize = sizeof(PNOTIFYICONDATA);
            nid.hWnd = hWnd;
            nid.uID = TRAYID;

            Shell_NotifyIcon(NIM_DELETE, &nid);

            PostQuitMessage(0);

            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
        case WM_INITDIALOG: 
        {
            HWND editControlInput = GetDlgItem(hWnd, IDC_EDIT1);
            SendMessage(editControlInput, EM_SETLIMITTEXT, BUFFERMAX, NULL);

            HWND comboBoxInput = GetDlgItem(hWnd, IDC_COMBO1);
            HWND comboBoxOutput = GetDlgItem(hWnd, IDC_COMBO2);

            char str[3] = "\0";

            for (UINT i = 2; i <= 36; i++) {

                ltostr(i, str, 10);

                SendMessage(comboBoxInput, CB_ADDSTRING, NULL, (LPARAM)str);
                SendMessage(comboBoxOutput, CB_ADDSTRING, NULL, (LPARAM)str);

                if (i == 2) {
                    SendMessage(comboBoxInput, CB_SELECTSTRING, (WPARAM)i, (LPARAM)&str);
                    SendMessage(comboBoxOutput, CB_SELECTSTRING, (WPARAM)i, (LPARAM)&str);
                }
            }

            return TRUE;
        }

        case WM_COMMAND: 
        
            switch (wParam)
            {
                case ID_BUTTON:
                {
                    HWND editControlInput = GetDlgItem(hWnd, IDC_EDIT1);
                    HWND editControlOutput = GetDlgItem(hWnd, IDC_EDIT2);

                    HWND comboBoxInput = GetDlgItem(hWnd, IDC_COMBO1);
                    HWND comboBoxOutput = GetDlgItem(hWnd, IDC_COMBO2);

                    char str[BUFFERMAX] = { 0 }, * end;

                    GetWindowText(comboBoxInput, str, BUFFERMAX);
                    int inputNS = strtol(str, &end, 10);

                    GetWindowText(comboBoxOutput, str, BUFFERMAX);
                    int outputNS = strtol(str, &end, 10);

                    *str = 0;

                    GetWindowText(editControlInput, str, BUFFERMAX);

                    bool correct = false;

                    for (UINT i = 0; i < BUFFERMAX && str[i] != '\0'; i++) {
                        if (!checkChar(str[i], inputNS)) {
                            correct = false;
                            break;
                        }
                        else
                            correct = true;
                    }

                    SetWindowText(editControlOutput, "\0");

                    if (correct) {
                        UINT64 a = strtoull(str, &end, inputNS);
                        if (a != MAXUINT64) {

                            ltostr(a, str, outputNS);

                            SetWindowText(editControlOutput, str);
                        }
                        else
                        {
                            MessageBox(hWnd, "Слишком большое число", NULL, MB_OK);
                            SetFocus(editControlInput);
                        }
                    }
                    else
                    {
                        MessageBox(hWnd, "Неверный ввод", NULL, MB_OK);
                        SetFocus(editControlInput);
                    }

                    return TRUE;
                }

                case IDCANCEL:
                    EndDialog(hWnd, wParam);
                    EnableMenuItem((HMENU)GetWindowLong(FindWindow(VERYMAINWINDOW, WINDOWTITLE), GWL_USERDATA),
                        IDMENUSHOW, MF_ENABLED);
                    return TRUE;
            }
        
        default:
            return FALSE;
    }
}

bool checkChar(char c, int radix) {

    char str[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < radix; i++)
        if (c == str[i])
            return true;

    return false;
}

void rev_str(char *str) {

    char tmp;
    int size = 0;

    for (size; str[size] != '\0'; size++);

    size--;

    for (int i = 0; i < size; i++) {
        tmp = str[i];
        str[i] = str[size];
        str[size] = tmp;
        size--;
    }
}

void ltostr(UINT64 n, char *s, int radix) {

    if (radix >= 2 && radix <= 36) {

        char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        UINT i = 0;
        s[i++] = digits[n % radix];
        n /= radix;

        while (n > 0) {
            s[i++] = digits[n % radix];
            n /= radix;
        }

        s[i] = '\0';

        rev_str(s);
    }
}