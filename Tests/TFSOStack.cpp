#include "pch.h"
#include "CppUnitTest.h"

#include "../TextEditing/FSOStack.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	TEST_CLASS(TFSOStack) {
	public:
		TEST_METHOD(FSOStack_Ctor)
		{
			ted::FSOStack<int, 3> s;

			Assert::IsTrue(s.Empty(), L"0");
			Assert::IsTrue(s.Capacity() == 3, L"1");
			Assert::IsTrue(s.Size() == 0, L"2");
		}

		TEST_METHOD(FSOStack_Full)
		{
			ted::FSOStack<int, 3> s;

			for (size_t i = 0; i < s.Capacity(); ++i) s.Push(i);

			Assert::IsFalse(s.Empty(), L"0");
			Assert::IsTrue(s.Full(), L"1");
			Assert::IsTrue(s.Size() == 3, L"2");

			for (size_t i = 0; i < s.Capacity(); ++i) {
				auto x = s.Top();
				s.Pop();

				Assert::IsTrue(x == s.Capacity() - i - 1, L"3");
			}

			Assert::IsTrue(s.Empty(), L"4");
		}

		TEST_METHOD(FSOStack_Overwrite)
		{
			ted::FSOStack<int, 3>	s;
			int						x;

			s.Push(0);	s.Push(1);	s.Push(2);
			s.Push(3);// overwrites 0
			
			Assert::IsTrue(s.Size() == 3, L"0");

			x = s.Top(); s.Pop();
			Assert::IsTrue(x == 3, L"1a");
			Assert::IsTrue(s.Size() == 2, L"1a");
			
			x = s.Top(); s.Pop();
			Assert::IsTrue(x == 2, L"2a");
			Assert::IsTrue(s.Size() == 1, L"2a");

			x = s.Top(); s.Pop();
			Assert::IsTrue(x == 1, L"3a");
			Assert::IsTrue(s.Size() == 0, L"3a");

			Assert::IsTrue(s.Empty(), L"3");
		}
	};
}