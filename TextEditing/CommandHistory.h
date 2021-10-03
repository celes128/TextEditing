#pragma once

#include "Icommand.h"
#include "FSOStack.h"

namespace ted {

	template <size_t CAPACITY>
	class CommandHistory {
	public:
		CommandHistory()
		{}

		~CommandHistory()
		{
			clear_stack(m_doneCmds);
			clear_stack(m_undoneCmds);
		}

		void Clear()
		{
			this->~CommandHistory();
		}

		void Execute(ICommand *cmd)
		{
			cmd->Execute();
			m_doneCmds.Push(cmd);
			clear_stack(m_undoneCmds);
		}

		// RETURN VALUE
		//	* true iff a command was undone.
		bool Undo()
		{
			if (!m_doneCmds.Empty()) {
				auto *cmd = m_doneCmds.Top();
				m_doneCmds.Pop();

				cmd->Undo();

				m_undoneCmds.Push(cmd);

				return true;
			}
			else return false;
		}

		// RETURN VALUE
		//	* true iff a command was redone.
		bool Redo()
		{
			if (!m_undoneCmds.Empty()) {
				auto *cmd = m_undoneCmds.Top();
				m_undoneCmds.Pop();

				cmd->Execute();

				m_doneCmds.Push(cmd);

				return true;
			}
			else return false;
		}

	private:
		void clear_stack(FSOStack<ICommand *, CAPACITY> &s)
		{
			while (!s.Empty()) {
				auto *cmd = s.Top();
				s.Pop();

				delete cmd;
			}
		}

	private:
		FSOStack<ICommand *, CAPACITY> m_doneCmds;
		
		FSOStack<ICommand *, CAPACITY> m_undoneCmds;
		// I could use an std::stack for this one.
	};
}