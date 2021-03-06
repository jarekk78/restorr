// WindowsProject2.cpp : Defines the entry point for the application.
// https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads
// https://bitbucket.org/ejarkac/restorr

#include "stdafx.h"
#include "shellapi.h"
#include "Strsafe.h"
#include "Commctrl.h"
#include <vector>
#include <string>
#include <Windows.h>
#include "WindowsProject2.h"
#include "Helpers.h"
#include "Types.h"
#include "ConfigStorage.h"
#include "Debug.h"
#include <cstring>

#define MAX_LOADSTRING 100
#define TIMER_PERIOD 1000
#define ABOUT_BOX_MSG_STR	L"Restorr ver. 0.2 (2018-04-03)"

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
std::string previousConfigurationFingerprint = "";
bool restorableConfigurationExists = false;
std::vector<OpenWindowsStruct*> titles;
bool autoRestoreEnabled = false;
bool shouldRunAtAtartup;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
#if defined(MEM_LEAKS_DETECTION)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT2));

    MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
    return (int) msg.wParam;
}

void showAboutWindow() {
	MessageBox(
		NULL,
		(LPCWSTR)ABOUT_BOX_MSG_STR,
		(LPCWSTR)L"About Restorr",
		MB_ICONINFORMATION | MB_OK
	);
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
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

LONG GetDWORDRegKey(HKEY hKey, const std::wstring &strValueName, DWORD &nValue, DWORD nDefaultValue)
{
	nValue = nDefaultValue;
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD nResult(0);
	LONG nError = ::RegQueryValueExW(hKey,
		strValueName.c_str(),
		0,
		NULL,
		reinterpret_cast<LPBYTE>(&nResult),
		&dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		nValue = nResult;
	}
	return nError;
}

LONG GetBoolRegKey(HKEY hKey, const std::wstring &strValueName, bool &bValue, bool bDefaultValue)
{
	DWORD nDefValue((bDefaultValue) ? 1 : 0);
	DWORD nResult(nDefValue);
	LONG nError = GetDWORDRegKey(hKey, strValueName.c_str(), nResult, nDefValue);
	if (ERROR_SUCCESS == nError)
	{
		bValue = (nResult != 0) ? true : false;
	}
	else bValue = bDefaultValue;
	return nError;
}

void SetBoolRegKey(HKEY hKey, const std::wstring &strValueName, bool bValue)
{
	DWORD value = bValue;
	RegSetValueEx(hKey, strValueName.c_str(), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
}


LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
{
	strValue = strDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		strValue = szBuffer;
	}
	return nError;
}

HKEY getOptionsHKey() {
	HKEY hKey;
	LONG lRes = RegCreateKey(HKEY_CURRENT_USER, L"Software\\Restorr", &hKey);
	return hKey;
}

void setRunAtStartup(bool runAtStartupEnabled) {
	HKEY hkey = NULL;

	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	GetModuleFileNameW(hModule, path, MAX_PATH);

	RegCreateKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);
	if (runAtStartupEnabled) {
		RegSetValueEx(hkey, L"Restorr", 0, REG_SZ, (LPBYTE)path, _tcslen(path) * sizeof(TCHAR));
	}
	else {
		RegDeleteValue(hkey, L"Restorr");
	}

	SetBoolRegKey(getOptionsHKey(), L"shouldRunAtStartup", shouldRunAtAtartup);
}

