#include "stdafx.h"
#include "ManagedHost.h"
#include "ConsoleLogger.h"
#include "PreferencesWrapper.h"
#include "Library.h"
#include "Playlist.h"
#include "Track.h"
#include "Utils.h"
#include "MainThreadCallback.h"
#include "AutoPlaylistClient.h"
#include "Settings.h"
#include "PlaylistLock.h"
#include "TrackPool.h"
#include "PlaylistPool.h"

#pragma managed

using namespace TouchRemote;
using namespace System::Collections;
using namespace System::Collections::Specialized;

//#define USE_AUTOPLAYLIST 

extern advconfig_string_factory _AdvConfig_HostName;

namespace foo_touchremote
{

#ifndef USE_AUTOPLAYLIST
	static foobar::PlaylistLock _PlaylistLock;
#endif

    Core::Http::HttpResponse^ HttpHandler(Core::Http::HttpRequest^ request)
    {
		if (settings::HttpLogging)
		{
			_console::printf("HTTP {0}: {1}", System::Threading::Thread::CurrentThread->ManagedThreadId, request->Path);

			for (int i = 0; i < request->QueryString->Count; i++)
			{
				_console::printf("  {0}: {1}", request->QueryString->GetKey(i), String::Join("; ", request->QueryString->GetValues(i)));
			}
		}

        try
        {
			Core::Dacp::Responders::IResponder^ responder = Core::Dacp::PathMapper::Map(request);
            if (responder != nullptr)
            {
				if (!responder->IsValid)
					return gcnew Core::Http::Response::NotFoundResponse();

				Core::Http::HttpResponse^ response = responder->GetResponse();
				if (response != nullptr)
					return response;
            }
        }
        catch (Exception^ ex)
        {
			_console::error(ex->ToString());
			return gcnew Core::Http::Response::ServerErrorResponse(ex);
        }

		if (settings::HttpLogging)
		{
			//request->Dump();
			_console::printf("HTTP {0}: Request is not served: {1}", System::Threading::Thread::CurrentThread->ManagedThreadId, request->Path);
		}

		return gcnew Core::Http::Response::NotFoundResponse();
    }

	static ManagedHost::ManagedHost()
	{
		m_instance = gcnew ManagedHost();
	}

	ManagedHost::ManagedHost()
	{
		m_databaseId = nullptr;
		m_name = L"foobar2000";
		m_version = gcnew System::Version(1, 1, 0, 0);
		m_logger = gcnew ConsoleLogger();
		m_preferences = gcnew PreferencesWrapper();
		m_mediaLibrary = gcnew Library();
		m_trackPool = gcnew TrackPool(m_mediaLibrary);
		m_playlistPool = gcnew PlaylistPool(m_mediaLibrary);
		m_currentTrack = nullptr;

		m_dacpServer = nullptr;
		m_dnsServer = nullptr;

		m_syncObject = gcnew System::Threading::ReaderWriterLockSlim(System::Threading::LockRecursionPolicy::NoRecursion);

		m_currentPlaylist = nullptr;
		m_currentPlaylistValid = false;
		m_currentPlaylistSync = gcnew Object();
		m_initialized = false;
	}

	ManagedHost^ ManagedHost::Instance::get()
	{
		return m_instance;
	}

	IDisposable^ ManagedHost::BeginRead()
	{
		return gcnew TouchRemote::Core::Misc::ReadWriteLock(m_syncObject, false);
	}

	IDisposable^ ManagedHost::BeginWrite()
	{
		return gcnew TouchRemote::Core::Misc::ReadWriteLock(m_syncObject, true);
	}

	UInt64 ManagedHost::DatabaseId::get()
	{
		if (settings::DatabaseId == 0)
		{
			array<unsigned char>^ c = gcnew array<unsigned char>(8);

			System::Security::Cryptography::RNGCryptoServiceProvider::Create()->GetNonZeroBytes(c);

			settings::DatabaseId = BitConverter::ToUInt64(c, 0);
		}

		return (UInt64)settings::DatabaseId;
	}

