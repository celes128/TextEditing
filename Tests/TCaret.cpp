#include "pch.h"
#include "CppUnitTest.h"

#include "../TextEditing/TextLine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {

	TEST_CLASS(TCaret) {
	public:
		TEST_METHOD(CaretAtLineBegin)
		{
			// ---------------------------------------
			ted::TextLine line("abcde");			

			line.CMoveToLineBegin();
			Assert::IsTrue(line.CAtLineBegin(), L"1");
		}

		TEST_METHOD(CaretAtLineEnd)
		{
			// ---------------------------------------
			ted::TextLine line("abcde");

			Assert::IsTrue(line.CAtLineEnd(), L"1");

			line.CMoveToLineBegin();
			line.CMoveToLineEnd();
			Assert::IsTrue(line.CAtLineEnd(), L"2");
		}

		TEST_METHOD(CaretMoveForward)
		{
			// ---------------------------------------
			ted::TextLine line("abcde");

			auto prev = line.Caret();
			line.CMoveForward();
			auto now = line.Caret();
			Assert::IsTrue(prev == now, L"0");

			line.CMoveToLineBegin();
			line.CMoveForward();
			Assert::IsTrue(line.Caret() == 1, L"1");
			line.CMoveForward();
			Assert::IsTrue(line.Caret() == 2, L"2");
			line.CMoveForward(3);
			Assert::IsTrue(line.Caret() == 5, L"3");
			line.CMoveForward();
			Assert::IsTrue(line.Caret() == 5, L"4");
		}

		TEST_METHOD(CaretMoveBackward)
		{
			// ---------------------------------------
			ted::TextLine line("abcde");

			line.CMoveBackward();
			Assert::IsTrue(line.Caret() == 4, L"0");
			line.CMoveBackward(4);
			Assert::IsTrue(line.Caret() == 0, L"1");
			line.CMoveBackward();
			Assert::IsTrue(line.Caret() == 0, L"2");
		}

		TEST_METHOD(CaretAtWordBegin)
		{
			// ---------------------------------------
			ted::TextLine line("abcde");

			Assert::IsFalse(line.CAtWordBegin(), L"0");
			line.CMoveToLineBegin();
			Assert::IsTrue(line.CAtWordBegin(), L"1");
			line.CMoveForward();
			Assert::IsFalse(line.CAtWordBegin(), L"2");

			// ------------------------------------------
			line = ted::TextLine(" abcde");

			line.CMoveToLineBegin();
			Assert::IsFalse(line.CAtWordBegin(), L"3");
			line.CMoveForward();
			Assert::IsTrue(line.CAtWordBegin(), L"4");
		}

		TEST_METHOD(CaretAtWordEnd)
		{
			// ---------------------------------------
			ted::TextLine line("abcde");

			Assert::IsTrue(line.CAtWordEnd(), L"0");
			line.CMoveBackward();
			Assert::IsFalse(line.CAtWordEnd(), L"1");
			line.CMoveToLineBegin();
			Assert::IsFalse(line.CAtWordEnd(), L"2");

			// ---------------------------------------
			line = "";
			Assert::IsFalse(line.CAtWordEnd(), L"3");

			// ---------------------------------------
			line = " a ";
			Assert::IsFalse(line.CAtWordEnd(), L"4");
			line.CMoveToLineBegin();
			Assert::IsFalse(line.CAtWordEnd(), L"5");
			line.CMoveForward();
			Assert::IsFalse(line.CAtWordEnd(), L"6");
			line.CMoveForward();
			Assert::IsTrue(line.CAtWordEnd(), L"7");
		}

		TEST_METHOD(CaretMoveToPrevWordBegin)
		{
			// ----------------------------------------
			//                01234567
			ted::TextLine line("ab cd e");
			bool		moved;

			moved = line.CMoveToPrevWordBegin();
			Assert::IsTrue(moved, L"0a");
			Assert::IsTrue(line.Caret() == 6, L"0b");

			moved = line.CMoveToPrevWordBegin();
			Assert::IsTrue(moved, L"1a");
			Assert::IsTrue(line.Caret() == 3, L"1b");

			moved = line.CMoveToPrevWordBegin();
			Assert::IsTrue(moved, L"2a");
			Assert::IsTrue(line.Caret() == 0, L"2b");

			moved = line.CMoveToPrevWordBegin();
			Assert::IsFalse(moved, L"3a");
			Assert::IsTrue(line.Caret() == 0, L"3b");
		}

		TEST_METHOD(CaretMoveToNextWordBegin)
		{
			// ----------------------------------------
			//                01234567
			ted::TextLine	line("ab cd e");
			bool			moved;

			line.CMoveToLineBegin();
			moved = line.CMoveToNextWordBegin();
			Assert::IsTrue(moved, L"0a");
			Assert::IsTrue(line.Caret() == 3, L"0b");

			moved = line.CMoveToNextWordBegin();
			Assert::IsTrue(moved, L"1a");
			Assert::IsTrue(line.Caret() == 6, L"1b");

			moved = line.CMoveToNextWordBegin();
			Assert::IsFalse(moved, L"2a");
			Assert::IsTrue(line.Caret() == 6, L"2b");

			// ----------------------------------------
			//      012345678
			line = "ab cd e ";

			line.CMoveBackward(2);
			Assert::IsTrue(line.Caret() == 6, L"3a");
			moved = line.CMoveToNextWordBegin();
			Assert::IsFalse(moved, L"3b");
			Assert::IsTrue(line.Caret() == 6, L"3c");

			// ----------------------------------------
			//      0
			line = "";

			moved = line.CMoveToNextWordBegin();
			Assert::IsFalse(moved, L"4a");
			Assert::IsTrue(line.Caret() == 0, L"4b");

			// ----------------------------------------
			//      01
			line = " ";

			moved = line.CMoveToNextWordBegin();
			Assert::IsFalse(moved, L"5a");
			Assert::IsTrue(line.Caret() == 1, L"5b");
		}
	};
}