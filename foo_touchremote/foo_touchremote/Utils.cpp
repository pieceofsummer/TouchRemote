#include "StdAfx.h"
#include "Utils.h"

using namespace System::Runtime::InteropServices;

#pragma managed

namespace foo_touchremote 
{

	String^ FromUtf8String(const char *value)
	{
		if (value == NULL)
			return nullptr;

		t_size len = strlen(value);
		if (len == 0)
			return String::Empty;

		cli::array<unsigned char>^ bytes = gcnew cli::array<unsigned char>(len);

		Marshal::Copy((IntPtr)(void*)value, bytes, 0, len);

		return Encoding::UTF8->GetString(bytes);
	}

	String^ FromUtf8String(pfc::string8 &value)
	{
		return FromUtf8String(value.get_ptr());
	}

	pfc::string8 ToUtf8String(String ^value)
	{
		if (String::IsNullOrEmpty(value)) return pfc::string8();

		cli::array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(value);

		int len = bytes->Length;
		
		pfc::string8 buffer;
		
		void* ptr = buffer.lock_buffer(len);

		Marshal::Copy(bytes, 0, (IntPtr)ptr, len);

		buffer.unlock_buffer();

		return buffer;
	}

}

namespace _console
{

	void print(String ^message)
	{
		if (String::IsNullOrEmpty(message)) return;

		cli::array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(message);

		int len = bytes->Length;
		
		pfc::string8 buffer;
		
		void* ptr = buffer.lock_buffer(len);

		Marshal::Copy(bytes, 0, (IntPtr)ptr, len);

		buffer.unlock_buffer();

		console::print(buffer.get_ptr());
	}

	void error(String ^error)
	{
		if (String::IsNullOrEmpty(error)) return;

		cli::array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(error);

		int len = bytes->Length;
		
		pfc::string8 buffer;
		
		void* ptr = buffer.lock_buffer(len);

		Marshal::Copy(bytes, 0, (IntPtr)ptr, len);

		buffer.unlock_buffer();

		console::error(buffer.get_ptr());
	}

	void printf(String ^message, ... cli::array<Object^>^ args)
	{
		if (String::IsNullOrEmpty(message)) return;
		
		try
		{
			print(String::Format(message, args));
		}
		catch (FormatException ^e)
		{
			error(e->ToString());
		}
	}

}
