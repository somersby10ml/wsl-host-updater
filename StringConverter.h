#pragma once
#include <string>
#include <windows.h>

class StringConverter {
public:
	static std::wstring toWstring(const std::string& str);
	static std::string toString(const std::wstring& wstr);

	// ASCII 문자열을 현재 프로젝트 설정에 맞는 TCHAR 문자열로 변환
	static std::basic_string<TCHAR> asciiToTchar(const std::string& str) {
#ifdef UNICODE
		return toWstring(str);
#else
		return str;
#endif
	}

	// 유니코드 문자열을 현재 프로젝트 설정에 맞는 TCHAR 문자열로 변환
	static std::basic_string<TCHAR> unicodeToTchar(const std::wstring& wstr) {
#ifdef UNICODE
		return wstr;
#else
		return toString(wstr);
#endif
	}

	// TCHAR 문자열을 ASCII 문자열로 변환
	static std::string tcharToAscii(const std::basic_string<TCHAR>& tstr) {
#ifdef UNICODE
		return toString(tstr);
#else
		return tstr;
#endif
	}

	// TCHAR 문자열을 유니코드 문자열로 변환
	static std::wstring tcharToUnicode(const std::basic_string<TCHAR>& tstr) {
#ifdef UNICODE
		return tstr;
#else
		return toWstring(tstr);
#endif
	}
};