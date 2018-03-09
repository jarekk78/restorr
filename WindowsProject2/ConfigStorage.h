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
std::string getConfigurationDescriptionStr();
void storeConfig(std::string configName, OWS_VEC_PTR* titles);
void moveTempConfigsOtherThanToCurrent(std::string configName);
void removeTempConfigIfExists(std::string &configName);
void applyConfig(std::string configName, OWS_VEC_PTR* titles);
void cleanUp(OWS_VEC_PTR* titles);