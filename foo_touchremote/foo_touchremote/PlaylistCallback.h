#pragma once

namespace foo_touchremote
{
	namespace foobar
	{

		class PlaylistCallback : public service_impl_t<playlist_callback_static>
		{
		public:
			virtual void on_items_added(t_size p_playlist,t_size p_start, metadb_handle_list_cref p_data,const bit_array & p_selection);
			virtual void on_items_reordered(t_size p_playlist,const t_size * p_order,t_size p_count);
			virtual void on_items_removing(t_size p_playlist,const bit_array & p_mask,t_size p_old_count,t_size p_new_count);
			virtual void on_items_removed(t_size p_playlist,const bit_array & p_mask,t_size p_old_count,t_size p_new_count);
			virtual void on_items_selection_change(t_size p_playlist,const bit_array & p_affected,const bit_array & p_state);
			virtual void on_item_focus_change(t_size p_playlist,t_size p_from,t_size p_to);
			virtual void on_items_modified(t_size p_playlist,const bit_array & p_mask);
			virtual void on_items_modified_fromplayback(t_size p_playlist,const bit_array & p_mask,play_control::t_display_level p_level);
			virtual void on_items_replaced(t_size p_playlist,const bit_array & p_mask, const pfc::list_base_const_t<t_on_items_replaced_entry> & p_data);
			virtual void on_item_ensure_visible(t_size p_playlist,t_size p_idx);

			virtual void on_playlist_activate(t_size p_old,t_size p_new);
			virtual void on_playlist_created(t_size p_index,const char * p_name,t_size p_name_len);
			virtual void on_playlists_reorder(const t_size * p_order,t_size p_count);
			virtual void on_playlists_removing(const bit_array & p_mask,t_size p_old_count,t_size p_new_count);
			virtual void on_playlists_removed(const bit_array & p_mask,t_size p_old_count,t_size p_new_count);
			virtual void on_playlist_renamed(t_size p_index,const char * p_new_name,t_size p_new_name_len);

			virtual void on_default_format_changed();
			virtual void on_playback_order_changed(t_size p_new_index);
			virtual void on_playlist_locked(t_size p_playlist,bool p_locked);

			virtual unsigned get_flags();
		};

	}
}