#include "stdafx.h"
#include "AutoPlaylistClient.h"
#include "ManagedHost.h"
#include "Track.h"

#pragma managed

using namespace System::Threading;

namespace foo_touchremote
{
	namespace foobar
	{

		AutoPlaylistClient::AutoPlaylistClient(array<ITrack^>^ tracks)
		{
			if (tracks != nullptr)
			{
				items.set_size(tracks->Length);

				for (int i = 0; i < tracks->Length; i++)
					items[i] = ((Track^)tracks[i])->GetHandle()->get_location();
			}
		}

		GUID AutoPlaylistClient::get_guid()
		{
			return foo_touchremote::guids::AutoPlaylistClient;	
		}

		void AutoPlaylistClient::get_display_name(pfc::string_base & out)
		{
			out = "TouchRemote Selection";
		}

		void AutoPlaylistClient::show_ui(t_size p_source_playlist) { }

		bool AutoPlaylistClient::show_ui_available() { return false; }

		void AutoPlaylistClient::set_full_refresh_notify(completion_notify::ptr notify) { }

		void AutoPlaylistClient::get_configuration(stream_writer * p_stream, abort_callback & p_abort) { }

		void AutoPlaylistClient::filter(metadb_handle_list_cref data, bool * out)
		{
			for (t_size i = 0; i < data.get_count(); i++)
				out[i] = items.find_item(data[i]->get_location()) != pfc::infinite32;
		}

		bool AutoPlaylistClient::sort(metadb_handle_list_cref p_items, t_size * p_orderbuffer)
		{
			for (t_size i = 0; i < p_items.get_count(); i++)
			{
				t_size x_index = items.find_item(p_items[i]->get_location());
				p_orderbuffer[x_index] = i;
			}
			return true;
		}

	}
}