	void ManagedHost::OnStart()
	{
		Logger->LogMessage("TouchRemote initialization started");	

		String^ strDatabaseId = DatabaseId.ToString("X16");

		_console::printf("TouchRemote database id: {0}", strDatabaseId);

		String^ displayName = FromUtf8String(settings::DisplayName);

		String^ hostName = nullptr;
		{
			pfc::string8 tmp;
			_AdvConfig_HostName.get(tmp);
			if (tmp.get_length() > 0)
				hostName = FromUtf8String(tmp);
		}

		t_uint32 portNumber = settings::Port;

		if (String::IsNullOrEmpty(displayName) || portNumber > 65535)
		{
			Logger->LogMessage("TouchRemote is not configured properly. Initialization aborted");
			return;
		}

		{
			static_api_ptr_t<playlist_manager> pm;
#ifndef USE_AUTOPLAYLIST
			t_uint32 playlist_index = pm->find_playlist(SELECTION_PLAYLIST_NAME);
			if (playlist_index != pfc::infinite32)
				pm->playlist_lock_install(playlist_index, &_PlaylistLock);
#endif
			SetCurrentVolume(static_api_ptr_t<playback_control>()->get_volume());
			SetCurrentPlaybackOrder(pm->playback_order_get_active());
		}

        m_name = displayName;

		NameValueCollection^ props = gcnew NameValueCollection();
		props["txtvers"] = "1";
		props["CtlN"] = displayName;
		props["DvNm"] = "foobar2000";
		props["DvTy"] = "TouchRemote";
		props["DvSv"] = "0206";
		props["DbId"] = strDatabaseId;
		props["iV"] = "196613";
		props["Ver" ] = "131074";

		m_dacpServer = gcnew Core::Dacp::DacpServer(this, portNumber);
		m_dacpServer->RequestHandler = gcnew Core::HandleRequestDelegate(&HttpHandler);
		m_dacpServer->Start();

		m_dnsServer = gcnew Bonjour::BonjourService();
		m_dnsServer->Start(strDatabaseId, "_touch-able._tcp", "local.", hostName, portNumber, props);
		//m_dnsServer->Start(strDatabaseId, "_daap._tcp", "local.", hostName, portNumber, props);
		//m_dnsServer->Start(strDatabaseId, "_remote-jukebox._tcp", "local.", hostName, portNumber, props);

		m_initialized = true;

		Logger->LogMessage("TouchRemote initialization finished");
	}

	void ManagedHost::OnQuit()
	{
		Logger->LogMessage("TouchRemote shutdown started");

		m_initialized = false;

		SetCurrentState(false, false);
		SetCurrentTrack(metadb_handle_ptr());
		SetCurrentPosition(0);

		if (m_dnsServer != nullptr)
			m_dnsServer->Stop();

		if (m_dacpServer != nullptr)
		{
			m_dacpServer->Stop();
			m_dacpServer->WaitForConnectionsClosed(TimeSpan::FromSeconds(1));
		}

		Logger->LogMessage("TouchRemote shutdown finished");		
	}

	String^ ManagedHost::Name::get()
	{
		return m_name;
	}

	Version^ ManagedHost::Version::get()
	{
		return m_version;
	}

	ILogger^ ManagedHost::Logger::get()
	{
		return m_logger;
	}

	IPreferences^ ManagedHost::Preferences::get()
	{
		return m_preferences;
	}

	IMediaLibrary^ ManagedHost::MediaLibrary::get()
	{
		return m_mediaLibrary;
	}


#pragma region Caches

	ITrack^ ManagedHost::GetTrack(metadb_handle_ptr &ptr)
	{
		return m_trackPool->GetTrack(ptr);
	}

	ITrack^ ManagedHost::GetUpdatedTrack(metadb_handle_ptr &ptr)
	{
		return m_trackPool->GetTrack(ptr, true);
	}

