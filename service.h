#pragma once
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <stdio.h>

bool InstallService(LPCTSTR serviceName, LPCTSTR displayName, LPCTSTR description);
bool UninstallService(LPCTSTR serviceName);
bool IsRunningAsService();