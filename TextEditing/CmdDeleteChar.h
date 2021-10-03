#pragma once

#include "Icommand.h"

namespace ted {

	class TextLine;

	class CmdDeleteChar : public ICommand {
	public:
		CmdDeleteChar(size_t i, TextLine &line);
		~CmdDeleteChar() {}

		void Execute() override;
		void Undo() override;

	private:
		char		m_c;
		size_t		m_i;
		TextLine	&m_line;
	};
}