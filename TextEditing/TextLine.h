#pragma once

#include <string>
#include "framework.h"

namespace ted {

	class TextLine {
	public:
		TextLine(const char *text = "");

		const auto &String() { return m_text; }
		auto Length() const { return m_text.length(); }
		auto Caret() const { return m_caret; }

		// DeleteCharLeft deletes the character on the left of the caret, if there is one.
		// Returns true iff a character was delete.
		bool CDeleteLeftChar();
		
		// DeleteCharRight deletes the character on the right of the caret, if there is one.
		// Returns true iff a character was delete.
		bool CDeleteRightChar();

		// Returns true iff the textline was not empty before the call.
		bool Clear();
		
		// InsertCharLeft inserts a character at the position of the caret.
		// Returns true iff a character was inserted.
		bool InsertChar(char c);

		// Caret functions
		auto CPos() const { return m_caret; }
		bool CAtLineBegin() const { return 0 == m_caret; }
		bool CAtLineEnd() const { return m_caret == Length(); }
		bool CAtWordBegin() const;
		bool CAtWordEnd() const;
		bool CInsideWord() const;

		bool CMoveForward(size_t n = 1);
		bool CMoveBackward(size_t n = 1);

		bool CMoveToLineBegin();
		bool CMoveToLineEnd();

		bool CMoveToPrevWordBegin();
		bool CMoveToNextWordBegin();

	private:
		std::string		m_text;
		size_t			m_caret;
	};
}