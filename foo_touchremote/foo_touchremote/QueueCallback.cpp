#include "stdafx.h"
#include "QueueCallback.h"
#include "JukeboxPlaylist.h"
#include "ManagedHost.h"
#include "Utils.h"

#pragma managed

using namespace TouchRemote::Core;

namespace foo_touchremote
{
	namespace foobar
	{
		
		void QueueCallback::on_changed(t_change_origin origin)
		{
			if (ManagedHost::Instance->MediaLibrary->Jukebox != nullptr)
				((JukeboxPlaylist^)ManagedHost::Instance->MediaLibrary->Jukebox)->Invalidate();

			SessionManager::DatabaseUpdated();
		}

	}
}