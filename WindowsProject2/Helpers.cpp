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

#include "Helpers.h"

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void LogMessage( const char* pszFormat, ...) {
	static char s_acBuf[2048]; // this here is a caveat!
	va_list args;
	va_start(args, pszFormat);
	vsprintf_s(s_acBuf, pszFormat, args);
	OutputDebugString(s2ws(s_acBuf).c_str() );
	va_end(args);
}

void GetDesktopResolution(int& horizontal, int& vertical) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

std::wstring utf8toUtf16(const std::string & str)
{
	if (str.empty())
		return std::wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	return std::wstring(&buffer[0], charsConverted);
}

std::string prependZeros(std::string s, unsigned int length) {
	while (s.length() < length) s.insert(0, "0");
	return s;
}

std::string toUpper(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

std::string WstrToUtf8Str(const std::wstring& wstr)
{
	std::string retStr;
	if (!wstr.empty())
	{
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

		if (sizeRequired > 0)
		{
			std::vector<char> utf8String(sizeRequired);
			int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
				-1, &utf8String[0], utf8String.size(), NULL,
				NULL);
			if (bytesConverted != 0)
			{
				retStr = &utf8String[0];
			}
			else
			{
				std::stringstream err;
				err << __FUNCTION__
					<< " std::string WstrToUtf8Str failed to convert wstring '"
					<< wstr.c_str() << L"'";
				throw std::runtime_error(err.str());
			}
		}
	}
	return retStr;
}


std::vector<std::string> splitString(std::string toBeSplitted, char delimiter) {
	std::istringstream ss(toBeSplitted);
	std::string token;
	std::vector<std::string> result;
	while (std::getline(ss, token, delimiter))
		result.push_back(token);
	return result;
}

std::string toHex_string(unsigned int i) {
	std::stringstream stream;
	stream << std::hex << i;
	std::string result(stream.str());
	return toUpper(prependZeros(result, 8));
}

int string_to_int(std::string s) {
	return std::stoi(s, nullptr);
}

