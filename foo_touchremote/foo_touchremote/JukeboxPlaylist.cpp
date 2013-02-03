#include "stdafx.h"
#include "JukeboxPlaylist.h"
#include "Library.h"
#include "Track.h"
#include "JukeboxTrack.h"
#include "Utils.h"
#include "ManagedHost.h"
#include "MainThreadCallback.h"

#pragma managed

using namespace System::Threading;

namespace foo_touchremote
{

	JukeboxPlaylist::JukeboxPlaylist(IMediaLibrary^ library, String^ name) : SpecialPlaylistBase(library, PlaylistType::ITunesDJ, name)
	{
		m_library = library;
		m_trackCount = -1;
		m_tracks = nullptr;
	}

	JukeboxPlaylist::JukeboxPlaylist(IMediaLibrary^ library, String^ name, int trackCount) : SpecialPlaylistBase(library, PlaylistType::ITunesDJ, name)
	{
		m_library = library;
		m_trackCount = trackCount;
		m_tracks = nullptr;
	}

	JukeboxPlaylist::~JukeboxPlaylist()
	{
		Invalidate();
	}

	CALLBACK_START_MU(JukeboxPlaylistTracks_enum, List<ITrack^>^, int)
		static_api_ptr_t<playlist_manager> mgr;

		ManagedHost^ host = ManagedHost::Instance;

		pfc::list_t<t_playback_queue_item> items;
		mgr->queue_get_contents(items);

		List<ITrack^>^ list = gcnew List<ITrack^>(items.get_count());
		
		metadb_handle_ptr current;
		if (static_api_ptr_t<playback_control>()->get_now_playing(current))
			list->Add(gcnew JukeboxTrack(host->MediaLibrary, current));

		for(t_size i = 0; i < items.get_count(); i++)
		{
			//list->Add(host->GetTrack(items[i].m_handle));
			list->Add(gcnew JukeboxTrack(host->MediaLibrary, items[i].m_handle));
		}

		return list;
	CALLBACK_END()

	System::Collections::Generic::IEnumerable<ITrack^>^ JukeboxPlaylist::Tracks::get()
	{
		if (m_tracks == nullptr)
		{
			m_tracks = (new JukeboxPlaylistTracks_enum())->Run(this);
			m_trackCount = m_tracks->Count;
		}
		return m_tracks;
	}

	CALLBACK_START_UU(JukeboxPlaylistTracks_count, t_size, int)
		return static_api_ptr_t<playlist_manager>()->queue_get_count() + (static_api_ptr_t<playback_control>()->is_playing() ? 1 : 0);
	CALLBACK_END()

	int JukeboxPlaylist::TrackCount::get()
	{
		if (m_tracks == nullptr)
		{
			if (m_trackCount == -1)
				m_trackCount = (new JukeboxPlaylistTracks_count())->Run(this);
			return m_trackCount;
		}
		return m_tracks->Count;
	}

	void JukeboxPlaylist::Extend(System::Collections::Generic::IDictionary<String^, Object^>^ data)
	{
		if (TrackCount > 0)
			data["ceJI"] = ((IList<ITrack^>^)Tracks)[0]->Id;
	}

	void JukeboxPlaylist::Invalidate()
	{
		Monitor::Enter(this);
		try
		{
			m_tracks = nullptr;
			m_trackCount = -1;
		}
		finally
		{
			Monitor::Exit(this);
		}
	}

	CALLBACK_START_UM(JukeboxPlaylist_vote, int, ITrack^)
		static_api_ptr_t<playback_control> mgr;
		static_api_ptr_t<playlist_manager> pl;

		Track^ t = safe_cast<Track^>(arg);
		if (t != nullptr)
		{
			pl->queue_add_item(t->GetHandle());
		}

		return 0;
	CALLBACK_END()

	void JukeboxPlaylist::Vote(ITrack^ track)
	{
		if (track == nullptr)
			throw gcnew ArgumentNullException("track");

		(new JukeboxPlaylist_vote())->Run(this, track);
	}

}