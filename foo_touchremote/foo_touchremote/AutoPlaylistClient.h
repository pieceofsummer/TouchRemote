#pragma once

using namespace TouchRemote::Interfaces;

namespace foo_touchremote
{
	namespace foobar
	{
		
		class AutoPlaylistClient : public service_impl_t<autoplaylist_client_v2>
		{
		public:
			AutoPlaylistClient(array<ITrack^>^ tracks);

			virtual GUID get_guid();

			virtual void filter(metadb_handle_list_cref data, bool * out);

			virtual bool sort(metadb_handle_list_cref p_items,t_size * p_orderbuffer);

			virtual void get_configuration(stream_writer * p_stream,abort_callback & p_abort);

			virtual void show_ui(t_size p_source_playlist);

			virtual void set_full_refresh_notify(completion_notify::ptr notify);

			virtual bool show_ui_available();

			virtual void get_display_name(pfc::string_base & out);

		private:
			pfc::list_t<playable_location_impl> items;
		};

	}
}