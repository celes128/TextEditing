#pragma once

#include "Icommand.h"

namespace ted {

	class TextLine;

	class CmdInsertChar : public ICommand {
	public:
		CmdInsertChar(char c, size_t i, TextLine &line);
		~CmdInsertChar() {}

		void Execute() override;
		void Undo() override;

	private:
		char		m_c;
		size_t		m_i;
		TextLine	&m_line;
	};
}