#include "constants.h"
#include "process.h"
#include "service.h"
#include "serviceMain.h"
#include "wsl.h"
#include "hosts.h"
#include "StringConverter.h"

bool install() {
	if (!IsRunningAsAdmin()) {
		int result = MessageBox(NULL,
			TEXT("서비스 설치를 위해 관리자 권한이 필요합니다. 관리자 권한으로 다시 실행하시겠습니까?"),
			TEXT("관리자 권한 필요"),
			MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES) {
			RestartAsAdmin();
			return 0;
		}
		return 1;
	}
	return InstallService(SERVICE_NAME, SERVICE_DISPLAY_NAME, SERVICE_DESCRIPTION_TEXT) ? false : true;
}

bool uninstall() {
	if (!IsRunningAsAdmin()) {
		int result = MessageBox(NULL,
			TEXT("서비스 제거를 위해 관리자 권한이 필요합니다. 관리자 권한으로 다시 실행하시겠습니까?"),
			TEXT("관리자 권한 필요"),
			MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES) {
			RestartAsAdmin();
			return 0;
		}
		return 1;
	}
	return UninstallService(SERVICE_NAME) ? 0 : 1;
}

void printUsage(const TCHAR* programPath) {
	const std::string help =
		"WSL Host Updater\n"
		"----------------\n\n"
		"Description:\n"
		"  This program automatically updates your Windows hosts file with the WSL IP address.\n"
		"  When installed as a service, it runs in the background and keeps the WSL_IP hostname updated.\n\n"
		"Features:\n"
		"  - Automatically detects WSL IP address\n"
		"  - Updates Windows hosts file\n"
		"  - Runs as a Windows service\n"
		"  - Starts automatically with Windows\n\n"
		"Usage:\n";

	std::cout << help;

	// 프로그램 경로와 함께 출력해야 하는 부분
	std::cout << "  " << programPath << " --install    : Install as a Windows service\n";
	std::cout << "  " << programPath << " --uninstall  : Uninstall the service\n";
	std::cout << "  " << programPath << "            : Show this help message\n\n";

	std::cout << "Note: Administrative privileges are required for installation/uninstallation.\n";
}

int main(int argc, TCHAR* argv[]) {
	if (argc > 1) {
		if (_stricmp(argv[1], "--install") == 0) {
			install();
		}
		else if (_stricmp(argv[1], "--uninstall") == 0) {
			uninstall();
		}
		else {
			std::cout << "Unknown command: " << argv[1] << std::endl;
			printUsage(argv[0]);
		}
	}

	// 서비스로 실행 중인 경우
	if (IsRunningAsService()) {
		SERVICE_TABLE_ENTRY ServiceTable[] = {
			{const_cast<LPTSTR>(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{NULL, NULL}
		};

		if (!StartServiceCtrlDispatcher(ServiceTable)) {
			return GetLastError();
		}


	}
	else {

	}

	return 0;
}