	void ManagedHost::LazyUpdateTrack(metadb_handle_ptr &ptr)
	{
		m_trackPool->GetTrack(ptr, true, true);
	}

	IPlaylist^ ManagedHost::GetPlaylist(t_size index)
	{
		return m_playlistPool[index];
	}

	void ManagedHost::InvalidateAllPlaylists()
	{
		if (!m_initialized) return;

		m_playlistPool->InvalidateAll();
	}

	void ManagedHost::InvalidatePlaylist(t_size index)
	{
		if (!m_initialized) return;

		Playlist^ pl = m_playlistPool[index];
		if (pl != nullptr)
			pl->Invalidate();
	}

	void ManagedHost::AddPlaylist(t_size index, System::String ^newName)
	{
		if (!m_initialized) return;

		m_playlistPool->InvalidateAll();
		Playlist^ pl = m_playlistPool[index];
		if (pl != nullptr)
			pl->Invalidate();
	}

	void ManagedHost::RenamePlaylist(t_size index, System::String ^newName)
	{
		if (!m_initialized) return;

		Playlist^ pl = m_playlistPool[index];
		if (pl != nullptr)
			pl->Name = newName;
	}

#pragma endregion

	/*CALLBACK_START_MU(Playlists_get, System::Collections::Generic::IEnumerable<IPlaylist^>^, int)
		static_api_ptr_t<playlist_manager> mgr;

		IMediaLibrary^ lib = ManagedHost::Instance->MediaLibrary;

		System::Collections::Generic::List<IPlaylist^>^ list = gcnew System::Collections::Generic::List<IPlaylist^>();

		t_size count = mgr->get_playlist_count();

		for (t_size i = 0; i < count; i++)
		{
			pfc::string8 name;

			Playlist^ pl;

			if (!mgr->playlist_get_name(i, name)) continue;
			
			if (strcmp(name, SELECTION_PLAYLIST_NAME) == 0) continue;

			pl = gcnew Playlist(lib, i, FromUtf8String(name));

			metadb_handle_list items;

			mgr->playlist_get_all_items(i, items);
			for(t_size j = 0; j < items.get_count(); j++)
				pl->AddTrack(items[j]);

			list->Add(pl);
		}

		return list;
	CALLBACK_END()*/

	System::Collections::Generic::IEnumerable<IPlaylist^>^ ManagedHost::Playlists::get()
	{
		//return (new Playlists_get())->Run(this);
		array<Playlist^>^ raw = m_playlistPool->Playlists;
		List<IPlaylist^>^ items = gcnew List<IPlaylist^>(raw->Length);
		for each (Playlist^ pl in raw)
		{
			if (pl->Name == SELECTION_PLAYLIST_NAME) continue;
			items->Add(pl);
		}
		return items;
	}

	RepeatMode ManagedHost::AvailableRepeatModes::get()
	{
		return RepeatMode::RepeatAll | RepeatMode::RepeatTrack;
	}

	CALLBACK_START_UU(ActivePlaybackOrder_set, int, t_size)
		static_api_ptr_t<playlist_manager>()->playback_order_set_active(arg);
		return 0;
	CALLBACK_END()

	RepeatMode ManagedHost::CurrentRepeatMode::get()
	{
		t_size currentIndex = m_currentPlaybackOrder;
		
		if (currentIndex == 0)
			return RepeatMode::None;
		
		if (currentIndex == 2)
			return RepeatMode::RepeatTrack;

		return RepeatMode::RepeatAll;
	}

	void ManagedHost::CurrentRepeatMode::set(RepeatMode value)
	{
		ShuffleMode shuffle = CurrentShuffleMode;

		t_size mode;

		switch (value)
		{
		case RepeatMode::None:
			mode = (shuffle == ShuffleMode::Shuffle) ? 4 : 0;
			break;
		case RepeatMode::RepeatAll:
			mode = (shuffle == ShuffleMode::Shuffle) ? 4 : 1;
			break;
		case RepeatMode::RepeatTrack:
			mode = (shuffle == ShuffleMode::Shuffle) ? 4 : 2;
			break;
		default:
			throw gcnew ArgumentOutOfRangeException("value");
		}

		(new ActivePlaybackOrder_set())->Run(this, mode);
	}

