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
		
		// InsertAtCaret inserts a character at the position of the caret.
		// Returns true iff a character was inserted.
		bool InsertAtCaret(char c);

		// InsertAtIndex inserts a character at the specified index.
		// Does nothing if i > Length().
		bool InsertAtIndex(char c, size_t i);

		// DeleteCharAtIndex deletes the character at the specified index.
		// Does nothing if i >= Length().
		bool DeleteCharAtIndex(size_t i, char *c = nullptr);

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

		// Does nothing if !CInsideWord() && !CAtWordEnd()
		bool CMoveToWordBegin();

		bool CMoveToPrevWordBegin();
		bool CMoveToNextWordBegin();

	private:
		std::string		m_text;
		size_t			m_caret;
	};
}