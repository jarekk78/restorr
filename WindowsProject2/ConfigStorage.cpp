#include "ConfigStorage.h"

#include "stdafx.h"
#include "shellapi.h"
#include "WindowsProject2.h"
#include "Strsafe.h"
#include "Commctrl.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include "Helpers.h"
#include "Types.h"

std::string getConfigFingerprint() {
	int horizontal = 0;
	int vertical = 0;
	GetDesktopResolution(horizontal, vertical);

	int monitorCount = GetSystemMetrics(SM_CMONITORS);

	std::string *result = new std::string();
	std::ostringstream stringStream;
	stringStream << vertical << "x" << horizontal << "_" << monitorCount << "_" << GetSystemMetrics(SM_XVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_YVIRTUALSCREEN)
		<< "_" << GetSystemMetrics(SM_CXVIRTUALSCREEN) << "x" << GetSystemMetrics(SM_CYVIRTUALSCREEN);
	return stringStream.str();
}

bool configExistsForFingerprint(const std::string &configName) {
	std::ifstream f(configName.c_str());
	return f.good();
}

void storeConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles) {
	std::wofstream myfile;
	myfile.open(configName);
	myfile << "Writing this to a file.\n";
	for (std::vector<OpenWindowsStruct*>::iterator it = titles->begin(); it != titles->end(); ++it) {
		OpenWindowsStruct entryStructP = **it;
		LogMessage("Store: ");
		OutputDebugStringW(entryStructP.title);
		LogMessage(", x = %d\n", entryStructP.rect.left);
		myfile
			<< entryStructP.hwnd << '\t'
			<< entryStructP.title << '\t'
			<< entryStructP.threadId << '\t'
			<< entryStructP.processId << '\t'
			<< entryStructP.isZoomed << '\t'
			<< entryStructP.isIconic << '\t'
			<< entryStructP.rect.left << '\t'
			<< entryStructP.rect.top << '\t'
			<< entryStructP.rect.right << '\t'
			<< entryStructP.rect.bottom << std::endl;
	}
	myfile.close();
}

void cleanUp(std::vector<OpenWindowsStruct*>* titles) {
	for (std::vector<OpenWindowsStruct*>::iterator it = (*titles).begin(); it != (*titles).end(); ++it) {
		free((*it)->title);
		free(*it);
	}
}

void moveTempConfigsOtherThanToCurrent(std::string configName) {
	// TODO
}

void removeTempConfigFor(std::string configName) {
	// TODO
}

void applyConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles) {
	std::wifstream myfile;
	LogMessage("Loading: %s\n", configName.c_str());
	myfile.open(configName);
	if (myfile.good()) {
		std::wstring line;
		while (getline(myfile, line)) {
			OutputDebugStringW(L"From file: ");
			LogMessage("%s\n", WstrToUtf8Str(line).c_str() );
			std::vector<std::string> splitted = splitString(WstrToUtf8Str(line), '\t');
			if (splitted.size() > 5) {
				for (std::vector<OpenWindowsStruct*>::iterator it = titles->begin(); it != titles->end(); ++it) {
					OpenWindowsStruct entryStructP = **it;
					if (prependZeros(splitted[0], 8) == toHex_string((unsigned int)entryStructP.hwnd)) {
						int left = string_to_int(splitted[6]);
						int top = string_to_int(splitted[7]);
						int right = string_to_int(splitted[8]);
						int bottom = string_to_int(splitted[9]);
						LogMessage("Restore: %s, x=%d\n", splitted[1].c_str(), left);
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
		myfile.close();
	}
}

