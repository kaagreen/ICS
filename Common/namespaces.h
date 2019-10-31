#pragma once
#include <string>

namespace AppErrors {
	const std::string INVALID_PARAMETERS_NUMBER	= "Недопустимое количество параметров";
	const std::string THREAD_RUNNING_ERROR		= "Не удалось открыть поток";
	const std::string WSA_INIT_ERROR			= "Ошибка инициализации библиотеки WinSock";
	const std::string SOCKET_INIT_ERROR			= "Ошибка инициализации сокета";
	const std::string SOCKET_BIND_ERROR			= "Ошибка привязки адреса к сокету";
	const std::string CONNECTION_CLOSED			= "Соединение закрыто";
	const std::string OPEN_FILE_ERROR			= "Ошибка открытия файла";
	const std::string WRITE_FILE_ERROR			= "Ошибка записи в файл";
	const std::string ERROR_EXIT			    = "Завершение работы приложения из-за возникшей ошибки";
};

namespace AppMessages {
	const std::string PARAMETERS				= "Параметры запуска приложения: ";
	const std::string START						= "Старт...";
	const std::string SERVER_RUNNING			= "Сервер успешно запущен";
	const std::string CONNECTED					= "Подключен";
	const std::string OPENED_PORT				= "Открыт порт";
	const std::string REQUEST_RECEIVED			= "Получено сообщение";
	const std::string EXIT						= "Завершение работы...";
};