#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote
{

	private ref class IDProvider
	{

	public:
		IDProvider(IMediaLibrary^ library);

		virtual int GetId(IPlaylist^ playlist);
		virtual int GetId(ITrack^ track);
		virtual int GetId(IAlbum^ album);
		virtual int GetId(IArtist^ artist);

		virtual __int64 GetPersistentId(IPlaylist^ playlist);
		virtual __int64 GetPersistentId(ITrack^ track);
		virtual __int64 GetPersistentId(IAlbum^ album);
		virtual __int64 GetPersistentId(IArtist^ artist);

	private:
		IMediaLibrary^ m_library;

		int m_id;
		Dictionary<ITrack^, int>^ m_tracks;
		Dictionary<IAlbum^, int>^ m_albums;
		Dictionary<IArtist^, int>^ m_artists;
		Dictionary<IPlaylist^, int>^ m_playlists;
	};

}