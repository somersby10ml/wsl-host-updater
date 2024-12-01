#include "StringConverter.h"


// ASCII(MultiByte) -> Unicode (std::string -> std::wstring)
inline std::wstring StringConverter::toWstring(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	// 필요한 버퍼 크기 계산
	int size = MultiByteToWideChar(
		CP_ACP,                 // ASCII 코드 페이지
		0,                      // 기본 플래그
		str.c_str(),           // 입력 문자열
		static_cast<int>(str.length()), // 입력 문자열 길이
		nullptr,               // 출력 버퍼 (크기 계산용이므로 nullptr)
		0                      // 출력 버퍼 크기 (크기 계산용이므로 0)
	);

	// 버퍼 할당 및 변환
	std::wstring result(size, 0);
	MultiByteToWideChar(
		CP_ACP,
		0,
		str.c_str(),
		static_cast<int>(str.length()),
		&result[0],           // 출력 버퍼
		size                  // 버퍼 크기
	);

	return result;
}

// Unicode -> ASCII(MultiByte) (std::wstring -> std::string)
inline std::string StringConverter::toString(const std::wstring& wstr) {
	if (wstr.empty()) {
		return std::string();
	}

	// 필요한 버퍼 크기 계산
	int size = WideCharToMultiByte(
		CP_ACP,                // ASCII 코드 페이지
		0,                     // 기본 플래그
		wstr.c_str(),         // 입력 문자열
		static_cast<int>(wstr.length()), // 입력 문자열 길이
		nullptr,              // 출력 버퍼 (크기 계산용이므로 nullptr)
		0,                    // 출력 버퍼 크기 (크기 계산용이므로 0)
		nullptr,              // 기본 문자 (사용하지 않음)
		nullptr               // 기본 문자 사용 여부 (사용하지 않음)
	);

	// 버퍼 할당 및 변환
	std::string result(size, 0);
	WideCharToMultiByte(
		CP_ACP,
		0,
		wstr.c_str(),
		static_cast<int>(wstr.length()),
		&result[0],          // 출력 버퍼
		size,                // 버퍼 크기
		nullptr,
		nullptr
	);

	return result;
}
