#include "stdafx.h"
#include "LibraryCallback.h"
#include "Utils.h"
#include "ManagedHost.h"
#include "Library.h"
#include "Track.h"

#pragma managed

namespace foo_touchremote
{
	namespace foobar
	{

		LibraryCallback::LibraryCallback()
		{

		}

		void LibraryCallback::on_items_added(metadb_handle_list_cref p_data)
		{
			_console::printf("Added {0} items", p_data.get_count());

			Library^ lib = (Library^)ManagedHost::Instance->MediaLibrary;
			
			IDisposable^ lock = lib->BeginWrite();
			try
			{
				for (t_size i = 0; i < p_data.get_count(); i++)
					lib->AddTrack(p_data[i]);
			}
			finally
			{
				delete lock;
				_console::print("Changes merged into library");
			}

			TouchRemote::Core::SessionManager::DatabaseUpdated();
		}

		void LibraryCallback::on_items_removed(metadb_handle_list_cref p_data)
		{
			_console::printf("Removed {0} items", p_data.get_count());

			Library^ lib = (Library^)ManagedHost::Instance->MediaLibrary;

			IDisposable^ lock = lib->BeginWrite();
			try
			{
				for (t_size i = 0; i < p_data.get_count(); i++)
					lib->RemoveTrack(p_data[i]);
			}
			finally
			{
				delete lock;
				_console::print("Changes merged into library");
			}

			TouchRemote::Core::SessionManager::DatabaseUpdated();
		}

		void LibraryCallback::on_items_modified(metadb_handle_list_cref p_data)
		{
			_console::printf("Modified {0} items", p_data.get_count());

			Library^ lib = (Library^)ManagedHost::Instance->MediaLibrary;

			IDisposable^ lock = lib->BeginWrite();
			try
			{
				for (t_size i = 0; i < p_data.get_count(); i++)
					lib->AddTrack(p_data[i]);
			}
			finally
			{
				delete lock;
				_console::print("Changes merged into library");
			}

			/*static_api_ptr_t<playlist_manager> m;
			
			t_uint32 index = m->find_playlist(SELECTION_PLAYLIST_NAME);
			if (index != pfc::infinite32)
			{
				
			}*/

			/*array<ITrack^>^ playbackSource = ManagedHost::Instance->GetPlaybackSource();
			if (playbackSource != nullptr && playbackSource->Length > 0)
			{
				bool playbackSourceAffected = false;

				lock = lib->BeginRead();
				try
				{
					for (int i = 0; i < playbackSource->Length; i++)
					{
						ITrack^ actualOne = lib->GetTrackCore(playbackSource[i]->Source);
						if (!Object::ReferenceEquals(actualOne, playbackSource[i]))
						{
							playbackSource[i] = actualOne;
							playbackSourceAffected = true;
						}
					}
				}
				finally
				{
					delete lock;
				}

				ManagedHost::Instance->SetPlaybackSource(playbackSource);
			}*/

			TouchRemote::Core::SessionManager::DatabaseUpdated();
		}

	}
}