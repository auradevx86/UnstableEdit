#include "wnd.h"
#include <corecrt_wstring.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EditWindowProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
void SetText(HWND);

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
    srand(time(NULL));

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
            HWND hEdit = GetDlgItem(hWnd, ID_EDIT);
            HWND hStatus = GetDlgItem(hWnd, ID_STATUS);
            GetClientRect(hWnd, &WndRect);
            GetWindowRect(GetDlgItem(hWnd, ID_STATUS), &StatusRect);
            GetWindowRect(GetDlgItem(hWnd, ID_EDIT), &EditRect);
            SendMessage(hStatus, WM_SIZE, 0, 0);
            SetWindowPos(hEdit, NULL, 0, 0, WndRect.right, WndRect.bottom - WndRect.top - StatusRect.bottom + StatusRect.top, SWP_NOZORDER);
        }
        break;
    case WM_KEYDOWN:
        {
            if(HIBYTE(GetKeyState(VK_INSERT))) {
                SetText(hWnd);
            }
            if(GetKeyState(VK_CONTROL) & wParam == 'A') {
                SendMessage(GetDlgItem(hWnd, ID_EDIT), EM_SETSEL, 0, -1);
            }
        } break;
    case WM_KEYUP:
        {
            GetKeyState(VK_CONTROL);
        } break;
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
                    
                    SetWindowSubclass(hEdit, &EditWindowProc, 0, 0);

                } break;
            case ID_EDIT_INSERT:
                {
                    SetText(GetDlgItem(hWnd, ID_EDIT));
                } break;
            case ID_FILE_EXIT:
                {
                    SendMessage(hWnd, WM_CLOSE, 0, 0);
                }
            }
        }
        break;
    case WM_DESTROY:
    case WM_CLOSE:
        {
            if(GetWindowTextLength(GetDlgItem(hWnd, ID_EDIT)) > 0) {
                int result = MessageBox(hWnd, L"Save changes to the current buffer?", L"Edit", MB_YESNOCANCEL | MB_ICONQUESTION);
                if(result == IDYES) {
                    MessageBox(hWnd, L"oh yeah this doesn't have a save function so see ya!", L"troll'd", MB_OK);
                }
                else if(result == IDCANCEL) {
                    return 0;
                }
            }
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

LRESULT CALLBACK EditWindowProc(HWND hEdit, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    if(uMsg == WM_KEYDOWN & wParam == VK_INSERT) {
        SetText(hEdit);
    }
    else if(uMsg == WM_KEYDOWN & HIBYTE(GetKeyState(VK_CONTROL)) & wParam == 'A') {
        SendMessage(hEdit, EM_SETSEL, 0, -1);
    }
    return DefSubclassProc(hEdit, uMsg, wParam, lParam);
}


// please enter the hwnd that contains the edit dialog
void SetText(HWND hEdit) {
    int random = rand() % 30;
    int length = GetWindowTextLength(hEdit);
    wchar_t* temp = new wchar_t[length];
    GetWindowText(hEdit, temp, length);
    std::wstring text(temp);

    switch(random) {
    case 1: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nyeahhh here's piece of credit card info i pwned:\r\nMatthew Harris: 5078 6054 8172 6075, CVV 873, EXP 04/28  ").c_str()); break;
    case 2: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa  ").c_str());
    case 3: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nFUCK YOU POLITICAL CORRECTNESS  ").c_str()); break;
    case 4: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\n（アニメ）ニャー、一、二、三、ニャー、ありがとう  ").c_str()); break;
    case 5: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nhjonk hjonk am goose  ").c_str()); break;
    case 6: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nyour next service to depart from platform 1 will be the 7:49 flinders street stopping all stations to flinders street except east richmond, departing in 10 hours.  ").c_str()); break;
    case 7: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nj.k rowling is a cunt  ").c_str()); break;
    case 8: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\ngo watch oshi no ko  ").c_str()); break;
    case 9: SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\ntay tay not today day\r\nmiku is better than taylor swift  ").c_str()); break;
    case 10:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nTHE CAKE IS A LIE  ").c_str()); break;
    case 11:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\n近前看其詳上寫著  ").c_str()); break;
    case 12:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nthe aperture science enrichment center would like to remind you that these people are cunts:\r\ntaylor swift\r\nj.k. rowling\r\nwheatley  ").c_str()); break;
    case 13:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nlove bluh bluh bluh bluh bluh bluh bluh bluh  ").c_str()); break;
    case 14:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\noo ee oo  ").c_str()); break;
    case 15:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nfuck bingo  ").c_str()); break;
    case 16:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\n十六じゅうろく      ").c_str()); break;
    case 17:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nfun fact: rand() returned 17  ").c_str()); break;
    case 18:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nall my fellas  ").c_str()); break;
    case 19:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nsoundcloud > spotify  ").c_str()); break;
    case 20:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nqwertyuiopasdfghjklzxcvbnm  ").c_str()); break;
    case 21:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\num... akually... your FACE  ").c_str()); break;
    case 22:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\n\\0  ").c_str()); break;
    case 23:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\ntake bell  ").c_str()); break;
    case 24:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\ntouhou girls have arrived at your location  ").c_str()); break;
    case 25:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nF.O.C.U.S\r\nfuck off cuz ur stupid  ").c_str()); break;
    case 26:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\ndhar mann endorses political correctness  ").c_str()); break;
    case 27:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nziggy from national tiles  ").c_str()); break;
    case 28:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nYOU KNOW WHO ELSE IS THE HONOURED ONE  ").c_str()); break;
    case 29:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nD-I-L-L-I-G-A-F, DILLIGAF!\r\nand it means...\r\ndo i look like i give a fuck?  ").c_str()); break;
    case 30:SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)(text + L"\r\nfun fact: rand() returned 30").c_str()); break;
    }     

    SendMessage(hEdit, EM_SETSEL, (WPARAM)(length + 1), (LPARAM)(length + 1));
    SendMessage(hEdit, EM_SCROLLCARET, 0, 0);
} 
