#pragma once
#include <string>
#include <windows.h>

class StringConverter {
public:
	static std::wstring toWstring(const std::string& str);
	static std::string toString(const std::wstring& wstr);

	// ASCII ���ڿ��� ���� ������Ʈ ������ �´� TCHAR ���ڿ��� ��ȯ
	static std::basic_string<TCHAR> asciiToTchar(const std::string& str) {
#ifdef UNICODE
		return toWstring(str);
#else
		return str;
#endif
	}

	// �����ڵ� ���ڿ��� ���� ������Ʈ ������ �´� TCHAR ���ڿ��� ��ȯ
	static std::basic_string<TCHAR> unicodeToTchar(const std::wstring& wstr) {
#ifdef UNICODE
		return wstr;
#else
		return toString(wstr);
#endif
	}

	// TCHAR ���ڿ��� ASCII ���ڿ��� ��ȯ
	static std::string tcharToAscii(const std::basic_string<TCHAR>& tstr) {
#ifdef UNICODE
		return toString(tstr);
#else
		return tstr;
#endif
	}

	// TCHAR ���ڿ��� �����ڵ� ���ڿ��� ��ȯ
	static std::wstring tcharToUnicode(const std::basic_string<TCHAR>& tstr) {
#ifdef UNICODE
		return tstr;
#else
		return toWstring(tstr);
#endif
	}
};