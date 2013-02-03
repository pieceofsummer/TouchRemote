#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote 
{

	public ref class ConsoleLogger : public ILogger
	{
	public:
		ConsoleLogger();
		
		virtual void LogMessage(String^ message, ... cli::array<Object^>^ args);
		virtual void LogError(Exception^ error);

	};

}