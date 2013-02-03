#include "stdafx.h"
#include "Playlist.h"
#include "Library.h"
#include "IDProvider.h"
#include "Track.h"
#include "Utils.h"
#include "ManagedHost.h"
#include "MainThreadCallback.h"

#pragma managed

using namespace System::Threading;

namespace foo_touchremote
{

	Playlist::Playlist(IMediaLibrary^ library, int index, String^ name)
	{
		Playlist(library, index, name, -1);
	}

	Playlist::Playlist(IMediaLibrary^ library, int index, String^ name, int trackCount)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");
		if (String::IsNullOrEmpty(name))
			throw gcnew ArgumentNullException("name");

		m_library = library;
		m_index = index;
		m_name = name;
		m_trackCount = trackCount;
		m_tracks = nullptr;
	}

	Playlist::~Playlist()
	{
		Invalidate();
	}

	int Playlist::Id::get()
	{
		return ((Library^)m_library)->Identifiers->GetId(this);
	}

	__int64 Playlist::PersistentId::get()
	{
		return ((Library^)m_library)->Identifiers->GetPersistentId(this);
	}

	CALLBACK_START_MU(PlaylistTracks_enum, List<ITrack^>^, t_size)
		static_api_ptr_t<playlist_manager> mgr;	

		ManagedHost^ host = ManagedHost::Instance;

		metadb_handle_list items;
		mgr->playlist_get_all_items(arg, items);

		List<ITrack^>^ list = gcnew List<ITrack^>(items.get_count());

		for(t_size i = 0; i < items.get_count(); i++)
			list->Add(host->GetTrack(items[i]));

		return list;
	CALLBACK_END()

	System::Collections::Generic::IEnumerable<ITrack^>^ Playlist::Tracks::get()
	{
		if (m_tracks == nullptr)
		{
			m_tracks = (new PlaylistTracks_enum())->Run(this, m_index);
			m_trackCount = m_tracks->Count;
		}
		return m_tracks;
	}

	String^ Playlist::Name::get()
	{
		return m_name;
	}

	void Playlist::Name::set(String^ value)
	{
		m_name = value;
	}

	PlaylistType Playlist::Type::get()
	{
		return PlaylistType::None;
	}

	CALLBACK_START_UU(PlaylistTracks_count, t_size, t_size)
		return static_api_ptr_t<playlist_manager>()->playlist_get_item_count(arg);
	CALLBACK_END()

	int Playlist::TrackCount::get()
	{
		if (m_tracks == nullptr)
		{
			if (m_trackCount == -1)
				m_trackCount = (new PlaylistTracks_count())->Run(this, m_index);
			return m_trackCount;
		}
		return m_tracks->Count;
	}

	int Playlist::Index::get()
	{
		return m_index;
	}

	bool Playlist::Equals(IPlaylist^ other)
	{
		if (ReferenceEquals(other, nullptr)) return false;
		if (ReferenceEquals(other, this)) return true;

		Playlist^ p = safe_cast<Playlist^>(other);
		if (p == nullptr) return false;

		//return p->m_index == m_index;
		return String::Equals(p->Name, Name, StringComparison::Ordinal);
	}

	String^ Playlist::ToString()
	{
		return m_name;
	}

	int Playlist::GetHashCode()
	{
		return m_name->GetHashCode();
	}

	bool Playlist::Equals(System::Object ^other)
	{
		return Equals(safe_cast<IPlaylist^>(other));
	}

	void Playlist::Invalidate()
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


	CALLBACK_START_UM(Play_impl, int, ITrack^)
		static_api_ptr_t<playback_control> mgr;
		static_api_ptr_t<playlist_manager> pl;

		Playlist^ p = static_cast<Playlist^>((Object^)m_this);

		pl->set_active_playlist(p->Index);
		pl->set_playing_playlist(p->Index);

		Track^ t = safe_cast<Track^>(arg);
		if (t != nullptr)
		{
			pl->playlist_set_focus_by_handle(p->Index, t->GetHandle());
			pl->playlist_set_selection(p->Index, bit_array_true(), bit_array_one(pl->playlist_get_focus_item(p->Index)));

			mgr->start(playback_control::track_command_settrack);
		}
		else
		{
			mgr->start(playback_control::track_command_rand);
		}

		return 0;
	CALLBACK_END()

	void Playlist::PlayRandom()
	{
		(new Play_impl())->Run(this);
	}

	void Playlist::Play(int index)
	{
		if (index < 0 || index >= m_tracks->Count)
		{
			PlayRandom();
			return;
		}

		(new Play_impl())->Run(this, m_tracks[index]);
	}

	void Playlist::Delete()
	{
		ManagedHost::Instance->DeletePlaylist(this); 
	}

	CALLBACK_START_UU(Playlist_clear, int, int)
		static_api_ptr_t<playlist_manager> mgr;

		Playlist^ pl = (Playlist^)(Object^)m_this;

		t_size index = mgr->find_playlist(ToUtf8String(pl->Name));
		if (index != pfc::infinite32)
		{
			mgr->playlist_clear(index);
		}

		return 0;
	CALLBACK_END()

	void Playlist::Clear()
	{
		(new Playlist_clear())->Run(this);
	}

	CALLBACK_START_UM(Playlist_add_tracks, int, IEnumerable<ITrack^>^)
		static_api_ptr_t<playlist_manager> mgr;

		Playlist^ pl = (Playlist^)(Object^)m_this;

		t_size index = mgr->find_playlist(ToUtf8String(pl->Name));
		if (index != pfc::infinite32)
		{
			metadb_handle_list items;

			for each (ITrack^ t in arg)
			{
				Track^ track = safe_cast<Track^>(t);
				if (track == nullptr) continue;

				items.add_item(track->GetHandle());
			}

			mgr->playlist_add_items(index, items, bit_array_false());
		}

		return 0;
	CALLBACK_END()

	void Playlist::AddTrack(ITrack ^track)
	{
		if (track == nullptr)
			throw gcnew ArgumentNullException("track");

		AddTrackRange(gcnew array<ITrack^> { track });
	}

	void Playlist::AddTrackRange(IEnumerable<ITrack^> ^tracks)
	{
		if (tracks == nullptr)
			throw gcnew ArgumentNullException("tracks");

		(new Playlist_add_tracks())->Run(this, tracks);
	}

	CALLBACK_START_UM(Playlist_del_tracks, int, IEnumerable<ITrack^>^)
		static_api_ptr_t<playlist_manager> mgr;

		Playlist^ pl = (Playlist^)(Object^)m_this;

		t_size index = mgr->find_playlist(ToUtf8String(pl->Name));
		if (index != pfc::infinite32)
		{
			IList<ITrack^>^ list = safe_cast<IList<ITrack^>^>(pl->Tracks);
			if (list != nullptr)
			{			
				bit_array_bittable mask;
				mask.resize(pl->TrackCount);

				for (int i = 0; i < list->Count; i++)
				{
					mask.set(i, System::Linq::Enumerable::Contains(arg, list[i]));
				}

				mgr->playlist_remove_items(index, mask);
			}
		}

		return 0;
	CALLBACK_END()

	void Playlist::RemoveTrack(ITrack ^track)
	{
		if (track == nullptr)
			throw gcnew ArgumentNullException("track");

		RemoveTrackRange(gcnew array<ITrack^> { track });
	}

	void Playlist::RemoveTrackRange(IEnumerable<ITrack^> ^tracks)
	{
		if (tracks == nullptr)
			throw gcnew ArgumentNullException("tracks");

		(new Playlist_del_tracks())->Run(this, tracks);
	}

	CALLBACK_START_UU(Playlist_move_track, int, int*)
		static_api_ptr_t<playlist_manager> mgr;

		int trackToMove = arg[0];
		int moveAfter = arg[1];

		Playlist^ pl = (Playlist^)(Object^)m_this;

		t_size index = mgr->find_playlist(ToUtf8String(pl->Name));
		if (index != pfc::infinite32)
		{
			t_size start = min(trackToMove, moveAfter + 1);
			t_size end = max(trackToMove, moveAfter);
			int delta = (trackToMove > moveAfter) ? 1 : -1;
			t_size count = mgr->playlist_get_item_count(index);

			if (start < count && end < count) 
			{
				order_helper move(count);
				if (delta > 0)
				{
					move[start] = end;
					for (t_size i = start; i < end; i++)
						move[i + 1] = i;
				}
				else
				{
					move[end] = start;
					for (t_size i = end; i > start; i--)
						move[i - 1] = i;
				}

				mgr->playlist_reorder_items(index, move.get_ptr(), move.get_count());
			}
		}

		return 0;
	CALLBACK_END()

	void Playlist::ReorderTracks(int trackToMove, int moveAfter)
	{
		if (trackToMove < 0 || trackToMove >= TrackCount)
			throw gcnew ArgumentOutOfRangeException("trackToMove");
		if (moveAfter >= 0 && moveAfter >= TrackCount)
			throw gcnew ArgumentOutOfRangeException("moveAfter");
		
		// nothing to move
		if (trackToMove == moveAfter) return;

		int pair[] = { trackToMove, moveAfter };

		(new Playlist_move_track())->Run(this, pair);
	}
}