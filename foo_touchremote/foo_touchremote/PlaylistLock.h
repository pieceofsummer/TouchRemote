#pragma once

namespace foo_touchremote
{
	namespace foobar
	{

		class PlaylistLock : public service_impl_single_t<playlist_lock>
		{
			
		public:

			virtual bool query_items_add(t_size p_base, const pfc::list_base_const_t<metadb_handle_ptr> & p_data,const bit_array & p_selection);
			virtual bool query_items_reorder(const t_size * p_order,t_size p_count);
			virtual bool query_items_remove(const bit_array & p_mask,bool p_force);
			virtual bool query_item_replace(t_size p_index,const metadb_handle_ptr & p_old,const metadb_handle_ptr & p_new);
			virtual bool query_playlist_rename(const char * p_new_name,t_size p_new_name_len);
			virtual bool query_playlist_remove();
			virtual bool execute_default_action(t_size p_item);
			virtual void on_playlist_index_change(t_size p_new_index);
			virtual void on_playlist_remove();
			virtual void get_lock_name(pfc::string_base & p_out);
			virtual void show_ui();
			virtual t_uint32 get_filter_mask();

		};

	}
}