#include "StdAfx.h"
#include "ConsoleLogger.h"
#include "Utils.h"

#pragma managed

namespace foo_touchremote
{

	ConsoleLogger::ConsoleLogger()
	{
	}

	void ConsoleLogger::LogMessage(System::String ^message, ... cli::array<System::Object ^,1> ^args)
	{
		if (String::IsNullOrEmpty(message)) return;

		_console::printf(message, args);
	}

	void ConsoleLogger::LogError(System::Exception ^error)
	{
		if (error == nullptr) return;

		_console::error(String::Format("[{0}] {1}", error->GetType()->FullName, error->Message));
		_console::print(error->StackTrace);
	}

}