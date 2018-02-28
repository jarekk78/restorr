#pragma once

struct OpenWindowsStruct {
	HWND hwnd;
	WCHAR* title;
	DWORD threadId;
	DWORD processId;
	bool isZoomed;
	bool isIconic;
	RECT rect;
};
