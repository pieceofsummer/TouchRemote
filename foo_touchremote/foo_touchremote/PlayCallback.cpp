#include "stdafx.h"
#include "PlayCallback.h"
#include "ManagedHost.h"
#include "Utils.h"

#pragma managed

using namespace TouchRemote::Core;

namespace foo_touchremote
{
	namespace foobar
	{

		void PlayCallback::on_playback_starting(playback_control::t_track_command p_command, bool p_paused)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				ManagedHost::Instance->SetCurrentState(true, p_paused);
				//if (p_command != playback_control::track_command_resume)
				//	ManagedHost::Instance->SetCurrentPosition(0);
				//SessionManager::StateUpdated();
			}
			finally
			{
				//delete lock;
			}
		}

		void PlayCallback::on_playback_new_track(metadb_handle_ptr p_track)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				ManagedHost::Instance->SetCurrentTrack(p_track);
				ManagedHost::Instance->SetCurrentPosition(0);
				SessionManager::StateUpdated();
			}
			finally
			{
				//delete lock;
			}
		}

		void PlayCallback::on_playback_stop(playback_control::t_stop_reason p_reason)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				//ManagedHost::Instance->SetCurrentPosition(0);
				if (p_reason != playback_control::stop_reason_starting_another)
				{
					ManagedHost::Instance->SetCurrentTrack(metadb_handle_ptr());
					ManagedHost::Instance->SetCurrentState(false, false);
					SessionManager::StateUpdated();
				}
			}
			finally
			{
				//delete lock;
			}
		}

		void PlayCallback::on_playback_seek(double p_time)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				ManagedHost::Instance->SetCurrentPosition(p_time);

				SessionManager::StateUpdated();
			}
			finally
			{
				//delete lock;
			}
		}

		void PlayCallback::on_playback_pause(bool p_state)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				ManagedHost::Instance->SetCurrentState(true, p_state);
				SessionManager::StateUpdated();
			}
			finally
			{
				//delete lock;
			}
		}

		void PlayCallback::on_playback_edited(metadb_handle_ptr p_track)
		{
			/*static_api_ptr_t<playback_control> ctl;
			
			metadb_handle_ptr np;
			if (!ctl->get_now_playing(np)) return;*/

			SessionManager::StateUpdated();
		}

		void PlayCallback::on_playback_dynamic_info(const file_info &p_info)
		{
		}

		void PlayCallback::on_playback_dynamic_info_track(const file_info &p_info)
		{
		}

		void PlayCallback::on_playback_time(double p_time)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				ManagedHost::Instance->SetCurrentPosition(p_time);
			}
			finally
			{
				//delete lock;
			}
		}

		void PlayCallback::on_volume_change(float p_new_val)
		{
			//IDisposable^ lock = ManagedHost::Instance->BeginWrite();
			try
			{
				ManagedHost::Instance->SetCurrentVolume(p_new_val);

				SessionManager::StateUpdated();
			}
			finally
			{
				//delete lock;
			}
		}

		unsigned PlayCallback::get_flags()
		{
			return flag_on_playback_starting
				| flag_on_playback_new_track
				| flag_on_playback_stop	
				| flag_on_playback_seek
				| flag_on_playback_pause
				| flag_on_playback_edited
				//| flag_on_playback_dynamic_info
				//| flag_on_playback_dynamic_info_track
				| flag_on_playback_time
				| flag_on_volume_change;
		}

	}
}