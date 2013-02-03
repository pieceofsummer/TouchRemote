#include "stdafx.h"
#include "InitQuit.h"
#include "ManagedHost.h"
#include "AssemblyLoader.h"
#include "Utils.h"

#pragma managed

namespace foo_touchremote
{

	namespace foobar
	{

		InitQuit::InitQuit()
		{			
			SetupAssemblyLoader();
		}

		void InitQuit::on_init()
		{
			try
			{
				ManagedHost::Instance->OnStart();
			}
			catch (Exception^ ex)
			{
				_console::print(ex->ToString());
			}
			catch (pfc::exception & e)
			{
				console::print(e.what());
			}
		}

		void InitQuit::on_quit()
		{
			ManagedHost::Instance->OnQuit();
		}

	}

}