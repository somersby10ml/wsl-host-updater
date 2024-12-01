#include "hosts.h"

#ifdef UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

bool UpdateHostsFile(LPCTSTR hostFilePath, LPCTSTR ip, LPCTSTR hostName, LPCTSTR comment) {
	try {
		// ���� hosts ���� �б�
		std::ifstream inFile(hostFilePath);
		if (!inFile) {
			return false;
		}

		std::vector<tstring> lines;
		tstring line;
		bool found = false;

		// �� �� ó��
		while (std::getline(inFile, line)) {
			// ���� ���� ����
			auto trimLine = [](std::string& str) {
				str.erase(0, str.find_first_not_of(" \t"));
				str.erase(str.find_last_not_of(" \t") + 1);
				};

			std::string originalLine = line;
			trimLine(line);

			// �ּ� �����̰ų� �� ���̸� �״�� ����
			if (line.empty() || line[0] == '#') {
				lines.push_back(originalLine);
				continue;
			}

			// �츮 ������ �������� Ȯ��
			if (line.find(comment) != std::string::npos) {
				found = true;
				// ���ο� IP�� ������Ʈ
				std::stringstream ss;
				ss << ip << " " << hostName << " # " << comment;
				lines.push_back(ss.str());
			}
			else {
				lines.push_back(originalLine);
			}
		}
		inFile.close();

		// ���� ������ �����ٸ� �߰�
		if (!found) {
			std::stringstream ss;
			ss << ip << " " << hostName << " # " << comment;
			lines.push_back(ss.str());
		}

		// ���� �ٽ� ����
		std::ofstream outFile(hostFilePath);
		if (!outFile) {
			return false;
		}

		for (size_t i = 0; i < lines.size(); ++i) {
			outFile << lines[i];
			// ������ ���� �ƴϸ� ���� �߰�
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
