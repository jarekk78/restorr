#include "ConfigStorage.h"

#include "stdafx.h"
#include "shellapi.h"
#include "WindowsProject2.h"
#include "Strsafe.h"
#include "Commctrl.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include "Helpers.h"
#include "Types.h"
#include <cstdlib>
#include "Debug.h"
#include "WindowData.h"

std::map<std::string, WindowData> storage;
std::map<std::string, WindowData> temporaryStorage;

std::string getConfigFingerprint() {
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	int monitorCount = GetSystemMetrics(SM_CMONITORS);

	std::ostringstream stringStream;
	stringStream << horizontal << "x" << vertical << "_" << monitorCount << "_" << GetSystemMetrics(SM_XVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_YVIRTUALSCREEN)
		<< "_" << GetSystemMetrics(SM_CXVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_CYVIRTUALSCREEN);
	return stringStream.str();
}

std::string getConfigurationDescriptionStr() {
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	int monitorCount = GetSystemMetrics(SM_CMONITORS);

	std::ostringstream stringStream;
	stringStream << 
		monitorCount << " display" << ((monitorCount>1)?"s":"") << ", resolution: " << 
		horizontal << "x" << vertical << 
		" (virtual screen: " << GetSystemMetrics(SM_CXVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_CYVIRTUALSCREEN)
		<< ", main display at " << -GetSystemMetrics(SM_XVIRTUALSCREEN) << "x" << -GetSystemMetrics(SM_YVIRTUALSCREEN)
		<< ")";
	return stringStream.str();
}

bool configExistsForFingerprint(const std::string &configName) {
	std::map<std::string, WindowData>::iterator it = storage.find(configName);
	return it != storage.end();
}

bool tempConfigExistsForFingerprint(const std::string &configName) {
	std::map<std::string, WindowData>::iterator it = temporaryStorage.find(configName);
	return it != temporaryStorage.end();
}

WCHAR* makeWStrCopy(WCHAR* str) {
	return NULL;
}

void cleanUp(OWS_VEC_PTR* titles) {
	for (std::vector<OpenWindowsStruct*>::iterator it = (*titles).begin(); it != (*titles).end(); ++it) {
		free((*it)->title);
		delete *it;
	}
}

void removeConfigIfExists( std::string &configName) {
	if (configExistsForFingerprint(configName)) {
		WindowData oldConfig = storage.at(configName);
		storage.erase(configName);
	}
}

void removeTempConfigIfExists(std::string &configName) {
	if (tempConfigExistsForFingerprint(configName)) {
		WindowData oldConfig = temporaryStorage.at(configName);
		temporaryStorage.erase(configName);
	}
}

void storeConfig(std::string configName, OWS_VEC_PTR* titles) {
	removeConfigIfExists(configName);
	storage.insert(std::pair<std::string, WindowData>(configName, WindowData(titles)));
}

void moveTempConfigsOtherThanToCurrent(std::string configName) {
	std::vector<std::string> toBeMovedToMainStorage;
	for (std::map<std::string, WindowData>::iterator it = temporaryStorage.begin(); it != temporaryStorage.end(); ++it) {
		std::string storedConfigName = it->first;
		if (storedConfigName != configName) {
			toBeMovedToMainStorage.push_back(storedConfigName);
		}
	}

	for (std::vector<std::string>::iterator it = toBeMovedToMainStorage.begin(); it != toBeMovedToMainStorage.end(); ++it) {
		std::string tempConfigName = *it;
		storage.insert(std::pair<std::string, WindowData>(configName, temporaryStorage.at(tempConfigName)));
		temporaryStorage.erase(tempConfigName);
	}
}

void moveWindowToNearestScreen(LONG &left, LONG &top, LONG &right, LONG &bottom, MonitorRects &monitors) {
	if (left == -32000 && top == -32000) return;

//	RECT windowTitleRect;
	int mCaptionHeight = (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
	for (std::vector<RECT>::iterator it = monitors.rcMonitors.begin(); it != monitors.rcMonitors.end(); ++it) {
//		*it->
	}
}

void applyConfig(std::string configName, OWS_VEC_PTR* titles) {
	MonitorRects monitors;
		
	if (!configExistsForFingerprint(configName)) {
		LogMessage("No config for key %s", configName.c_str());
		return;
	}
	WindowData config = storage.at(configName);
	std::vector<WindowDataEntry> entries = config.getEntries();
	for (std::vector<WindowDataEntry>::iterator configIt = entries.begin(); configIt != entries.end(); ++configIt) {
		WindowDataEntry configEntry = *configIt;
		for (std::vector<OpenWindowsStruct*>::iterator it = titles->begin(); it != titles->end(); ++it) {
			OpenWindowsStruct entryStructP = **it;
			if (configEntry.hasHwnd(entryStructP.hwnd)) {
				LONG left = configEntry.getLeft();
				LONG top = configEntry.getTop();
				LONG right = configEntry.getRight();
				LONG bottom = configEntry.getBottom();
				moveWindowToNearestScreen(left, top, right, bottom, monitors);
				WINDOWPLACEMENT windowPlacement;
				windowPlacement.length = sizeof(WINDOWPLACEMENT);
				windowPlacement.rcNormalPosition.left = left;
				windowPlacement.rcNormalPosition.top = top;
				windowPlacement.rcNormalPosition.right = right;
				windowPlacement.rcNormalPosition.bottom = bottom;
				windowPlacement.showCmd = configEntry.getShowCmd();
				if (windowPlacement.showCmd != SW_MAXIMIZE && windowPlacement.showCmd != SW_SHOWMINIMIZED) windowPlacement.showCmd = SW_SHOWNORMAL;
				SetWindowPlacement(entryStructP.hwnd, &windowPlacement);
				GetWindowPlacement(entryStructP.hwnd, &windowPlacement);
				if (windowPlacement.showCmd == SW_SHOWNORMAL && windowPlacement.rcNormalPosition.left == left && windowPlacement.rcNormalPosition.top == top) {
					SetWindowPos(entryStructP.hwnd, HWND_TOP,
						left, top, configEntry.getRight() - left, configEntry.getBottom() - top,
						SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER);
				}
			}
		}
	}
}
