#pragma once

struct OpenWindowsStruct {
	HWND hwnd;
	WCHAR* title;
	DWORD threadId;
	DWORD processId;
	bool isZoomed;
	bool isIconic;
	RECT rect;
	UINT showCmd;
};

#define OWS_VEC_PTR std::vector<OpenWindowsStruct*>

struct MonitorRects
{
	std::vector<RECT>   rcMonitors;

	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
	{
		MonitorRects* pThis = reinterpret_cast<MonitorRects*>(pData);
		pThis->rcMonitors.push_back(*lprcMonitor);
		return TRUE;
	}

	MonitorRects()
	{
		EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
	}
};


