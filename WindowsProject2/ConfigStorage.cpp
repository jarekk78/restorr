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

std::map<std::string, std::vector<OpenWindowsStruct*>*> storage;

std::string getConfigFingerprint() {
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	int monitorCount = GetSystemMetrics(SM_CMONITORS);

	std::ostringstream stringStream;
	stringStream << vertical << "x" << horizontal << "_" << monitorCount << "_" << GetSystemMetrics(SM_XVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_YVIRTUALSCREEN)
		<< "_" << GetSystemMetrics(SM_CXVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_CYVIRTUALSCREEN);
	return stringStream.str();
}

bool configExistsForFingerprint(const std::string &configName) {
	std::map<std::string, std::vector<OpenWindowsStruct*>*>::iterator it = storage.find(configName);
	return it != storage.end();
}

WCHAR* makeWStrCopy(WCHAR* str) {
	return NULL;
}

void cleanUp(std::vector<OpenWindowsStruct*>* titles) {
	for (std::vector<OpenWindowsStruct*>::iterator it = (*titles).begin(); it != (*titles).end(); ++it) {
		free((*it)->title);
		delete *it;
	}
}

std::vector<OpenWindowsStruct*>* makeConfigCopy(std::vector<OpenWindowsStruct*>* t) {
	std::vector<OpenWindowsStruct*>* result = DBG_NEW std::vector<OpenWindowsStruct*>();
	for (std::vector<OpenWindowsStruct*>::iterator it = (*t).begin(); it != (*t).end(); ++it) {
		OpenWindowsStruct* e = (OpenWindowsStruct*)malloc(sizeof(OpenWindowsStruct));
		memcpy(e, *it, sizeof(OpenWindowsStruct));
		e->title = NULL; //  makeWStrCopy((*it)->title);
		result->push_back(e);
	} 
	return result;
}

void removeConfigIfExists(std::string configName) {
	if (configExistsForFingerprint(configName)) {
		LogMessage("Remove config %s", configName.c_str());
		std::vector<OpenWindowsStruct*>* oldConfig = storage.at(configName);
		cleanUp(oldConfig);
		storage.erase(configName);
		delete oldConfig;
	}
}

void storeConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles) {
	std::vector<OpenWindowsStruct*>* config = makeConfigCopy(titles);
	removeConfigIfExists(configName);
	storage.insert( std::pair<std::string, std::vector<OpenWindowsStruct*>*>(configName, config));
}

void moveTempConfigsOtherThanToCurrent(std::string configName) {
	// TODO
}

void removeTempConfigFor(std::string configName) {
	std::string configToBeRemoved = configName + "-temp";
	removeConfigIfExists(configToBeRemoved);
}

void applyConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles) {
	if (!configExistsForFingerprint(configName)) {
		LogMessage("No config for key %s", configName.c_str());
		return;
	}
	LogMessage("Loading: %s\n", configName.c_str());
	std::vector<OpenWindowsStruct*>* config = storage.at(configName);
	for (std::vector<OpenWindowsStruct*>::iterator configIt = (*config).begin(); configIt != (*config).end(); ++configIt) {
		OpenWindowsStruct* configEntry = *configIt;
		for (std::vector<OpenWindowsStruct*>::iterator it = titles->begin(); it != titles->end(); ++it) {
			OpenWindowsStruct entryStructP = **it;
			if (configEntry->hwnd == entryStructP.hwnd) {
				int left = configEntry->rect.left;
				int top = configEntry->rect.top;
				int right = configEntry->rect.right;
				int bottom = configEntry->rect.bottom;
				LogMessage("Restore: ");
				OutputDebugStringW(configEntry->title);
				LogMessage(", x = %d\n", left);
						SetWindowPos(entryStructP.hwnd,
							HWND_TOP,
							left,
							top,
							right - left, bottom - top,          // Ignores size arguments. 
							SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOZORDER);

			}
		}
	}
}
