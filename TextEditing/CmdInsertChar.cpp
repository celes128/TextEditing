#include "pch.h"
#include "CmdInsertChar.h"

#include "TextLine.h"

namespace ted {

	CmdInsertChar::CmdInsertChar(char c, size_t i, TextLine &line)
		: m_c(c)
		, m_i(i)
		, m_line(line)
	{}

	void CmdInsertChar::Execute()
	{
		m_line.InsertAtIndex(m_c, m_i);
	}

	void CmdInsertChar::Undo()
	{
		m_line.DeleteCharAtIndex(m_i);
	}
}