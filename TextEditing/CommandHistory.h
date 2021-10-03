#pragma once

#include "Icommand.h"
#include "FSOStack.h"

namespace ted {

	template <size_t CAPACITY>
	class CommandHistory {
	public:
		using CommandStack = FSOStack<ICommand *, CAPACITY>;
		
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
			return transfer_one(m_doneCmds, m_undoneCmds, Action::Undo);
		}

		// RETURN VALUE
		//	* true iff a command was redone.
		bool Redo()
		{
			return transfer_one(m_undoneCmds, m_doneCmds, Action::Execute);
		}

	private:
		void clear_stack(CommandStack &s)
		{
			while (!s.Empty()) {
				auto *cmd = s.Top();
				s.Pop();

				delete cmd;
			}
		}

		enum class Action { Execute, Undo };

		// transfer_one...
		//	... pops a command from the source stack,
		//	... execute or undo it based on the specified action and,
		//	... pushes it onto the destination stack.
		// transfer_one does nothing if the source stack is empty.
		//
		// RETURN VALUE
		//	* true iff a command was transfered.
		bool transfer_one(CommandStack &src, CommandStack &dest, Action action)
		{
			if (!src.Empty()) {
				auto *cmd = src.Top();
				src.Pop();

				if (action == Action::Execute) cmd->Execute();
				else if (action == Action::Undo) cmd->Undo();

				dest.Push(cmd);

				return cmd;
			}
			else return nullptr;
		}

	private:
		CommandStack m_doneCmds;
		
		CommandStack m_undoneCmds;
		// I could use an std::stack for this one.
	};
}