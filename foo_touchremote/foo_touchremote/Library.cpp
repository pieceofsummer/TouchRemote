#include "stdafx.h"
#include "Library.h"
#include "FilePlaybackSource.h"
#include "Track.h"
#include "JukeboxPlaylist.h"
#include "ManagedHost.h"
#include "IDProvider.h"
#include "Album.h"
#include "Artist.h"
#include "Utils.h"

#pragma managed

namespace foo_touchremote
{

	Library::AAEntry::AAEntry(IArtist^ artist)
	{
		m_artist = artist;
		m_albums = gcnew Dictionary<String^, IAlbum^>(StringComparer::InvariantCultureIgnoreCase);
	}

	IArtist^ Library::AAEntry::Artist::get()
	{
		return m_artist;
	}

	Dictionary<String^, IAlbum^>^ Library::AAEntry::Albums::get()
	{
		return m_albums;
	}


	Library::Library()
	{
		m_tracks = gcnew Dictionary<IPlaybackSource^, ITrack^>();
		m_artists = gcnew Dictionary<String^, AAEntry^>(StringComparer::InvariantCultureIgnoreCase);

		m_musicPlaylist = gcnew MusicPlaylist(this, "Music");
		m_moviesPlaylist = gcnew MoviesPlaylist(this, "Movies");
		m_jukeboxPlaylist = nullptr; // gcnew JukeboxPlaylist(this, "Foobar DJ");
		
		m_idProvider = gcnew IDProvider(this);
		m_lockObject = gcnew System::Threading::ReaderWriterLockSlim(System::Threading::LockRecursionPolicy::NoRecursion);
	}

	IDisposable^ Library::BeginRead()
	{
		return gcnew TouchRemote::Core::Misc::ReadWriteLock(m_lockObject, false);
	}

	IDisposable^ Library::BeginWrite()
	{
		return gcnew TouchRemote::Core::Misc::ReadWriteLock(m_lockObject, true);
	}

	int Library::Id::get()
	{
		return 100;
	}

	__int64 Library::PersistentId::get()
	{
		return (__int64)ManagedHost::Instance->DatabaseId;
	}

	void Library::Extend(System::Collections::Generic::IDictionary<String^, Object^>^ data)
	{
		data["abpl"] = true;
	}
	
	System::Collections::Generic::IEnumerable<ITrack^>^ Library::Tracks::get()
	{
		return m_tracks->Values;
	}

	System::Collections::Generic::IEnumerable<IAlbum^>^ Library::Albums::get()
	{
		return nullptr;
	}

	System::Collections::Generic::IEnumerable<IArtist^>^ Library::Artists::get()
	{
		return nullptr;
	}

	String^ Library::Name::get()
	{
		return "_MediaLibrary_";
	}

	void Library::Name::set(String^ value)
	{
		// 
	}

	PlaylistType Library::Type::get()
	{
		return PlaylistType::None;
	}

	int Library::TrackCount::get()
	{
		return m_tracks->Count;
	}

	IPlaylist^ Library::Music::get()
	{
		return m_musicPlaylist;
	}

	IPlaylist^ Library::Videos::get()
	{
		return m_moviesPlaylist;
	}

	IPlaylist^ Library::Podcasts::get()
	{
		return nullptr;
	}

	IPlaylist^ Library::AudioBooks::get()
	{
		return nullptr;
	}

	IPlaylist^ Library::Jukebox::get()
	{
		return m_jukeboxPlaylist;
	}

	void Library::AddTrack(metadb_handle_ptr &handle)
	{
		if (handle.is_empty()) return;

		try	
		{
			ITrack^ track = ManagedHost::Instance->GetUpdatedTrack(handle);

			ITrack^ oldOne;
			if (!m_tracks->TryGetValue(track->Source, oldOne))
				oldOne = nullptr;

			if (!ReferenceEquals(track, oldOne))
			{
				m_tracks[track->Source] = track;

				if (!ReferenceEquals(oldOne, nullptr))
				{
					if (ManagedHost::Instance->CurrentTrack == oldOne)
					{
						ManagedHost::Instance->SetCurrentTrack(track);
					}
				}
			}
		}
		catch (Exception^ ex)
		{
			_console::error(ex->ToString());
		}
	}

	void Library::RemoveTrack(metadb_handle_ptr &handle)
	{
		if (handle.is_empty()) return;	

		IPlaybackSource^ key = gcnew FilePlaybackSource(handle->get_location());

		ITrack^ track;
		if (m_tracks->TryGetValue(key, track))
		{
			m_tracks->Remove(key);
			//delete track;
		}
	}

	ITrack^ Library::GetTrackCore(IPlaybackSource^ key)
	{
		if (key == nullptr) return nullptr;

		ITrack^ track;
		if (m_tracks->TryGetValue(key, track))
			return track;

		return nullptr;
	}

	void Library::RemoveTrackCore(IPlaybackSource^ key)
	{
		if (key == nullptr) return;

		IDisposable^ lock = BeginWrite();
		try
		{
			m_tracks->Remove(key);
		}
		finally
		{
			delete lock;
		}
	}

	IDProvider^ Library::Identifiers::get()
	{
		return m_idProvider;
	}

	void Library::RegisterAlbumAndArtist(String^ artistName, String^ albumName, IArtist^ %artist, IAlbum^ %album)
	{
		artist = nullptr;
		album = nullptr;

		if (String::IsNullOrEmpty(artistName)) return;

		AAEntry^ t_artist;
		if (!m_artists->TryGetValue(artistName, t_artist))
		{
			t_artist = gcnew AAEntry(gcnew Artist(this, artistName));

			m_artists[artistName] = t_artist;
		}

		artist = t_artist->Artist;

		if (String::IsNullOrEmpty(albumName)) return;

		IAlbum^ t_album;
		if (!t_artist->Albums->TryGetValue(albumName, t_album))
		{
			t_album = gcnew Album(this, t_artist->Artist, albumName);
			
			t_artist->Albums[albumName] = t_album;
		}

		album = t_album;
	}

	bool Library::Equals(IPlaylist^ other)
	{
		if (ReferenceEquals(other, nullptr)) return false;
		if (ReferenceEquals(other, this)) return true;

		return false;
	}

	String^ Library::ToString()
	{
		return Name;
	}

	int Library::GetHashCode()
	{
		return Id;
	}

	bool Library::Equals(System::Object ^other)
	{
		return Equals(safe_cast<IPlaylist^>(other));
	}

}