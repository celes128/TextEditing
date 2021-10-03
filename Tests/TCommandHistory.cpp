#include "pch.h"
#include "CppUnitTest.h"

#include "../TextEditing/TextLine.h"
#include "../TextEditing/CmdInsertChar.h"
#include "../TextEditing/CommandHistory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	TEST_CLASS(TCommandHistory) {
	public:
		TEST_METHOD(TCommandHistory_Ctor)
		{
			ted::CommandHistory<3>	hist;
			bool					done;

			done = hist.Redo();
			Assert::IsFalse(done, L"0");
			done = hist.Undo();
			Assert::IsFalse(done, L"1");
		}

		TEST_METHOD(TCommandHistory_Execute)
		{
			ted::CommandHistory<3>	hist;
			ted::TextLine			line;

			hist.Execute(new ted::CmdInsertChar('a', 0, line));
			Assert::IsTrue(line.String() == "a", L"0");
		}

		TEST_METHOD(TCommandHistory_UndoRedo)
		{
			ted::CommandHistory<3>	hist;
			ted::TextLine			line;
			bool					done;

			hist.Execute(new ted::CmdInsertChar('a', 0, line));
			Assert::IsTrue(line.String() == "a", L"0");

			done = hist.Undo();
			Assert::IsTrue(done, L"1a");
			Assert::IsTrue(line.String() == "", L"1b");

			done = hist.Redo();
			Assert::IsTrue(done, L"2a");
			Assert::IsTrue(line.String() == "a", L"2b");
		}
	};
}