#pragma once
#include "stdafx.h"
#include <vector>
#include <string>
#include "Types.h"
#include "WindowDataEntry.h"

class WindowData {
public:
	WindowData(std::vector<OpenWindowsStruct*>* titles);
	~WindowData();
	std::vector<WindowDataEntry> getEntries();
private:
	std::vector<WindowDataEntry> data;
};

