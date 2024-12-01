#include "service.h"


bool IsRunningAsService() {
	// ���� ���μ����� ��ū ���
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		return false;
	}

	// ��ū ���� ���
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

	// ���� ���μ����� ���� ID ���
	DWORD sessionId;
	if (!ProcessIdToSessionId(GetCurrentProcessId(), &sessionId)) {
		return false;
	}

	// ���񽺴� ���� ���� 0���� �����
	// ���� Local System �������� ����Ǵ��� Ȯ��
	TCHAR userName[256];
	DWORD userNameSize = sizeof(userName) / sizeof(TCHAR);

	if (!GetUserName(userName, &userNameSize)) {
		return false;
	}

	// ���� ID�� 0�̰� Local System �������� ����Ǵ� ��� ���񽺷� �Ǵ�
	return (sessionId == 0) && (_tcscmp(userName, TEXT("SYSTEM")) == 0);
}

bool InstallService(LPCTSTR serviceName, LPCTSTR displayName, LPCTSTR description) {
	TCHAR szPath[MAX_PATH];
	if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
		printf("Cannot get module file name (%d)\n", GetLastError());
		return false;
	}

	// ���� ������ ����
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // local computer
		NULL,                   // ServicesActive database
		SC_MANAGER_ALL_ACCESS   // full access rights
	);

	if (NULL == schSCManager) {
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return false;
	}

	// ���� ����
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

	// ���� ���� ����
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

	// ���� ����
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

	// ���񽺰� ���� ���̸� ����
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	if (QueryServiceStatusEx(schService, SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
		if (ssp.dwCurrentState != SERVICE_STOPPED) {
			// ���� ����
			SERVICE_STATUS ss;
			if (ControlService(schService, SERVICE_CONTROL_STOP, &ss)) {
				Sleep(1000);
			}
		}
	}

	// ���� ����
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
