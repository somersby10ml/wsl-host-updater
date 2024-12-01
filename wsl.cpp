#include "wsl.h"
#include <Windows.h>
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

DWORD GetActiveSessionId() {
	PWTS_SESSION_INFO pSessionInfo;
	DWORD count;
	DWORD sessionId = -1;

	if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &count)) {
		for (DWORD i = 0; i < count; i++) {
			if (pSessionInfo[i].State == WTSActive) {
				sessionId = pSessionInfo[i].SessionId;
				break;
			}
		}
		WTSFreeMemory(pSessionInfo);
	}
	return sessionId;
}

HANDLE GetUserToken() {
	DWORD sessionId = GetActiveSessionId();
	if (sessionId == -1) return NULL;

	HANDLE hUserToken = NULL;
	if (!WTSQueryUserToken(sessionId, &hUserToken)) {
		return NULL;
	}

	HANDLE hPrimaryToken = NULL;
	if (!DuplicateTokenEx(hUserToken,
		TOKEN_ALL_ACCESS,
		NULL,
		SecurityImpersonation,
		TokenPrimary,
		&hPrimaryToken)) {
		CloseHandle(hUserToken);
		return NULL;
	}

	CloseHandle(hUserToken);
	return hPrimaryToken;
}


std::string GetWSLIPAddress() {
	std::string ip;

	// 사용자 토큰 얻기
	HANDLE hUserToken = GetUserToken();
	if (!hUserToken) {
		return "Error: Failed to get user token";
	}

	// 파이프 생성
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hReadPipe = NULL, hWritePipe = NULL;

	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
		CloseHandle(hUserToken);
		return "Error: Failed to create pipe";
	}

	// 시작 정보 구조체 설정
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hWritePipe;
	si.hStdError = hWritePipe;
	si.hStdInput = NULL;

	// 프로세스 정보 구조체
	PROCESS_INFORMATION pi = { 0 };

	// wsl 명령어 준비
	char cmdline[] = "wsl hostname -I";

	// 프로세스 생성
	BOOL success = CreateProcessAsUser(
		hUserToken,                // 사용자 토큰
		NULL,                      // No module name (use command line)
		cmdline,                   // Command line
		NULL,                      // Process handle not inheritable
		NULL,                      // Thread handle not inheritable
		TRUE,                      // Handle inheritance required for pipes
		CREATE_NO_WINDOW,         // No window
		NULL,                      // Use parent's environment block
		NULL,                      // Use parent's starting directory 
		&si,                      // Pointer to STARTUPINFO structure
		&pi                       // Pointer to PROCESS_INFORMATION structure
	);

	if (!success) {
		CloseHandle(hUserToken);
		CloseHandle(hReadPipe);
		CloseHandle(hWritePipe);
		return "Error: Failed to create process";
	}

	// 쓰기 파이프는 더 이상 필요없음
	CloseHandle(hWritePipe);

	// 프로세스 완료 대기
	WaitForSingleObject(pi.hProcess, 5000);  // 5초 타임아웃

	// 결과 읽기
	char buffer[128];
	DWORD bytesRead;

	while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = '\0';
		ip += buffer;
	}

	// 리소스 정리
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hUserToken);
	CloseHandle(hReadPipe);

	// 결과 문자열 정리
	while (!ip.empty() && (ip.back() == '\n' || ip.back() == '\r' || ip.back() == ' ')) {
		ip.pop_back();
	}

	if (ip.empty()) {
		return "Error: Could not retrieve WSL IP";
	}

	// 여러 IP가 있는 경우 첫 번째 IP만 반환
	size_t spacePos = ip.find(' ');
	if (spacePos != std::string::npos) {
		ip = ip.substr(0, spacePos);
	}

	return ip;
}

// 에러 체크를 포함한 버전
bool GetWSLIPAddress(std::string& outIP, std::string& outError) {
	try {
		std::array<char, 128> buffer;
		std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen("wsl hostname -I", "r"), _pclose);

		if (!pipe) {
			outError = "Failed to execute WSL command";
			return false;
		}

		std::string result;
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}

		// 결과 정리
		while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
			result.pop_back();
		}

		if (result.empty()) {
			outError = "WSL IP address not found";
			return false;
		}

		// 여러 IP 중 첫 번째 IP만 추출
		size_t spacePos = result.find(' ');
		if (spacePos != std::string::npos) {
			outIP = result.substr(0, spacePos);
		}
		else {
			outIP = result;
		}

		return true;
	}
	catch (const std::exception& e) {
		outError = std::string("Exception: ") + e.what();
		return false;
	}
}