	ShuffleMode ManagedHost::AvailableShuffleModes::get()
	{
		return ShuffleMode::Shuffle;
	}

	ShuffleMode ManagedHost::CurrentShuffleMode::get()
	{
		t_size currentIndex = m_currentPlaybackOrder;
		
		if (currentIndex < 3)
			return ShuffleMode::None;
		
		return ShuffleMode::Shuffle;
	}

	void ManagedHost::CurrentShuffleMode::set(ShuffleMode value)
	{
		RepeatMode repeat = CurrentRepeatMode;

		t_size mode;

		switch (value)
		{
		case ShuffleMode::None:
			switch(repeat)
			{
			case RepeatMode::None:
				mode = 0;
				break;
			case RepeatMode::RepeatTrack:
				mode = 2;
				break;
			default:
				mode = 1;
				break;
			}
			break;
		case ShuffleMode::Shuffle:
			mode = 4;
			break;
		default:
			throw gcnew ArgumentOutOfRangeException("value");
		}

		(new ActivePlaybackOrder_set())->Run(this, mode);
	}
    
   	CALLBACK_START_UU(CurrentVolume_get, float, int)
		static_api_ptr_t<playback_control> mgr;
        return mgr->get_volume();;
	CALLBACK_END()

	int ManagedHost::CurrentVolume::get()
	{
		float vol = m_currentVolume;

        //float vol = (new CurrentVolume_get())->Run(this, 0);

		if (vol >= 0.0f) return 100;

		return (int)floor(pow(2.0f, vol / 10.0f) * 100.0f);
	}

	CALLBACK_START_UU(CurrentVolume_set, float, float)
		static_api_ptr_t<playback_control> mgr;
        mgr->set_volume(arg);
        return mgr->get_volume();;
	CALLBACK_END()

	void ManagedHost::CurrentVolume::set(int value)
	{
		float vol;

		if (value >= 100)
			vol = 0.0;
		else if (value <= 0)
			vol = -100.0;
		else
			vol = log(value / 100.0f) * 10.0f / log(2.0f);

		m_currentVolume = (new CurrentVolume_set())->Run(this, vol);
	}

	ITrack^ ManagedHost::CurrentTrack::get()
	{
		return m_currentTrack;
	}

	void ManagedHost::SetCurrentTrack(metadb_handle_ptr & track)
	{
        SetCurrentTrack(GetTrack(track));
	}

	void ManagedHost::SetCurrentTrack(ITrack^ track)
	{
        if (m_currentTrack != nullptr)
            ((Track^)m_currentTrack)->CancelDynamic();

		m_currentTrack = track;
	}

    void ManagedHost::SetCurrentTrackDynamic(const file_info & info)
    {
        if (m_currentTrack == nullptr) return;

        ((Track^)m_currentTrack)->SetDynamic(info);
    }

	void ManagedHost::SetCurrentPosition(double position)
	{
		m_currentPosition = position;
	}

	void ManagedHost::SetCurrentState(bool isPlaying, bool isPaused)
	{
		if (isPlaying)
			m_currentState = (isPaused) ? PlaybackState::Paused : PlaybackState::Playing;
		else
			m_currentState = PlaybackState::Stopped;
	}

	void ManagedHost::SetCurrentVolume(float volume)
	{
		m_currentVolume = volume;
	}

	void ManagedHost::SetCurrentPlaybackOrder(t_size order)
	{
		m_currentPlaybackOrder = order;
	}

	//CALLBACK_START(CurrentPlaybackState_get, PlaybackState, int)
	//	static_api_ptr_t<play_control> mgr;

	//	bool isPlaying = mgr->is_playing();
	//	bool isPaused = mgr->is_paused();

