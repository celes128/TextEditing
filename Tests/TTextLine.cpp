#include "pch.h"
#include "CppUnitTest.h"

#include "../TextEditing/TextLine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	TEST_CLASS(TTextLine) {
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

		TEST_METHOD(TextLine_InsertAtCaret)
		{
			ted::TextLine line;

			line.InsertAtCaret('a');
			Assert::IsTrue(line.String() == "a", L"0");
			line.InsertAtCaret('b');
			Assert::IsTrue(line.String() == "ab", L"1");
			line.InsertAtCaret('c');
			Assert::IsTrue(line.String() == "abc", L"2");
		}

		TEST_METHOD(TextLineInsertAtIndex)
		{
			ted::TextLine line;

			line.InsertAtIndex('b', 0);
			Assert::IsTrue(line.String() == "b", L"0");

			line.InsertAtIndex('a', 0);
			Assert::IsTrue(line.String() == "ab", L"1");

			line.InsertAtIndex('d', 2);
			Assert::IsTrue(line.String() == "abd", L"2");

			line.InsertAtIndex('c', 2);
			Assert::IsTrue(line.String() == "abcd", L"3");

			line.InsertAtCaret('e');
			Assert::IsTrue(line.String() == "abcde", L"4");
		}

		TEST_METHOD(TextLineDeleteCharAtIndex)
		{
			ted::TextLine	line;
			char			c;
			bool			deleted;

			deleted = line.DeleteCharAtIndex(0);
			Assert::IsFalse(deleted, L"0");

			line.InsertAtCaret('a');
			deleted = line.DeleteCharAtIndex(0, &c);
			Assert::IsTrue(deleted, L"1a");
			Assert::IsTrue(c == 'a', L"1b");
			Assert::IsTrue(line.String() == "", L"1c");
			Assert::IsTrue(line.Caret() == 0, L"1d");
		}
	};
}
