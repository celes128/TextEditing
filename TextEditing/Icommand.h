#pragma once

namespace ted {

	class ICommand {
	public:
		ICommand() {}
		virtual ~ICommand() {}

		virtual void Execute() = 0;
		virtual void Undo() = 0;
	};
}