/*
	FUNCTION: InitInstance(HINSTANCE, int)
	PURPOSE: Saves instance handle and creates main window
	COMMENTS:
		In this function, we save the instance handle in a global variable and create and display the main program window.
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    GetBoolRegKey(getOptionsHKey(), L"shouldRunAtStartup", shouldRunAtAtartup, true);
	SetBoolRegKey(getOptionsHKey(), L"shouldRunAtStartup", shouldRunAtAtartup);
	setRunAtStartup(shouldRunAtAtartup);
	if (shouldRunAtAtartup) 
		OutputDebugStringW(L"shouldRunAtAtartup from registry: true\n");
	else
		OutputDebugStringW(L"shouldRunAtAtartup from registry: false\n");

   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_BORDER| WS_CAPTION| WS_SYSMENU, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd) return FALSE;

   // add icon
   nid.uVersion = NOTIFYICON_VERSION_4;
   nid.cbSize = sizeof(nid);
   nid.hWnd = hWnd;
   nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_GUID | NIF_INFO;
   static const GUID myGUID =
#ifndef NDEBUG
   { 0x20d4df0, 0x12d7, 0x430e,{ 0x93, 0x7b, 0x1d, 0x54, 0xAC, 0x4A, 0x42, 0xB3 } };
#else
   { 0xcfb68f1, 0xb127, 0xa45c, { 0x8a, 0xef, 0x00, 0x88, 0x4b, 0x28, 0x26, 0xa4 } };

#endif
   nid.guidItem = myGUID;
   StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Test application");
   // Load the icon for high DPI.
   LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_SMALL), LIM_SMALL, &(nid.hIcon));
   nid.uCallbackMessage = WM_SYSICON;

//   if (Shell_NotifyIcon(NIM_DELETE, &nid)) {
//   }
   if (Shell_NotifyIcon(NIM_ADD, &nid) && Shell_NotifyIcon(NIM_SETVERSION, &nid)) {
	   nCmdShow = SW_HIDE;
	   ShowWindow(hWnd, nCmdShow);
	   UpdateWindow(hWnd);
   }
   else {
	   MessageBox(NULL, _T("Application instance is already running."), _T("Restorr"), MB_OK | MB_SYSTEMMODAL);
	   OutputDebugStringW(L"Failed to create icon.\n");
	   return FALSE;
   }
   return TRUE;
}

void setCheckedMenuItem(bool checked, UINT menuItem) {
	CheckMenuItem(Hmenu, menuItem, MF_BYCOMMAND | (checked?MF_CHECKED:MF_UNCHECKED));
}

void enableDisableMenuItems() {
	EnableMenuItem(Hmenu, ID_TRAY_STORE, (!timerState) ? MF_ENABLED : MF_DISABLED);
	EnableMenuItem(Hmenu, ID_TRAY_AUTO, (timerState) ? MF_ENABLED : MF_DISABLED);
}

void setPauseCheckbox() {
	setCheckedMenuItem(!timerState, ID_TRAY_PAUSE);
	enableDisableMenuItems();
}

void setAutoRestoreCheckbox() {
	setCheckedMenuItem(autoRestoreEnabled, ID_TRAY_AUTO);
	enableDisableMenuItems();
}

BOOL ShowBalloon(LPCWSTR pszTitle, LPCWSTR pszText)
{
	nid.dwInfoFlags = NIIF_INFO | NIIF_NOSOUND;
	StringCchCopy(nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle), pszTitle);
	StringCchCopy(nid.szInfo, ARRAYSIZE(nid.szInfo), pszText);
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void toggleAuto() {
	autoRestoreEnabled = !autoRestoreEnabled;
	setAutoRestoreCheckbox();
}

void toggleShouldRunAtStartup() {
	if (shouldRunAtAtartup) {
		OutputDebugStringW(L"toggleShouldRunAtStartup was: true\n");
	}
	else {
		OutputDebugStringW(L"toggleShouldRunAtStartup was: false\n");
	}
	shouldRunAtAtartup = !shouldRunAtAtartup;
	setCheckedMenuItem(shouldRunAtAtartup, ID_TRAY_RUNATSTARTUP);
	SetBoolRegKey(getOptionsHKey(), L"shouldRunAtStartup", shouldRunAtAtartup);
	setRunAtStartup(shouldRunAtAtartup);
}

void toggleTimer(HWND hWnd) {
	if (timerState) {
		KillTimer(hWnd, IDT_TIMER1);
		Shell_NotifyIcon(NIM_MODIFY, &nid);
//		ShowBalloon(L"Paused", L"Detection of resolution changes and storage of window positions paused.");
	}
	else {
		SetTimer(hWnd, IDT_TIMER1, TIMER_PERIOD, (TIMERPROC)NULL);
//		ShowBalloon(L"Started", L"Application is detection resolution changes and storing window positions.");
	}
	timerState = !timerState;
	setPauseCheckbox();
}

BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam) {
	const DWORD TITLE_SIZE = 1024;
	WCHAR windowTitle[TITLE_SIZE];

	GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

	int length = ::GetWindowTextLength(hwnd);
	std::wstring title(&windowTitle[0]);
	if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager" || title == L"Start") {
		return TRUE;
	}

	OpenWindowsStruct* openWindow = (OpenWindowsStruct*)malloc(sizeof(OpenWindowsStruct));
	WCHAR* titleWC = (WCHAR*)malloc(sizeof(WCHAR)*TITLE_SIZE);
	memcpy(titleWC, windowTitle, sizeof(WCHAR)*TITLE_SIZE);

	std::vector<OpenWindowsStruct*>& titles =
		*reinterpret_cast<std::vector<OpenWindowsStruct*>*>(lParam);
	titles.push_back(openWindow);

	DWORD processId;
	DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);

	bool isZoomed = IsZoomed(hwnd);
	bool isIconic = IsIconic(hwnd);

	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	
	WINDOWPLACEMENT windowPlacement;
	GetWindowPlacement(hwnd, &windowPlacement);

	openWindow->title = titleWC;
	openWindow->hwnd = hwnd;
	openWindow->threadId = threadId;
	openWindow->processId = processId;
	openWindow->isZoomed = isZoomed;
	openWindow->isIconic = isIconic;
	openWindow->rect = windowRect;
	openWindow->showCmd = windowPlacement.showCmd;
#ifndef NDEBUG
	if (isZoomed) LogMessage("Zoomed");
	if (isIconic) LogMessage("Iconic");
#endif
	if (isZoomed || isIconic) {
		openWindow->rect.left = windowPlacement.rcNormalPosition.left;
		openWindow->rect.top = windowPlacement.rcNormalPosition.top;
		openWindow->rect.right = windowPlacement.rcNormalPosition.right;
		openWindow->rect.bottom = windowPlacement.rcNormalPosition.bottom;
#ifndef NDEBUG
		LogMessage(">>  [%d,%d,%d,%d] (%d)\n", openWindow->rect.left, openWindow->rect.top, openWindow->rect.right, openWindow->rect.bottom, windowPlacement.showCmd);
#endif
	}

#ifndef NDEBUG
	OutputDebugStringW(openWindow->title);
	LogMessage(" [%d,%d,%d,%d]\n", openWindow->rect.left, openWindow->rect.top, openWindow->rect.right, openWindow->rect.bottom);
#endif
	return TRUE;
}

void getCurrentConfiguration( bool doStore, bool doCleanup ) {
	std::string newConfigFingerprint = getConfigFingerprint();
	titles.clear();
	EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&titles));
	if (doStore) {
		storeConfig(newConfigFingerprint + ((restorableConfigurationExists) ? "-temp" : ""), &titles);
	}
	moveTempConfigsOtherThanToCurrent(newConfigFingerprint);
	if (doCleanup) {
		cleanUp(&titles);
		titles.clear();
	}
}

void restoreConfiguration() {
	OutputDebugStringW(L"Restoring...\n");
	bool doCleanup = false;
	bool doStore = false;
	getCurrentConfiguration(doStore, doCleanup);
	std::string configName = getConfigFingerprint();
	applyConfig(configName, &titles);
	restorableConfigurationExists = false;
	removeTempConfigIfExists(configName);
	cleanUp(&titles);
	titles.clear();
}

void onTimer() {
//	OutputDebugStringW(L"Store\n");
	MonitorRects monitors;
	LogMessage("You have %d monitors connected.\n", monitors.rcMonitors.size());
	for (std::vector<RECT>::iterator it = monitors.rcMonitors.begin(); it != monitors.rcMonitors.end(); ++it) {
		RECT r = *it;
		LogMessage("Monitor %d, %d, %d, %d\n", r.left, r.top, r.right, r.bottom);
	}
//	LogMessage( getConfigurationDescriptionStr().c_str() );
//	if (autoRestoreEnabled) OutputDebugStringW(L"Would auto restore\n");
	std::string newConfigFingerprint = getConfigFingerprint();
	bool configurationHasChanged = previousConfigurationFingerprint != "" && newConfigFingerprint != previousConfigurationFingerprint;
	if (configurationHasChanged) {
		std::wstring concatted_stdstr = L"Change of screen configuration detected: " + utf8toUtf16(getConfigurationDescriptionStr()) + L".";
		if (configExistsForFingerprint(newConfigFingerprint)) {
			if (autoRestoreEnabled) {
				concatted_stdstr += L"\n\nWindow positions will be restored.";
			}
			else {
				concatted_stdstr += L"\n\nClick here to restore window positions.";
			}
			restorableConfigurationExists = true;
		}
		else {
			restorableConfigurationExists = false;
		}
		LPCWSTR concatted = concatted_stdstr.c_str();
		ShowBalloon(L"Screen configuration change", concatted);
	}
	previousConfigurationFingerprint = newConfigFingerprint;
	bool doCleanup = true;
	bool doStore = true;
	getCurrentConfiguration(doStore, doCleanup);

	if (restorableConfigurationExists && autoRestoreEnabled)
		restoreConfiguration();
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
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_ABOUT, TEXT("About Restorr"));
			AppendMenu(Hmenu, MF_SEPARATOR, 0, TEXT(""));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_STORE, TEXT("Store"));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_RESTORE, TEXT("Restore"));
			AppendMenu(Hmenu, MF_SEPARATOR, 0, TEXT(""));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_PAUSE, TEXT("Pause"));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_AUTO, TEXT("Automatically restore window positions"));
			AppendMenu(Hmenu, MF_SEPARATOR, 0, TEXT(""));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_RUNATSTARTUP, TEXT("Run application at startup"));
			AppendMenu(Hmenu, MF_SEPARATOR, 0, TEXT(""));
			AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

			toggleTimer(hWnd);
			toggleAuto();
			if (shouldRunAtAtartup) {
				shouldRunAtAtartup = false;
				toggleShouldRunAtStartup();
			}
		}
		break;
	case WM_TIMER:
		{
		switch (wParam) {
		case IDT_TIMER1: 
			onTimer();
			break;
		}
		}
		break;
	case WM_SYSICON:
		{
			int wmId = LOWORD(lParam);
			switch (wmId)
			{
			case WM_LBUTTONUP: { // show window
//					ShowWindow(hWnd, SW_SHOW);
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
					}
					if (clicked == ID_TRAY_AUTO) {
						toggleAuto();
					}
					if (clicked == ID_TRAY_STORE) {
						onTimer();
					}
					if (clicked == ID_TRAY_RESTORE) {
						restoreConfiguration();
					}
					if (clicked == ID_TRAY_RUNATSTARTUP) {
						toggleShouldRunAtStartup();
					}
					if (clicked == ID_TRAY_ABOUT) {
						showAboutWindow();
					}
			} break;
			case NIN_BALLOONUSERCLICK: {
				if (restorableConfigurationExists) {
					restoreConfiguration();
//					ShowBalloon(L"Debug", L"Restoring...");
/*					std::string configName = getConfigFingerprint();
					applyConfig(configName, &titles);
					restorableConfigurationExists = false;
					removeTempConfigIfExists(configName);
					*/
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
