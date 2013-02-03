#include "stdafx.h"
#include "AssemblyLoader.h"
#include "Utils.h"

#pragma managed

using namespace System;
using namespace System::Reflection;

namespace foo_touchremote
{

	void UnhandledExceptionCallback(Object^ sender, UnhandledExceptionEventArgs^ e)
	{
		String ^ex = e->ExceptionObject->ToString();
		_console::error(ex);
	}

	Assembly^ ResolveAssemblyCallback(Object^ sender, ResolveEventArgs^ e)
	{
		String^ fileName = String::Concat((gcnew AssemblyName(e->Name))->Name, ".dll");

        if (fileName->EndsWith(".resources.dll", StringComparison::InvariantCultureIgnoreCase))
        {
            // ignore satellite assemblies
            return nullptr;
        }

		//_console::printf("{0} not found in foobar's directory", fileName);

		String^ pluginPath = FromUtf8String(core_api::get_my_full_path());

		String^ componentsDir = IO::Path::GetDirectoryName(pluginPath);

		String^ path = IO::Path::Combine(componentsDir, fileName);

		//_console::printf("  trying {0}", path);
		if (IO::File::Exists(path))
		{
			//_console::print("    ok!");
			return Assembly::LoadFile(path);
		}

		//_console::print("    failed");

		_console::printf("{0} not found", fileName);
		_console::error("foobar may now crash :(");
		return nullptr;
	}

	void SetupAssemblyLoader()
	{
		AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(&ResolveAssemblyCallback);
		AppDomain::CurrentDomain->UnhandledException += gcnew UnhandledExceptionEventHandler(&UnhandledExceptionCallback);
	}

}