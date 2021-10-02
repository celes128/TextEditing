#include "pch.h"
#include "CppUnitTest.h"

#include "../TextEditing/TextLine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	TEST_CLASS(Tests) {
	public:	
		TEST_METHOD(TextLine_Ctor)
		{
			ted::TextLine line;

			Assert::IsTrue(line.String().length() == 0, L"1a");
			Assert::IsTrue(line.Caret() == 0, L"1b");

			line = ted::TextLine("abc");
			Assert::IsTrue(line.String().length() == 3, L"2a");
			Assert::IsTrue(line.Caret() == 3, L"2b");
		}

		TEST_METHOD(TextLine_CDeleteLeftChar)
		{
			ted::TextLine line("abc");

			line.CDeleteLeftChar();
			Assert::IsTrue(line.String() == "ab", L"1a");
			Assert::IsTrue(line.Caret() == 2, L"1b");

			line.CDeleteLeftChar();
			line.CDeleteLeftChar();
			Assert::IsTrue(line.String() == "", L"2a");
			Assert::IsTrue(line.Caret() == 0, L"2b");

			bool ans = line.CDeleteLeftChar();
			Assert::IsFalse(ans, L"3a");
		}

		TEST_METHOD(TextLine_InsertChar)
		{
			ted::TextLine line;

			line.InsertChar('a');
			Assert::IsTrue(line.String() == "a", L"0");
			line.InsertChar('b');
			Assert::IsTrue(line.String() == "ab", L"1");
			line.InsertChar('c');
			Assert::IsTrue(line.String() == "abc", L"2");
		}
	};
}
