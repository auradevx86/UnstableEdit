#include "wnd.h"
#include <corecrt_wstring.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    HWND hWnd;
    MSG msg = { };
    WNDCLASSEX wc = { };
    wchar_t const szClassName[] = L"NoteWindowClass";

    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = szClassName;
    wc.hInstance = hInstance;
    wc.lpszMenuName = NULL;
    wc.cbWndExtra = 0;
    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW-4);
    wc.style = 0;

    if(!RegisterClassEx(&wc)) {
        MessageBox(HWND_DESKTOP, L"Window registration failed!", L"Fatal error", MB_OK | MB_ICONERROR);
        return -1;
    }

    hWnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        szClassName,
        L"Edit",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL
    );

    if(hWnd == NULL) {
        MessageBox(HWND_DESKTOP, L"Window creation failed!", L"Fatal error", MB_OK | MB_ICONERROR);
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);
    InitCommonControls();

    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
    case WM_CREATE:
        {
            HWND hStatus;
            HWND hEdit;
            HMENU hMenu;
            HMENU hSubMenu;

            hMenu = CreateMenu();
            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_SHOWMSG, L"&New buffer...\tCtrl+N");
            AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, L"E&xit");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&File");
            hSubMenu = CreatePopupMenu();
            AppendMenu(hSubMenu, MF_STRING, ID_EDIT_INSERT, L"&Insert random text...\tInsert");
            AppendMenu(hSubMenu, MF_STRING, ID_EDIT_SELECTALL, L"&Select &all...");
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&Edit");
            SetMenu(hWnd, hMenu);

            hStatus = CreateWindowEx(
                0,
                STATUSCLASSNAME,
                L"",
                WS_CHILD | WS_VISIBLE,
                0, 0, 0, 0,
                hWnd,
                (HMENU)ID_STATUS,
                GetModuleHandle(NULL),
                NULL
            );

            SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)L"Ready");
        }
        break;
    case WM_SIZE:
        {
            RECT WndRect;
            RECT StatusRect;
            RECT EditRect;
            HWND hStatus = GetDlgItem(hWnd, ID_STATUS);
            HWND hEdit = GetDlgItem(hWnd, ID_EDIT);
            GetClientRect(hWnd, &WndRect);
            GetWindowRect(GetDlgItem(hWnd, ID_STATUS), &StatusRect);
            GetWindowRect(hEdit, &EditRect);
            SendMessage(hStatus, WM_SIZE, 0, 0);
            SetWindowPos(hEdit, NULL, 0, 0, WndRect.right, WndRect.bottom - WndRect.top - StatusRect.bottom + StatusRect.top, SWP_NOZORDER);
        }
        break;
    case WM_COMMAND:
        {
            switch(LOWORD(wParam)) {
            case ID_FILE_SHOWMSG:
                {
                    RECT WndRect;
                    RECT StatusRect;
                    HWND hEdit;
                    GetClientRect(hWnd, &WndRect);
                    GetWindowRect(GetDlgItem(hWnd, ID_STATUS), &StatusRect);
                    int result = 0;

                    hEdit = CreateWindowEx(
                        WS_EX_CLIENTEDGE,
                        L"EDIT",
                        L"",
                        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                        0, 0, (WndRect.right - WndRect.left), (WndRect.bottom - (StatusRect.bottom - StatusRect.top)),
                        hWnd,
                        (HMENU)ID_EDIT,
                        GetModuleHandle(NULL),
                        NULL
                    );
                    
                    if(hEdit == NULL) {
                        MessageBox(hWnd, L"Failed to create Edit window!", L"Error", MB_OK | MB_ICONERROR);
                        return -1;
                    }
                    
                } break;
            case ID_EDIT_INSERT:
                {
                    srand(time(NULL));
                    int random = rand() % 2 + 1;
                    wchar_t* text;

                    switch(random) {
                        case 1: wcscpy(L"yeahhh here's some credit card info i pwned:\r\nMatthew Harris: 5078 6054 8172 6075, CVV 873, EXP 07/23\r\nSophia White: 5078 6006 0424 9915, CVV 518, EXP 04/21", text); break;
                    }
                    
                    
                } break;
            }
        }
        break;
    case WM_DESTROY:
    case WM_CLOSE:
        {
            PostQuitMessage(0);
        }
        break;
    case WM_QUIT:
        {
            // for the people who are confused why i put this here,
            // it's a joke. the while loop in the wWinMain function 
            // only breaks when GetMessage returns something that's less than 1.
            // well, WM_QUIT causes GetMessage to return 0, and WindowProc()
            // is not called. if someone managed to trigger this then please
            // tell me how. i would like to know.
            MessageBox(HWND_DESKTOP, L"how the fuck did you get here?!", L"what the... how did WindowProc() receive WM_QUIT?!", MB_OK | MB_ICONQUESTION);
        }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
