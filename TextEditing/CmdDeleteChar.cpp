#include "pch.h"
#include "CmdDeleteChar.h"

#include "TextLine.h"

namespace ted {

	CmdDeleteChar::CmdDeleteChar(size_t i, TextLine &line)
		: m_c(line[i])
		, m_i(i)
		, m_line(line)
	{}

	void CmdDeleteChar::Execute()
	{
		m_line.DeleteCharAtIndex(m_i);
	}

	void CmdDeleteChar::Undo()
	{
		m_line.InsertAtIndex(m_c, m_i);
	}
}