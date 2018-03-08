#pragma once

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

void GetDesktopResolution(int& horizontal, int& vertical);
std::wstring utf8toUtf16(const std::string & str);
std::string prependZeros(std::string s, unsigned int length);
std::string toUpper(std::string str);
std::string WstrToUtf8Str(const std::wstring& wstr);
std::vector<std::string> splitString(std::string toBeSplitted, char delimiter);
std::string toHex_string(unsigned int i);
int string_to_int(std::string s);
void LogMessage( const char* pszFormat, ...);
bool startsWith(std::string s, std::string prefix);