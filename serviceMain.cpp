#include "serviceMain.h"
#include "constants.h"
#include <string>
#include "wsl.h"
#include "StringConverter.h"
#include "hosts.h"

// ���� ����
SERVICE_STATUS          gServiceStatus;
SERVICE_STATUS_HANDLE   gStatusHandle;
HANDLE                  gServiceStopEvent = NULL;

// ���� ���� �Լ� ����
VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) {
	gStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if (gStatusHandle == NULL) {
		return;
	}

	gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	gServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	gServiceStatus.dwWin32ExitCode = 0;
	gServiceStatus.dwServiceSpecificExitCode = 0;
	gServiceStatus.dwCheckPoint = 0;
	gServiceStatus.dwWaitHint = 0;

	if (!SetServiceStatus(gStatusHandle, &gServiceStatus)) {
		return;
	}

	gServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (gServiceStopEvent == NULL) {
		gServiceStatus.dwCurrentState = SERVICE_STOPPED;
		gServiceStatus.dwWin32ExitCode = GetLastError();
		SetServiceStatus(gStatusHandle, &gServiceStatus);
		return;
	}

	// ���񽺰� ���� ������ �˸�
	gServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(gStatusHandle, &gServiceStatus);


	std::string wslIp = GetWSLIPAddress();
	auto convertWslIp = StringConverter::asciiToTchar(wslIp);
	UpdateHostsFile(HOSTS_PATH, convertWslIp.c_str(), HOST_NAME, SERVICE_COMMENT);

	CloseHandle(gServiceStopEvent);

	gServiceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(gStatusHandle, &gServiceStatus);
}

// ���� ��Ʈ�� �ڵ鷯 ����
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
	switch (dwCtrl) {
		case SERVICE_CONTROL_STOP:
			if (gServiceStatus.dwCurrentState != SERVICE_RUNNING)
				break;

			gServiceStatus.dwControlsAccepted = 0;
			gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			gServiceStatus.dwWin32ExitCode = 0;
			gServiceStatus.dwCheckPoint = 4;

			SetServiceStatus(gStatusHandle, &gServiceStatus);

			// ���� ���� �̺�Ʈ ����
			SetEvent(gServiceStopEvent);
			break;

		default:
			break;
	}
}
