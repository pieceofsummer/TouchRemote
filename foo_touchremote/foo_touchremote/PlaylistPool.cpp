#include "stdafx.h"
#include "PlaylistPool.h"
#include "Playlist.h"
#include "Track.h"
#include "Utils.h"
#include "MainThreadCallback.h"

#pragma managed

namespace foo_touchremote
{

	PlaylistPool::PlaylistPool(IMediaLibrary^ library)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");

		m_library = library;
		m_playlists = nullptr;
		m_lock = gcnew ReaderWriterLockSlim(LockRecursionPolicy::NoRecursion);
	}

	Playlist^ PlaylistPool::default::get(t_size index)
	{
		if (index == pfc::infinite32) return nullptr;

		m_lock->EnterUpgradeableReadLock();
		try
		{
			ValidateAll();

			if (index >= m_playlists->Length)
				return nullptr;

			return m_playlists[index];
		}
		finally
		{
			m_lock->ExitUpgradeableReadLock();
		}
	}

	array<Playlist^>^ PlaylistPool::Playlists::get()
	{
		m_lock->EnterUpgradeableReadLock();
		try
		{
			ValidateAll();
			return m_playlists;
		}
		finally
		{
			m_lock->ExitUpgradeableReadLock();
		}
	}

	void PlaylistPool::InvalidateAll()
	{
		m_lock->EnterWriteLock();
		try
		{
			if (m_playlists != nullptr)
				for each (Playlist^ p in m_playlists)
					p->Invalidate();
			m_playlists = nullptr;
		}
		finally
		{
			m_lock->ExitWriteLock();
		}
	}

	CALLBACK_START_MM(Playlists_enum, array<Playlist^>^, IMediaLibrary^)
		static_api_ptr_t<playlist_manager> mgr;

		t_size count = mgr->get_playlist_count();

		array<Playlist^>^ items = gcnew array<Playlist^>(count);

		for (t_size i = 0; i < count; i++)
		{
			pfc::string8 name;

			if (!mgr->playlist_get_name(i, name))
				name = "Untitled";
			
			t_size trackCount = mgr->playlist_get_item_count(i);

			items[i] = gcnew Playlist(arg, i, FromUtf8String(name), trackCount);
		}

		return items;
	CALLBACK_END()

	void PlaylistPool::ValidateAll()
	{
		if (m_playlists != nullptr) return;

		m_lock->EnterWriteLock();
		try
		{
			m_playlists = (new Playlists_enum())->Run(this, m_library);
		}
		finally
		{
			m_lock->ExitWriteLock();
		}
	}

}