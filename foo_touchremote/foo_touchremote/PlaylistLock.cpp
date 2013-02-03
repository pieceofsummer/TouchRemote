#include "stdafx.h"
#include "PlaylistLock.h"

#pragma managed

namespace foo_touchremote
{
	namespace foobar
	{

		bool PlaylistLock::query_items_add(t_size p_base, const pfc::list_base_const_t<metadb_handle_ptr> & p_data,const bit_array & p_selection)
		{
			return false;
		}

		bool PlaylistLock::query_items_reorder(const t_size * p_order,t_size p_count)
		{
			return false;
		}

		bool PlaylistLock::query_items_remove(const bit_array & p_mask,bool p_force)
		{
			return false;
		}
		
		bool PlaylistLock::query_item_replace(t_size p_index,const metadb_handle_ptr & p_old,const metadb_handle_ptr & p_new)
		{
			return (p_old->get_location() == p_new->get_location());
		}

		bool PlaylistLock::query_playlist_rename(const char * p_new_name,t_size p_new_name_len) 
		{ 
			return false; 
		}

		bool PlaylistLock::query_playlist_remove() 
		{ 
			return true; 
		}

		bool PlaylistLock::execute_default_action(t_size p_item) 
		{ 
			return false; 
		}

		void PlaylistLock::on_playlist_index_change(t_size p_new_index) { }

		void PlaylistLock::on_playlist_remove() { }

		void PlaylistLock::get_lock_name(pfc::string_base & p_out)
		{
			p_out = "TouchRemote Selection";
		}

		void PlaylistLock::show_ui() { }

		t_uint32 PlaylistLock::get_filter_mask()
		{
			return playlist_lock::filter_add |
				playlist_lock::filter_remove |
				playlist_lock::filter_reorder |
				playlist_lock::filter_replace |
				playlist_lock::filter_rename;
		}


	}
}