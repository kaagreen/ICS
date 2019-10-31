#pragma once
#include <string>

namespace AppErrors {
	const std::string INVALID_PARAMETERS_NUMBER	= "������������ ���������� ����������";
	const std::string THREAD_RUNNING_ERROR		= "�� ������� ������� �����";
	const std::string WSA_INIT_ERROR			= "������ ������������� ���������� WinSock";
	const std::string SOCKET_INIT_ERROR			= "������ ������������� ������";
	const std::string SOCKET_BIND_ERROR			= "������ �������� ������ � ������";
	const std::string CONNECTION_CLOSED			= "���������� �������";
	const std::string OPEN_FILE_ERROR			= "������ �������� �����";
	const std::string WRITE_FILE_ERROR			= "������ ������ � ����";
	const std::string ERROR_EXIT			    = "���������� ������ ���������� ��-�� ��������� ������";
};

namespace AppMessages {
	const std::string PARAMETERS				= "��������� ������� ����������: ";
	const std::string START						= "�����...";
	const std::string SERVER_RUNNING			= "������ ������� �������";
	const std::string CONNECTED					= "���������";
	const std::string OPENED_PORT				= "������ ����";
	const std::string REQUEST_RECEIVED			= "�������� ���������";
	const std::string EXIT						= "���������� ������...";
};