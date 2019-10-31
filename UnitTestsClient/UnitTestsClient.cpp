#include "pch.h"
#include "CppUnitTest.h"
#include "MockCollector.h"
#include "MessageQueue.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsClient
{
	TEST_CLASS(UnitTestsCollector)
	{
	public:
		
		TEST_METHOD(TestCollect)
		{
			MockCollector collector;
			collector.collect();
			Assert::IsTrue(MSGQUEUE->hasServerMessage(), L"������� ��������� ���� ������ ���������� ��������� ������� � ������� ���������");
			Messages::Message msg = MSGQUEUE->getServerMessage();
			Assert::AreEqual(msg.text, collector.jsonStr(), L"��������� ��������� ������� �� ������������� ����������");
		}
	};
}
