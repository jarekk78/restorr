#pragma once

#include "Types.h"
#include "string"
#include "vector"

bool configExistsForFingerprint(const std::string &configName);
void storeConfig(std::string configName, std::vector<OpenWindowsStruct*>* titles);
void moveTempConfigsOtherThanToCurrent(std::string configName);
void removeTempConfigFor(std::string configName);
	