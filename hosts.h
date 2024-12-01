#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <Windows.h>

bool UpdateHostsFile(LPCTSTR hostFilePath, LPCTSTR ip, LPCTSTR hostName, LPCTSTR comment);

