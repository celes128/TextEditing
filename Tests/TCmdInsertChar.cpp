#include "pch.h"
#include "CppUnitTest.h"

#include "../TextEditing/TextLine.h"
#include "../TextEditing/CmdInsertChar.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {
	TEST_CLASS(TCmdInsertChar) {
	public:
		TEST_METHOD(CmdInsertChar)
		{
			ted::TextLine line;

			ted::CmdInsertChar cmd('a', 0, line);
			cmd.Execute();
			Assert::IsTrue(line.String() == "a", L"1a");

			cmd.Undo();
			Assert::IsTrue(line.String() == "", L"1b");
		}
	};
}