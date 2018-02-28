#pragma once

std::wstring utf8toUtf16(const std::string & str);

std::string prependZeros(std::string s, unsigned int length);
std::string toUpper(std::string str);
std::string WstrToUtf8Str(const std::wstring& wstr);
std::vector<std::string> splitString(std::string toBeSplitted, char delimiter);
std::string toHex_string(unsigned int i);
int string_to_int(std::string s);