#pragma once
#include <windows.h>
#include <tchar.h>

static constexpr const TCHAR* SERVICE_NAME = TEXT("wsl-host-updater");
static constexpr const TCHAR* SERVICE_DISPLAY_NAME = TEXT("WSL Host Updater Service");
static constexpr const TCHAR* SERVICE_DESCRIPTION_TEXT = TEXT("Automatically update hosts file with WSL IP");

static constexpr const TCHAR* HOSTS_PATH = TEXT("C:\\Windows\\System32\\drivers\\etc\\hosts");
static constexpr const TCHAR* SERVICE_COMMENT = TEXT("wsl-host-update-service");
static constexpr const TCHAR* HOST_NAME = TEXT("wsl_ip");