	//	if (isPlaying)
	//		return (isPaused) ? PlaybackState::Paused : PlaybackState::Playing;
	//	else
	//		return PlaybackState::Stopped;
	//CALLBACK_END()

	PlaybackState ManagedHost::CurrentState::get()
	{
		return m_currentState;
	}

	TimeSpan ManagedHost::CurrentPosition::get()
	{
		return TimeSpan::FromSeconds(m_currentPosition);
	}

	CALLBACK_START_UU(CurrentPosition_set, bool, double)
		static_api_ptr_t<playback_control_v2> mgr;

		if (mgr->playback_can_seek())
		{
			mgr->playback_seek(arg);
			return true;
		}
		else
			return false;
	CALLBACK_END()

	void ManagedHost::CurrentPosition::set(TimeSpan value)
	{
		bool r = (new CurrentPosition_set())->Run(this, value.TotalSeconds);
		if (r)
			m_currentPosition = value.TotalSeconds;
	}

	CALLBACK_START_UU(ActivePlaylist_get, t_size, int)
		return static_api_ptr_t<playlist_manager>()->get_playing_playlist();
	CALLBACK_END()

	IPlaylist^ ManagedHost::ActivePlaylist::get()
	{
		System::Threading::Monitor::Enter(m_currentPlaylistSync);
		try
		{
			if (m_currentPlaylistValid)
				return m_currentPlaylist;
		}
		finally
		{
			System::Threading::Monitor::Exit(m_currentPlaylistSync);
		}

		t_size index = (new ActivePlaylist_get())->Run(this);
		
		IPlaylist^ pl = m_playlistPool[index];
		if (pl != nullptr && pl->Name == SELECTION_PLAYLIST_NAME)
			pl = nullptr;
		
		System::Threading::Monitor::Enter(m_currentPlaylistSync);
		m_currentPlaylist = pl;
		m_currentPlaylistValid = true;
		System::Threading::Monitor::Exit(m_currentPlaylistSync);

		return pl;
	}

	CALLBACK_START_UM(PlaybackSource_set, int, cli::array<ITrack^>^)
		static_api_ptr_t<playlist_manager> mgr;

		t_size index = mgr->find_or_create_playlist(SELECTION_PLAYLIST_NAME);
		if (index != pfc::infinite32)
		{
#ifdef USE_AUTOPLAYLIST
			static_api_ptr_t<autoplaylist_manager_v2> apl;
			if (apl->is_client_present(index))
				apl->remove_client(index);
			apl->add_client(new foo_touchremote::foobar::AutoPlaylistClient(arg), index, autoplaylist_flag_sort);
#else
			mgr->playlist_lock_uninstall(index, &_PlaylistLock);

			mgr->playlist_clear(index);
			if (arg != nullptr)
			{
				metadb_handle_list items;
				items.set_size(arg->Length);

				for (int i = 0; i < arg->Length; i++)
					items[i] = ((Track^)arg[i])->GetHandle();

				mgr->playlist_add_items(index, items, bit_array_false());
			}

			mgr->playlist_lock_install(index, &_PlaylistLock);
#endif
		}

		return 0;
	CALLBACK_END()

	void ManagedHost::ClearPlaybackSource()
	{
		(new PlaybackSource_set())->Run(this);
	}

	void ManagedHost::SetPlaybackSource(cli::array<ITrack^>^ tracks)
	{
		if (tracks == nullptr)
			throw gcnew ArgumentNullException("tracks");

		m_sourceTracks = tracks;
		(new PlaybackSource_set())->Run(this, tracks);
	}

	array<ITrack^>^ ManagedHost::GetPlaybackSource()
	{
		return m_sourceTracks;
	}

	CALLBACK_START_MU(PlaybackSource_play, ITrack^, t_size)
		static_api_ptr_t<playlist_manager> mgr;
		static_api_ptr_t<playback_control> pbm;

