#pragma once
#include <windows.h>

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwCtrl);
