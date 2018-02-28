#include "ConfigStorage.h"
#include "string"
#include "stdafx.h"
#include "shellapi.h"
#include "Strsafe.h"
#include "Commctrl.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <Windows.h>
#include "Types.h"

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
		myfile
			<< entryStructP.hwnd << '\t'
			<< entryStructP.title << '\t'
			<< entryStructP.threadId << '\t'
			<< entryStructP.processId << '\t'
			<< entryStructP.isZoomed << '\t'
			<< entryStructP.isIconic << '\t'
			<< entryStructP.rect.left << '\t'
			<< entryStructP.rect.top << std::endl;
	}
	myfile.close();
}

void moveTempConfigsOtherThanToCurrent(std::string configName) {
	// TODO
}

void removeTempConfigFor(std::string configName) {
	// TODO
}