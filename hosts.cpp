#include "hosts.h"

#ifdef UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

bool UpdateHostsFile(LPCTSTR hostFilePath, LPCTSTR ip, LPCTSTR hostName, LPCTSTR comment) {
	try {
		// 현재 hosts 파일 읽기
		std::ifstream inFile(hostFilePath);
		if (!inFile) {
			return false;
		}

		std::vector<tstring> lines;
		tstring line;
		bool found = false;

		// 각 줄 처리
		while (std::getline(inFile, line)) {
			// 양쪽 공백 제거
			auto trimLine = [](std::string& str) {
				str.erase(0, str.find_first_not_of(" \t"));
				str.erase(str.find_last_not_of(" \t") + 1);
				};

			std::string originalLine = line;
			trimLine(line);

			// 주석 라인이거나 빈 줄이면 그대로 유지
			if (line.empty() || line[0] == '#') {
				lines.push_back(originalLine);
				continue;
			}

			// 우리 서비스의 라인인지 확인
			if (line.find(comment) != std::string::npos) {
				found = true;
				// 새로운 IP로 업데이트
				std::stringstream ss;
				ss << ip << " " << hostName << " # " << comment;
				lines.push_back(ss.str());
			}
			else {
				lines.push_back(originalLine);
			}
		}
		inFile.close();

		// 서비스 라인이 없었다면 추가
		if (!found) {
			std::stringstream ss;
			ss << ip << " " << hostName << " # " << comment;
			lines.push_back(ss.str());
		}

		// 파일 다시 쓰기
		std::ofstream outFile(hostFilePath);
		if (!outFile) {
			return false;
		}

		for (size_t i = 0; i < lines.size(); ++i) {
			outFile << lines[i];
			// 마지막 줄이 아니면 개행 추가
			if (i < lines.size() - 1) {
				outFile << "\n";
			}
		}

		outFile.close();
		return true;
	}
	catch (const std::exception&) {
		return false;
	}
}
