#pragma once
#include <string>
#include <memory>
#include <array>

std::string GetWSLIPAddress();
bool GetWSLIPAddress(std::string& outIP, std::string& outError);