		t_size index = mgr->find_playlist(SELECTION_PLAYLIST_NAME);
		if (index == pfc::infinite32)
			return nullptr;

		mgr->set_playing_playlist(index);
		mgr->set_active_playlist(index);

		if (arg == pfc::infinite32)
		{
			pbm->start(play_control::track_command_rand);
		}
		else
		{
			mgr->playlist_set_focus_item(index, arg);
			mgr->playlist_set_selection(index, bit_array_true(), bit_array_one(arg));

			pbm->start(play_control::track_command_settrack);
		}

		metadb_handle_ptr ptr;

		if (pbm->get_now_playing(ptr))
			return ManagedHost::Instance->GetTrack(ptr);

		return nullptr;
	CALLBACK_END()

	ITrack^ ManagedHost::Play(int index)
	{
		if (index == -1)
			CurrentShuffleMode = ShuffleMode::Shuffle;
		return (new PlaybackSource_play())->Run(this, (t_size)index);
	}

	CALLBACK_START_UU(PlayControl_play, int, int)
		static_api_ptr_t<play_control> mgr;

		switch (arg)
		{
			case 1:
				mgr->play_or_pause();

                if (settings::StopOnPause && mgr->is_paused())
                    mgr->stop();
				break;
			case 2:
				mgr->start(play_control::track_command_next);
				break;
			case 3:
				mgr->start(play_control::track_command_prev);
				break;
		}

		return 0;
	CALLBACK_END()

	void ManagedHost::PlayPause()
	{
		(new PlayControl_play())->Run(this, 1);
	}

	void ManagedHost::PlayNext()
	{
		(new PlayControl_play())->Run(this, 2);
	}

	void ManagedHost::PlayPrevious()
	{
		(new PlayControl_play())->Run(this, 3);
	}

	void ManagedHost::SetCurrentPlaylistInvalid()
	{
		System::Threading::Monitor::Enter(m_currentPlaylistSync);
		m_currentPlaylistValid = false;
		m_currentPlaylist = nullptr;
		System::Threading::Monitor::Exit(m_currentPlaylistSync);
	}

	CALLBACK_START_MM(Playlist_create, IPlaylist^, String^)
		static_api_ptr_t<playlist_manager> mgr;

		IMediaLibrary^ lib = ManagedHost::Instance->MediaLibrary;

		IPlaylist^ pl = nullptr;

		t_size index = mgr->find_or_create_playlist(ToUtf8String(arg));
		if (index != pfc::infinite32)
		{
			pl = ManagedHost::Instance->GetPlaylist(index);
			if (pl != nullptr && pl->Name == SELECTION_PLAYLIST_NAME)
				pl = nullptr;

			/*pfc::string8 name;
			if (mgr->playlist_get_name(index, name) && strcmp(name, SELECTION_PLAYLIST_NAME) != 0)
			{
				pl = gcnew Playlist(lib, index, FromUtf8String(name));
			
				metadb_handle_list items;

				mgr->playlist_get_all_items(index, items);
				for(t_size i = 0; i < items.get_count(); i++)
					pl->AddTrack(items[i]);
			}*/
		}

		return pl;
	CALLBACK_END()

	IPlaylist^ ManagedHost::CreatePlaylist(System::String ^name)
	{
		if (String::IsNullOrEmpty(name))
			throw gcnew ArgumentNullException("name");

		return (new Playlist_create())->Run(this, name);
	}

	CALLBACK_START_UM(Playlist_delete, int, String^)
		static_api_ptr_t<playlist_manager> mgr;

		t_size index = mgr->find_playlist(ToUtf8String(arg));
		if (index != pfc::infinite32)
		{
			mgr->remove_playlist_switch(index);
		}

		return 0;
	CALLBACK_END()

	void ManagedHost::DeletePlaylist(IPlaylist^ playlist)
	{
		if (playlist == nullptr)
			throw gcnew ArgumentNullException("playlist");

		(new Playlist_delete())->Run(this, playlist->Name);
	}

}