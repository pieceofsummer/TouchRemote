#pragma once

using namespace System;
using namespace System::Text;

namespace foo_touchremote
{
	pfc::string8 ToUtf8String(String^ value);
	String^ FromUtf8String(pfc::string8 &value);
	String^ FromUtf8String(const char *value);
}

namespace _console
{
	void error(String^ error);
	void print(String^ message);
	void printf(String ^message, ... cli::array<Object^>^ args);
}