#include "pch.h"
#include "CppUnitTest.h"
#include "Thread.h"
#include "MessageQueue.h"
#include "RequestWriter.h"
#include "MockInputListener.h"
#include "MockSocketListener.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTestSocketListener)
	{

	public:
		TEST_METHOD(TestConstruct)
		{
			SocketListener server;
			Assert::IsNotNull(server.initEvent(), L"Конструктор SocketListener не создал _initEvent");
			Assert::IsNotNull(server.createSocketEvent(), L"Конструктор SocketListener не создал _createSocketEvent");
			Assert::IsNotNull(server.socketBindEvent(), L"Конструктор SocketListener не создал _socketBindEvent");
			Assert::IsTrue(server.isSocketValid(), L"Конструктор SocketListener не создал SOCKET");
			DWORD initresult = WaitForSingleObject(server.initEvent(), 10000);
			Assert::AreEqual(WAIT_OBJECT_0, initresult), L"Конструктор SocketListener не установил _initEvent";
			initresult = WaitForSingleObject(server.createSocketEvent(), 10000);
			Assert::AreEqual(WAIT_OBJECT_0, initresult), L"Конструктор SocketListener не установил _createSocketEvent";
		}

		TEST_METHOD(TestStart)
		{
			SocketListener server;
			SocketListener::Params rightParams{ 14080 };
			SocketListener::Params wrongParams{ -1 };
			
			Messages::Message msg{ Messages::EXIT, 0, "" };
			MSGQUEUE->postMessage(msg);
			
			server.start((LPVOID)&rightParams);
			Assert::IsTrue(server.isSocketBinded(), L"Метод SocketListener::start вызван с правильными параметрами, но не смог привязать сокет к адресу");

			DWORD initresult = WaitForSingleObject(server.socketBindEvent(), 10000);
			Assert::AreEqual(WAIT_OBJECT_0, initresult), L"Метод SocketListener::start вызван с правильными параметрами, но не установил _socketBindEvent";

			server.start((LPVOID)&wrongParams);
			Assert::IsFalse(server.isSocketBinded(), L"Метод SocketListener::start вызван с правильными параметрами и не должен был установить флаг привязки сокета к адресу");

			initresult = WaitForSingleObject(server.socketBindEvent(), 10000);
			Assert::AreNotEqual(WAIT_OBJECT_0, initresult), L"Метод SocketListener::start вызван с правильными параметрамии не должен был установить _socketBindEvent";

			Assert::IsTrue(MSGQUEUE->isExitFlag(), L"После вызова метода SocketListener::start с неправильными параметрами, должен быть установлен MessageQueue::exitFlag");
		}

		TEST_METHOD(TestCloseConnections)
		{
			MockSocketListener listener;
			ClientsList clients;
			SClientPtr client = std::make_shared<Client>(socket(AF_INET, SOCK_STREAM, 0), "TestClient");
			clients.push_back(client);
			listener.setClients(clients);
			const std::string clientmsgtext = "Test client cleanup";
			Messages::ClientMessage clientmsg{ Messages::CLOSE_CLIENT, 0, clientmsgtext, client.get() };
			Messages::Message exitmsg{ Messages::EXIT, 0 ,"" };
			MSGQUEUE->postClientMessage(clientmsg);
			MSGQUEUE->postMessage(exitmsg);
			listener.closeConnections(NULL);
			Assert::IsTrue(listener.getClients().empty(), L"Метод должен очищать лист клиентов при получении сообщения Messages::CLOSE_CLIENT");
			
			bool clientflag = false;
			while (MSGQUEUE->hasMessage())
			{
				Messages::Message msg = MSGQUEUE->getMessage();
				if (msg.text == clientmsgtext)
					clientflag = true;
			}
			Assert::IsTrue(clientflag, L"SocketListener должен размещать сообщение о закрытии клиента в очередь сообщений");
		}
	};

	TEST_CLASS(UnitTestMessageQueue)
	{
		TEST_METHOD(TestPostMessage)
		{
			MessageQueue msgqueue;
			Messages::Message msg{ Messages::NOTYFICATION, 0, "" };
			msgqueue.postMessage(msg);
			Assert::IsTrue(msgqueue.hasMessage(), L"Очередь сообщений не должна быть пустой");
		}
		TEST_METHOD(TestGetMessage)
		{
			MessageQueue msgqueue;
			Messages::Message msg{ Messages::NOTYFICATION, 0, "" };
			msgqueue.postMessage(msg);
			msgqueue.getMessage();
			Assert::IsFalse(msgqueue.hasMessage(), L"Очередь сообщений не должна содержать сообщений");
		}

		TEST_METHOD(TestPostClentMessage)
		{
			MessageQueue msgqueue;
			Messages::ClientMessage msg{ Messages::NOTYFICATION, 0, "", NULL };
			msgqueue.postClientMessage(msg);
			Assert::IsTrue(msgqueue.hasClientMessage(), L"Очередь сообщений не должна быть пустой");
		}
		TEST_METHOD(TestGetClientMessage)
		{
			MessageQueue msgqueue;
			Messages::ClientMessage msg{ Messages::NOTYFICATION, 0, "", NULL };
			msgqueue.postClientMessage(msg);
			msgqueue.getClientMessage();
			Assert::IsFalse(msgqueue.hasMessage(), L"Очередь сообщений не должна содержать сообщений");
		}

		TEST_METHOD(TestPostWFMessage)
		{
			MessageQueue msgqueue;
			Messages::WFMessage msg{ "", "" };
			msgqueue.postFWMessage(msg);
			Assert::IsTrue(msgqueue.hasWFMessage(), L"Очередь сообщений не должна быть пустой");
		}
		TEST_METHOD(TestGetWFMessage)
		{
			MessageQueue msgqueue;
			Messages::WFMessage msg{"", "" };
			msgqueue.postFWMessage(msg);
			msgqueue.getWFMessage();
			Assert::IsFalse(msgqueue.hasMessage(), L"Очередь сообщений не должна содержать сообщений");
		}

		TEST_METHOD(TestExitFlag)
		{
			MessageQueue msgqueue;
			Messages::Message msg{ Messages::EXIT, 0, "" };
			msgqueue.postMessage(msg);
			Assert::IsTrue(msgqueue.isExitFlag(), L"Не установлен ExitFlag");
		}
	};

	TEST_CLASS(UnitTestRequestWriter) {
		TEST_METHOD(TestWrite) {
			setlocale(LC_CTYPE, "");
			RequestWriter writer;
			std::string path;
			#ifdef OBJ_DIRECTORY
				std::wstring wpath;
				wpath = std::wstring(OBJ_DIRECTORY);
				path = std::string(wpath.begin(), wpath.end());
			#endif

			std::string name = "test";
			std::string text = "test";
			const int length = text.length();
			std::string rawpath = path + name + ".json";

			Messages::Message msg{ Messages::EXIT, 0, "" };
			MSGQUEUE->postMessage(msg);

			RequestWriter::Params params{path};
			writer.start((LPVOID)&params);

			writer.write(name, text);

			HANDLE hFile;
			LPSTR filename = const_cast<char*>(rawpath.c_str());
			hFile = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			Assert::AreNotEqual(INVALID_HANDLE_VALUE, hFile, L"Не создан проверочный файл");

			char* buff = new char[length + 1];
			ZeroMemory(buff, length + 1);
			DWORD readed = 0;

			BOOL bErrorFlag = ReadFile(hFile, buff, length, &readed, NULL);
			std::string readedText(buff);

			Assert::AreNotEqual(readed, (DWORD)0, L"Неверно записан файл");
			Assert::AreEqual(readedText, text, L"Неверно записан файл");
			CloseHandle(hFile);
			delete[] buff;
		}
	};

	TEST_CLASS(UnitInputListener)
	{
		TEST_METHOD(TestExitFlag) {
			MockInputListener listener;
			listener.setInput("eXit");
			listener.start(NULL);
			Assert::IsTrue(MSGQUEUE->isExitFlag(), L"После ввода сообщения пользователя Exit, должен быть установлен MessageQueue::exitFlag");
		}
	};
}
