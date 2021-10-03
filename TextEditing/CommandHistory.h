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
			auto *cmd = transfer_one(m_doneCmds, m_undoneCmds);
			if (cmd) {
				cmd->Undo();
				return true;
			}
			else return false;
		}

		// RETURN VALUE
		//	* true iff a command was redone.
		bool Redo()
		{
			auto *cmd = transfer_one(m_undoneCmds, m_doneCmds);
			if (cmd) {
				cmd->Execute();
				return true;
			}
			else return false;
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

		// transfer_one pops a command from the source stack and
		// pushes it onto the destination stack.
		// Does nothing if the source stack is empty.
		//
		// RETURN VALUE
		//	* a pointer to the transfered command if there is one else nullptr.
		ICommand *transfer_one(CommandStack &src, CommandStack &dest)
		{
			if (!src.Empty()) {
				auto *cmd = src.Top();
				src.Pop();

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