#include "stdafx.h"
#include "WindowData.h"
#include <vector>
#include <string>
#include "Types.h"
#include "WindowDataEntry.h"

WindowData::WindowData(std::vector<OpenWindowsStruct*>* t) {
	for (std::vector<OpenWindowsStruct*>::iterator it = (*t).begin(); it != (*t).end(); ++it) {
		WindowDataEntry e;
		e.setHwnd((*it)->hwnd);
		e.setIsZoomed((*it)->isZoomed);
		e.setIsIconic((*it)->isIconic);
		e.setRect((*it)->rect);
		data.push_back(e);
	}
}

WindowData::~WindowData() {
	data.clear();
}

std::vector<WindowDataEntry> WindowData::getEntries() {
	return data;
}