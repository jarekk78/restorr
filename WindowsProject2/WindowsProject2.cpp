// WindowsProject2.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "shellapi.h"
#include "WindowsProject2.h"
#include "Strsafe.h"
#include "Commctrl.h"

#define MAX_LOADSTRING 100

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HMENU Hmenu;
NOTIFYICONDATA nid = {};
bool timerState = false;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

//	nCmdShow = SW_HIDE;
    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_BORDER| WS_CAPTION| WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // add icon
   nid.uVersion = NOTIFYICON_VERSION_4;
   nid.cbSize = sizeof(nid);
   nid.hWnd = hWnd;
   nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_GUID;
   static const GUID myGUID =
   { 0x20d4df0, 0x12d7, 0x430e,{ 0x93, 0x7b, 0x1d, 0x54, 0xAC, 0x4A, 0x42, 0xB2 } };
   nid.guidItem = myGUID;
   StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Test application");
   // Load the icon for high DPI.
   LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_SMALL), LIM_SMALL, &(nid.hIcon));
   nid.uCallbackMessage = WM_SYSICON;
   if (Shell_NotifyIcon(NIM_ADD, &nid)) {
	   Shell_NotifyIcon(NIM_SETVERSION, &nid);

	   nCmdShow = SW_HIDE;
	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);
   }
   return TRUE;
}

void setPauseCheckbox() {
	if (!timerState)
		CheckMenuItem(Hmenu, ID_TRAY_PAUSE, MF_BYCOMMAND | MF_CHECKED);
	else
		CheckMenuItem(Hmenu, ID_TRAY_PAUSE, MF_BYCOMMAND | MF_UNCHECKED);
}

BOOL ShowBalloon(LPCWSTR pszTitle, LPCWSTR pszText)
{
	nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
	StringCchCopy(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle), pszTitle);
	StringCchCopy(nid.szInfo, ARRAYSIZE(nid.szInfo), pszText);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL ShowPrintJobBalloon()
{
	return ShowBalloon(L"Some title", L"Some text...");
}


void toggleTimer(HWND hWnd) {
	if (timerState) {
		KillTimer(hWnd, IDT_TIMER1);
		timerState = false;
		ShowPrintJobBalloon();

		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
	else {
		SetTimer(hWnd, IDT_TIMER1, 1000, (TIMERPROC)NULL);
		timerState = true;
	}
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

	case WM_CREATE:
		{
			Hmenu = CreatePopupMenu();
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_PAUSE, TEXT("Pause"));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

			toggleTimer(hWnd);
		}
		break;
	case WM_TIMER:
		{
		switch (wParam) {
		case IDT_TIMER1: 
			OutputDebugStringW(L"Timer1.");
			break;
		}
		}
		break;
	case WM_SYSICON:
		{
			int wmId = LOWORD(lParam);
			switch (wmId)
			{
			case WM_LBUTTONUP: {
					ShowWindow(hWnd, SW_SHOW);
				} break;
			case WM_RBUTTONDOWN: {
					POINT curPoint;
					GetCursorPos(&curPoint);
					SetForegroundWindow(hWnd);
					UINT clicked = TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hWnd, NULL);
					SendMessage(hWnd, WM_NULL, 0, 0);
					if (clicked == ID_TRAY_EXIT) {
						Shell_NotifyIcon(NIM_DELETE, &nid);
						PostQuitMessage(0);
					}
					if (clicked == ID_TRAY_PAUSE) {
						toggleTimer(hWnd);
						setPauseCheckbox();
					}
			} break;
			}
		}
		break;
	case WM_SIZE:
	{
		if (wParam == SIZE_MINIMIZED) {
			ShowWindow(hWnd, SW_HIDE);
		}
	}
	break;
	case WM_CLOSE:
	{
			ShowWindow(hWnd, SW_HIDE);
			return 0;
	}
	break;
	case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
