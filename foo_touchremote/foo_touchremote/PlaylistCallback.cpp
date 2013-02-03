#include "stdafx.h"
#include "PlaylistCallback.h"
#include "ManagedHost.h"
#include "Utils.h"

#pragma managed

using namespace TouchRemote::Core;

namespace foo_touchremote
{
	namespace foobar
	{
		
		bool is_selection_playlist(t_size index)
		{
			if (index == pfc::infinite32) return false;

			pfc::string8_fastalloc name;

			static_api_ptr_t<playlist_manager> mgr;

			if (!mgr->playlist_get_name(index, name)) return false;

			return (0 == strcmp(name, SELECTION_PLAYLIST_NAME));
		}

		void PlaylistCallback::on_items_added(t_size p_playlist, t_size p_start, metadb_handle_list_cref p_data, const bit_array &p_selection)
		{
			if (is_selection_playlist(p_playlist)) return;

			ManagedHost::Instance->InvalidatePlaylist(p_playlist);
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_items_reordered(t_size p_playlist, const t_size *p_order, t_size p_count)
		{
			if (is_selection_playlist(p_playlist)) return;

			ManagedHost::Instance->InvalidatePlaylist(p_playlist);
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_items_removing(t_size p_playlist, const bit_array &p_mask, t_size p_old_count, t_size p_new_count)
		{
		}

		void PlaylistCallback::on_items_removed(t_size p_playlist, const bit_array &p_mask, t_size p_old_count, t_size p_new_count)
		{
			if (is_selection_playlist(p_playlist)) return;

			ManagedHost::Instance->InvalidatePlaylist(p_playlist);
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_items_selection_change(t_size p_playlist,const bit_array &p_affected, const bit_array &p_state)
		{
		}

		void PlaylistCallback::on_item_focus_change(t_size p_playlist, t_size p_from, t_size p_to)
		{
		}

		void PlaylistCallback::on_items_modified(t_size p_playlist, const bit_array &p_mask)
		{
			if (is_selection_playlist(p_playlist)) return;

			static_api_ptr_t<playlist_manager> mgr;

			t_size count = mgr->playlist_get_item_count(p_playlist);
			if (count != pfc::infinite32)
			{
				ManagedHost^ host = ManagedHost::Instance;

				for (t_size i = 0; i < count; i++)
				{
					if (!p_mask[i]) continue;
					
					metadb_handle_ptr ptr;
					if (mgr->playlist_get_item_handle(ptr, p_playlist, i))
					{
						host->LazyUpdateTrack(ptr);
					}
				}
			}
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_items_modified_fromplayback(t_size p_playlist, const bit_array &p_mask, playback_control::t_display_level p_level)
		{
		}

		void PlaylistCallback::on_items_replaced(t_size p_playlist, const bit_array &p_mask, const pfc::list_base_const_t<t_on_items_replaced_entry> & p_data)
		{
			if (is_selection_playlist(p_playlist)) return;

			ManagedHost::Instance->InvalidatePlaylist(p_playlist);
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_item_ensure_visible(t_size p_playlist, t_size p_idx)
		{
		}

		void PlaylistCallback::on_playlist_activate(t_size p_old, t_size p_new)
		{
			ManagedHost::Instance->SetCurrentPlaylistInvalid();
		}

		void PlaylistCallback::on_playlist_created(t_size p_index, const char *p_name, t_size p_name_len)
		{
			if (is_selection_playlist(p_index)) return;

			ManagedHost::Instance->AddPlaylist(p_index, FromUtf8String(p_name));
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_playlists_reorder(const t_size *p_order, t_size p_count)
		{
			ManagedHost::Instance->InvalidateAllPlaylists();
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_playlists_removing(const bit_array &p_mask, t_size p_old_count, t_size p_new_count)
		{
		}

		void PlaylistCallback::on_playlists_removed(const bit_array &p_mask, t_size p_old_count, t_size p_new_count)
		{
			ManagedHost::Instance->InvalidateAllPlaylists();
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_playlist_renamed(t_size p_index, const char *p_new_name, t_size p_new_name_len)
		{
			ManagedHost::Instance->RenamePlaylist(p_index, FromUtf8String(p_new_name));
			SessionManager::DatabaseUpdated();
		}

		void PlaylistCallback::on_default_format_changed()
		{
		}

		void PlaylistCallback::on_playback_order_changed(t_size p_new_index)
		{
			ManagedHost::Instance->SetCurrentPlaybackOrder(p_new_index);
			SessionManager::StateUpdated();
		}

		void PlaylistCallback::on_playlist_locked(t_size p_playlist, bool p_locked)
		{
		}

		unsigned PlaylistCallback::get_flags()
		{
			return flag_all;
		}

	}
}