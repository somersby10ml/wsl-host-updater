#include "process.h"

bool IsRunningAsAdmin() {
	BOOL isAdmin = false;
	HANDLE hToken = NULL;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		return isAdmin;
	}

	TOKEN_ELEVATION elevation;
	DWORD cbSize = sizeof(TOKEN_ELEVATION);

	if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &cbSize)) {
		isAdmin = elevation.TokenIsElevated;
	}
	CloseHandle(hToken);
	return isAdmin;
}

bool RestartAsAdmin() {
	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
		return false;
	}

	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpVerb = TEXT("runas");
	sei.lpFile = szPath;
	sei.hwnd = NULL;
	sei.nShow = SW_NORMAL;

	if (!ShellExecuteEx(&sei)) {
		return false;
	}

	return true;
}