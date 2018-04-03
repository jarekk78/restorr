#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include "Types.h"

class WindowDataEntry {
public:
	WindowDataEntry();
	void setHwnd(HWND hwnd);
	void setIsZoomed(bool isZoomed);
	void setIsIconic(bool isIconic);
	void setRect(RECT rect);
	HWND getHwnd();
	bool getIsZoomed();
	bool getIsIconic();
	LONG getLeft();
	LONG getTop();
	LONG getRight();
	LONG getBottom();
	bool hasHwnd(HWND h);
	void setShowCmd(UINT showCmd);
	UINT getShowCmd();
private:
	HWND hwnd;
	std::wstring title;
	DWORD threadId;
	DWORD processId;
	bool isZoomed;
	bool isIconic;
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
	UINT showCmd;
};