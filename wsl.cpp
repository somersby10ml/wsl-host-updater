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

	// ����� ��ū ���
	HANDLE hUserToken = GetUserToken();
	if (!hUserToken) {
		return "Error: Failed to get user token";
	}

	// ������ ����
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hReadPipe = NULL, hWritePipe = NULL;

	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
		CloseHandle(hUserToken);
		return "Error: Failed to create pipe";
	}

	// ���� ���� ����ü ����
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hWritePipe;
	si.hStdError = hWritePipe;
	si.hStdInput = NULL;

	// ���μ��� ���� ����ü
	PROCESS_INFORMATION pi = { 0 };

	// wsl ��ɾ� �غ�
	char cmdline[] = "wsl hostname -I";

	// ���μ��� ����
	BOOL success = CreateProcessAsUser(
		hUserToken,                // ����� ��ū
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

	// ���� �������� �� �̻� �ʿ����
	CloseHandle(hWritePipe);

	// ���μ��� �Ϸ� ���
	WaitForSingleObject(pi.hProcess, 5000);  // 5�� Ÿ�Ӿƿ�

	// ��� �б�
	char buffer[128];
	DWORD bytesRead;

	while (ReadFile(hReadPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = '\0';
		ip += buffer;
	}

	// ���ҽ� ����
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hUserToken);
	CloseHandle(hReadPipe);

	// ��� ���ڿ� ����
	while (!ip.empty() && (ip.back() == '\n' || ip.back() == '\r' || ip.back() == ' ')) {
		ip.pop_back();
	}

	if (ip.empty()) {
		return "Error: Could not retrieve WSL IP";
	}

	// ���� IP�� �ִ� ��� ù ��° IP�� ��ȯ
	size_t spacePos = ip.find(' ');
	if (spacePos != std::string::npos) {
		ip = ip.substr(0, spacePos);
	}

	return ip;
}

// ���� üũ�� ������ ����
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

		// ��� ����
		while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
			result.pop_back();
		}

		if (result.empty()) {
			outError = "WSL IP address not found";
			return false;
		}

		// ���� IP �� ù ��° IP�� ����
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