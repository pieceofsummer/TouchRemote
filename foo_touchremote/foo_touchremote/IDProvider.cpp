#include "stdafx.h"
#include "IDProvider.h"

#pragma managed

using namespace System::Threading;

namespace foo_touchremote
{

	IDProvider::IDProvider(IMediaLibrary^ library)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");

		m_library = library;
		m_tracks = gcnew Dictionary<ITrack^, int>();
		m_albums = gcnew Dictionary<IAlbum^, int>();
		m_artists = gcnew Dictionary<IArtist^, int>();
		m_playlists = gcnew Dictionary<IPlaylist^, int>();

		m_id = 1000;
	}

	int IDProvider::GetId(IPlaylist^ playlist)
	{
		if (playlist == nullptr) return 0;

		int id;

		Monitor::Enter(m_playlists);
		try
		{
			if (!m_playlists->TryGetValue(playlist, id))
			{
				id = Interlocked::Increment(m_id);
				m_playlists[playlist] = id;
			}
		}
		finally
		{
			Monitor::Exit(m_playlists);
		}

		return id;
	}

	int IDProvider::GetId(ITrack^ track)
	{
		if (track == nullptr) return 0;

		int id;

		Monitor::Enter(m_tracks);
		try
		{
			if (!m_tracks->TryGetValue(track, id))
			{
				id = Interlocked::Increment(m_id);
				m_tracks[track] = id;
			}
		}
		finally
		{
			Monitor::Exit(m_tracks);
		}

		return id;
	}

	int IDProvider::GetId(IAlbum^ album)
	{
		if (album == nullptr) return 0;

		int id;

		Monitor::Enter(m_albums);
		try
		{
			if (!m_albums->TryGetValue(album, id))
			{
				id = Interlocked::Increment(m_id);
				m_albums[album] = id;
			}
		}
		finally
		{
			Monitor::Exit(m_albums);
		}

		return id;
	}

	
	int IDProvider::GetId(IArtist^ artist)
	{
		if (artist == nullptr) return 0;

		int id;

		Monitor::Enter(m_artists);
		try
		{
			if (!m_artists->TryGetValue(artist, id))
			{
				id = Interlocked::Increment(m_id);
				m_artists[artist] = id;
			}
		}
		finally
		{
			Monitor::Exit(m_artists);
		}

		return id;
	}

	__int64 IDProvider::GetPersistentId(IPlaylist^ playlist)
	{
		return GetId(playlist) + 0x100000000;
	}

	__int64 IDProvider::GetPersistentId(ITrack^ track)
	{
		return GetId(track) + 0x100000000;
	}

	__int64 IDProvider::GetPersistentId(IAlbum^ album)
	{
		return GetId(album) + 0x100000000;
	}

	__int64 IDProvider::GetPersistentId(IArtist^ artist)
	{
		return GetId(artist) + 0x100000000;
	}
}