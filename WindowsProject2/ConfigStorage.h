#pragma once

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

std::string getConfigFingerprint();
bool configExistsForFingerprint(const std::string &configName);
void storeConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles);
void cleanUp(std::vector<OpenWindowsStruct*>* titles);
void moveTempConfigsOtherThanToCurrent(std::string configName);
void removeTempConfigFor(std::string configName);
void applyConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles);
