#include "service.h"


bool IsRunningAsService() {
	// 현재 프로세스의 토큰 얻기
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		return false;
	}

	// 토큰 정보 얻기
	DWORD dwLengthNeeded;
	TOKEN_ELEVATION_TYPE elevationType;

	if (!GetTokenInformation(
		hToken,
		TokenElevationType,
		&elevationType,
		sizeof(TOKEN_ELEVATION_TYPE),
		&dwLengthNeeded)) {
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);

	// 현재 프로세스의 세션 ID 얻기
	DWORD sessionId;
	if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
		return false;
	}

	// 서비스는 보통 세션 0에서 실행됨
	// 또한 Local System 계정으로 실행되는지 확인
	TCHAR userName[256];
	DWORD userNameSize = sizeof(userName) / sizeof(TCHAR);

	if (!GetUserName(userName, &userNameSize)) {
		return false;
	}

	// 세션 ID가 0이고 Local System 계정으로 실행되는 경우 서비스로 판단
	return (sessionId == 0) && (_tcscmp(userName, TEXT("SYSTEM")) == 0);
}

bool InstallService(LPCTSTR serviceName, LPCTSTR displayName, LPCTSTR description) {
	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
		printf("Cannot get module file name (%d)\n", GetLastError());
		return false;
	}

	// 서비스 관리자 열기
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // local computer
		NULL,                   // ServicesActive database
		SC_MANAGER_ALL_ACCESS   // full access rights
	);

	if (NULL == schSCManager) {
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

	// 서비스 생성
	SC_HANDLE schService = CreateService(
		schSCManager,              // SCM database
		serviceName,              // name of service
		displayName,      // display name
		SERVICE_ALL_ACCESS,        // desired access
		SERVICE_WIN32_OWN_PROCESS, // service type
		SERVICE_AUTO_START,        // start type
		SERVICE_ERROR_NORMAL,      // error control type
		szPath,                    // path to service's binary
		NULL,                      // no load ordering group
		NULL,                      // no tag identifier
		NULL,                      // no dependencies
		NULL,                      // LocalSystem account
		NULL                       // no password
	);

	if (schService == NULL) {
		printf("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return false;
	}

	// 서비스 설명 설정
	SERVICE_DESCRIPTION sd = { 0, };
	sd.lpDescription = const_cast<LPTSTR>(description);
	ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sd);

	SERVICE_REQUIRED_PRIVILEGES_INFO srpi = { 0, };
	srpi.pmszRequiredPrivileges = (LPSTR)"SeAssignPrimaryTokenPrivilege\0SeIncreaseQuotaPrivilege\0SeImpersonatePrivilege\0";

	ChangeServiceConfig2(
		schService,
		SERVICE_CONFIG_REQUIRED_PRIVILEGES_INFO,
		&srpi
	);


	printf("Service installed successfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}

bool UninstallService(LPCTSTR serviceName) {
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database
		SC_MANAGER_ALL_ACCESS    // full access rights
	);

	if (NULL == schSCManager) {
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

	// 서비스 열기
	SC_HANDLE schService = OpenService(
		schSCManager,
		serviceName,
		DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS
	);

	if (schService == NULL) {
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return false;
	}

	// 서비스가 실행 중이면 중지
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	if (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
		if (ssp.dwCurrentState != SERVICE_STOPPED) {
			// 서비스 중지
			SERVICE_STATUS ss;
			if (ControlService(schService, SERVICE_CONTROL_STOP, &ss)) {
				Sleep(1000);
			}
		}
	}

	// 서비스 삭제
	if (!DeleteService(schService)) {
		printf("DeleteService failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return false;
	}

	printf("Service uninstalled successfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}
