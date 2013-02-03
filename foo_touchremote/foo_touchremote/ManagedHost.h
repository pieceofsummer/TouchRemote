#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;

#pragma managed

namespace foo_touchremote
{
	// forward declaration
	ref class TrackPool;
	ref class PlaylistPool;

	public ref class ManagedHost : public IPlayer
	{
	private:
		static ManagedHost();
		ManagedHost();
		
	public:

		static property ManagedHost^ Instance
		{
			ManagedHost^ get();
		}

		void OnStart();
		void OnQuit();

		virtual property String^ Name 
		{ 
			String^ get(); 
		}

		virtual property System::Version^ Version 
		{ 
			System::Version^ get(); 
		}

		virtual property ILogger^ Logger 
		{ 
			ILogger^ get(); 
		}

		virtual property IPreferences^ Preferences
		{
			IPreferences^ get();
		}

		virtual property IMediaLibrary^ MediaLibrary
		{
			IMediaLibrary^ get();
		}

		virtual property System::Collections::Generic::IEnumerable<IPlaylist^>^ Playlists
		{
			System::Collections::Generic::IEnumerable<IPlaylist^>^ get();
		}

		virtual property RepeatMode AvailableRepeatModes
		{
			RepeatMode get();
		}

		virtual property RepeatMode CurrentRepeatMode
		{
			RepeatMode get();
			void set(RepeatMode value);
		}

		virtual property ShuffleMode AvailableShuffleModes
		{
			ShuffleMode get();
		}

		virtual property ShuffleMode CurrentShuffleMode
		{
			ShuffleMode get();
			void set(ShuffleMode value);
		}

		virtual property int CurrentVolume
		{
			int get();
			void set(int value);
		}

		virtual property PlaybackState CurrentState
		{
			PlaybackState get();
		}

		virtual property TimeSpan CurrentPosition
		{
			TimeSpan get();
			void set(TimeSpan value);
		}

		virtual property ITrack^ CurrentTrack
		{
			ITrack^ get();
		}

		virtual property IPlaylist^ ActivePlaylist
		{
			IPlaylist^ get();
		}

		virtual property UInt64 DatabaseId
		{
			UInt64 get();
		}

		virtual void ClearPlaybackSource();
		virtual void SetPlaybackSource(cli::array<ITrack^>^ tracks);
		virtual cli::array<ITrack^>^ GetPlaybackSource();
		virtual ITrack^ Play(int index);
		virtual void PlayPause();
		virtual void PlayNext();
		virtual void PlayPrevious();

		virtual IDisposable^ BeginRead();
		virtual IDisposable^ BeginWrite();

		virtual IPlaylist^ CreatePlaylist(String^ name);

	internal:


		void DeletePlaylist(IPlaylist^ playlist);

		void SetCurrentPlaybackOrder(t_size order);
		void SetCurrentVolume(float volume);
		void SetCurrentTrack(metadb_handle_ptr & track);
		void SetCurrentTrack(ITrack^ track);
		void SetCurrentPosition(double position);
		void SetCurrentState(bool isPlaying, bool isPaused);
		void SetCurrentPlaylistInvalid();

		ITrack^ GetTrack(metadb_handle_ptr &ptr);
		ITrack^ GetUpdatedTrack(metadb_handle_ptr &ptr);
		void LazyUpdateTrack(metadb_handle_ptr &ptr);

		IPlaylist^ GetPlaylist(t_size index);
		void InvalidateAllPlaylists();
		void InvalidatePlaylist(t_size index);
		void RenamePlaylist(t_size index, String^ newName);
		void AddPlaylist(t_size index, String^ newName);
		
	private:
		static ManagedHost ^m_instance;
		bool m_initialized;

		String^ m_name;
		String^ m_databaseId;
		System::Version^ m_version;
		ILogger^ m_logger;
		IPreferences^ m_preferences;
		IMediaLibrary^ m_mediaLibrary;
		TrackPool^ m_trackPool;
		PlaylistPool^ m_playlistPool;
		
		volatile t_size m_currentPlaybackOrder;
		volatile float m_currentVolume;
		ITrack^ m_currentTrack;
		volatile double m_currentPosition;
		volatile PlaybackState m_currentState;
		array<ITrack^>^ m_sourceTracks;

		IPlaylist^ m_currentPlaylist;
		bool m_currentPlaylistValid;
		Object^ m_currentPlaylistSync;

		TouchRemote::Core::Dacp::DacpServer^ m_dacpServer;
		TouchRemote::Bonjour::BonjourService^ m_dnsServer;

		System::Threading::ReaderWriterLockSlim^ m_syncObject;
	};

}