#include "pch.h"
#include "TextLine.h"
#include <cctype>

namespace ted {

	bool TextLine::CAtWordBegin() const
	{
		if (CAtLineEnd()) return false;
		if (CAtLineBegin()) return !std::isspace(m_text[m_caret]);

		// Here the following assertions are true:
		//	* m_text.length() >= 1,
		//	* 0 < m_caret < m_text.length().
		//	* we can inspect the character at index m_caret-1 (m_text[m_caret-1])

		if (std::isspace(m_text[m_caret])) return false;
		else return std::isspace(m_text[m_caret - 1]);

		return !std::isspace(m_text[m_caret]) && std::isspace(m_text[m_caret - 1]);
	}

	bool TextLine::CAtWordEnd() const
	{
		const auto len = Length();

		if (len == 0) return false;
		if (CAtLineEnd()) return !std::isspace(m_text[m_caret - 1]);
		if (CAtLineBegin()) return false;

		return std::isspace(m_text[m_caret]) && !std::isspace(m_text[m_caret - 1]);
	}

	bool TextLine::CMoveForward(size_t n)
	{
		const auto prev = m_caret;
		const auto len = Length();

		m_caret += n;
		if (m_caret > len) {
			CMoveToLineEnd();
		}

		return m_caret != prev;
	}

	bool TextLine::CMoveBackward(size_t n)
	{
		const auto prev = m_caret;
		const auto len = Length();

		if (n <= m_caret) m_caret -= n;
		else m_caret = 0;

		return m_caret != prev;
	}

	bool TextLine::CMoveToLineBegin()
	{
		if (!CAtLineBegin()) {
			m_caret = 0;
			return true;
		}
		else return false;
	}

	bool TextLine::CMoveToLineEnd()
	{
		if (!CAtLineEnd()) {
			m_caret = Length();
			return true;
		}
		else return false;
	}

	// word_begin returns the index of the first non-space character
	// of the word that intersects the index i in the string s.
	// PRECONDITIONS
	//	* s.length() >= 1
	//	* !std::isspace(s[i])
	static size_t word_begin(const std::string &s, size_t i)
	{
		const auto len = s.length();

		assert(len >= 1);
		assert(!std::isspace(s[i]));

		if (i != 0) {
			for (auto j = i; j >= 1; --j) {
				if (std::isspace(s[j - 1])) {
					return j;
				}
			}
			return 0;
		}
		else {
			return 0;
		}
	}

	// PRECONDITIONS
	//	* s.length() >= 1
	//	* i < s.length()
	//	* !std::isspace(s[i])
	// word_end returns the index of the character that follows the last character
	// of the word that intersects index i.
	// Returns s.length() if s ends in this word.
	//
	// EXAMPLES
	//	Let suppose that s = "ab cde f". The length of s is 9.
	//
	//		0123456789
	//		ab cde  f
	//
	//	word_end(s, 0) = word_end(s, 1) = 2
	//	word_end(s, 3) = word_end(s, 4) = word_end(s, 5) = 6
	//	word_end(s, 8) = 9
	//	Any other index causes undefined behaviour.
	static size_t word_end(const std::string &s, size_t i)
	{
		const auto len = s.length();
		assert(len >= 1);
		assert(i < len);
		assert(!std::isspace(s[i]));

		for (; i < len && !std::isspace(s[i]); ++i)
			;
		return i;
	}

	bool TextLine::CMoveToPrevWordBegin()
	{
		const auto len = Length();
		if (len == 0) return false;

		// Save current position to check if we moved
		const auto prev = m_caret;
		// Make i point to a space character
		size_t i;
		if (CAtLineEnd()) {
			if (!std::isspace(m_text[m_caret - 1])) {
				m_caret = word_begin(m_text, m_caret - 1);
				return prev != m_caret;
			}
			else i = m_caret - 1;
		}
		else {
			if (!std::isspace(m_text[m_caret])) {
				auto j = word_begin(m_text, m_caret);
				if (0 == j) return false;
				else i = j - 1;
			}
		}

		// Here i points to space character

		// Move backward in the line until we find a non-space character.
		// However we might reach the start of the line.
		//				***** WARNING *****
		// i is an unsigned type so I cannot use i >= 0 in the condition.
		for (; std::isspace(m_text[i]); --i) {
			if (0 == i) return std::isspace(m_text[i]);
		}

		// Here i >= 1 and !std::isspace(m_text[i])
		m_caret = word_begin(m_text, i);

		return m_caret != prev;
	}

	bool TextLine::CMoveToNextWordBegin()
	{
		const auto len = Length();
		if (len == 0) return false;
		if (CAtLineEnd()) return false;

		// Here the following is true:
		//	* len >= 1,
		//	* 0 <= m_caret < len - 1.

		// Save current position to check if we moved
		const auto prev = m_caret;
		// Make i point to a space character
		size_t i;
		if (!std::isspace(m_text[m_caret])) {
			i = word_end(m_text, m_caret);
			if (i == len) return false;// the line ends by this word
		}

		// Here m_text[i] is a space and 1 <= i < len

		// Skip all the spaces
		for (; std::isspace(m_text[i]); ++i) {
			if (i + 1 == len) return false;// the line ends by spaces
		}

		// Here i < len and m_text[i] is not a space.
		m_caret = i;
		return prev != m_caret;
	}


	// ------------------------------------
	//				TextLine
	// ------------------------------------

	TextLine::TextLine(const char *text)
		: m_text(text)
		, m_caret(m_text.length())
	{}

	bool TextLine::CDeleteLeftChar()
	{
		if (m_caret > 0) {
			CMoveBackward();
			m_text.erase(m_caret, 1);
			return true;
		}
		return false;
	}

	bool TextLine::Clear()
	{
		size_t n = m_text.length();
		if (n > 0) {
			m_text.clear();
			CMoveToLineBegin();
			return true;
		}
		else return false;
	}

	bool TextLine::InsertChar(char c)
	{
		m_text.insert(m_caret, 1, c);
		CMoveForward();
		return true;
	}
}