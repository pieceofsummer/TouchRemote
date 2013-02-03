#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;
using namespace TouchRemote::Core::Library;
using namespace System::Collections::Generic;

namespace foo_touchremote
{

	ref class IDProvider;

	public ref class Library : public IMediaLibrary, public IPropertyExtender
	{

	public:
		Library();

		virtual property int Id
		{
			int get();
		}

		virtual property __int64 PersistentId
		{
			__int64 get();
		}

		virtual void Extend(System::Collections::Generic::IDictionary<String^, Object^>^ data);

		virtual property System::Collections::Generic::IEnumerable<ITrack^>^ Tracks
		{
			System::Collections::Generic::IEnumerable<ITrack^>^ get();
		}

		virtual property System::Collections::Generic::IEnumerable<IAlbum^>^ Albums
		{
			System::Collections::Generic::IEnumerable<IAlbum^>^ get();
		}

		virtual property System::Collections::Generic::IEnumerable<IArtist^>^ Artists
		{
			System::Collections::Generic::IEnumerable<IArtist^>^ get();
		}

		virtual property String^ Name
		{
			String^ get();
			void set(String^ value);
		}

		virtual property PlaylistType Type
		{
			PlaylistType get();
		}

		virtual property int TrackCount
		{
			int get();
		}

		virtual property IPlaylist^ Music
		{
			IPlaylist^ get();
		}

		virtual property IPlaylist^ Podcasts
		{
			IPlaylist^ get();
		}

		virtual property IPlaylist^ Videos
		{
			IPlaylist^ get();
		}

		virtual property IPlaylist^ AudioBooks
		{
			IPlaylist^ get();
		}

		virtual property IPlaylist^ Jukebox
		{
			IPlaylist^ get();
		}

		virtual bool Equals(IPlaylist ^other);
		virtual String^ ToString() override;
		virtual bool Equals(Object^ other) override;
		virtual int GetHashCode() override;

		virtual IDisposable^ BeginRead();
		virtual IDisposable^ BeginWrite();

	internal:

		property IDProvider^ Identifiers
		{
			IDProvider^ get();
		}

		void AddTrack(metadb_handle_ptr &handle);
		void RemoveTrack(metadb_handle_ptr &handle);

		ITrack^ GetTrackCore(IPlaybackSource^ key);
		void RemoveTrackCore(IPlaybackSource^ key);

		void RegisterAlbumAndArtist(String^ artistName, String^ albumName, IArtist^ %artist, IAlbum^ %album);

	private:
		ref class AAEntry
		{
		public:
			AAEntry(IArtist^ artist);

			property IArtist^ Artist
			{
				IArtist^ get();
			}

			property Dictionary<String^, IAlbum^>^ Albums
			{
				Dictionary<String^, IAlbum^>^ get();
			}

		private:
			IArtist^ m_artist;
			Dictionary<String^, IAlbum^>^ m_albums;
		};

		System::Threading::ReaderWriterLockSlim^ m_lockObject;

		Dictionary<IPlaybackSource^, ITrack^>^ m_tracks;
		Dictionary<String^, AAEntry^>^ m_artists;

		IPlaylist^ m_musicPlaylist;
		IPlaylist^ m_moviesPlaylist;
		IPlaylist^ m_tvShowsPlaylist;
		IPlaylist^ m_jukeboxPlaylist;

		IDProvider^ m_idProvider;

	};

}