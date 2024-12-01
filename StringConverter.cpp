#include "StringConverter.h"


// ASCII(MultiByte) -> Unicode (std::string -> std::wstring)
inline std::wstring StringConverter::toWstring(const std::string& str) {
	if (str.empty()) {
		return std::wstring();
	}

	// �ʿ��� ���� ũ�� ���
	int size = MultiByteToWideChar(
		CP_ACP,                 // ASCII �ڵ� ������
		0,                      // �⺻ �÷���
		str.c_str(),           // �Է� ���ڿ�
		static_cast<int>(str.length()), // �Է� ���ڿ� ����
		nullptr,               // ��� ���� (ũ�� �����̹Ƿ� nullptr)
		0                      // ��� ���� ũ�� (ũ�� �����̹Ƿ� 0)
	);

	// ���� �Ҵ� �� ��ȯ
	std::wstring result(size, 0);
	MultiByteToWideChar(
		CP_ACP,
		0,
		str.c_str(),
		static_cast<int>(str.length()),
		&result[0],           // ��� ����
		size                  // ���� ũ��
	);

	return result;
}

// Unicode -> ASCII(MultiByte) (std::wstring -> std::string)
inline std::string StringConverter::toString(const std::wstring& wstr) {
	if (wstr.empty()) {
		return std::string();
	}

	// �ʿ��� ���� ũ�� ���
	int size = WideCharToMultiByte(
		CP_ACP,                // ASCII �ڵ� ������
		0,                     // �⺻ �÷���
		wstr.c_str(),         // �Է� ���ڿ�
		static_cast<int>(wstr.length()), // �Է� ���ڿ� ����
		nullptr,              // ��� ���� (ũ�� �����̹Ƿ� nullptr)
		0,                    // ��� ���� ũ�� (ũ�� �����̹Ƿ� 0)
		nullptr,              // �⺻ ���� (������� ����)
		nullptr               // �⺻ ���� ��� ���� (������� ����)
	);

	// ���� �Ҵ� �� ��ȯ
	std::string result(size, 0);
	WideCharToMultiByte(
		CP_ACP,
		0,
		wstr.c_str(),
		static_cast<int>(wstr.length()),
		&result[0],          // ��� ����
		size,                // ���� ũ��
		nullptr,
		nullptr
	);

	return result;
}
