#include "stdafx.h"
#include "TrackPool.h"
#include "Track.h"
#include "Utils.h"

#pragma managed

namespace foo_touchremote
{

	TrackPool::TrackPool(IMediaLibrary^ library)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");

		m_library = library;
		m_tracks = gcnew Dictionary<IntPtr, WeakReference^>();
		m_lock = gcnew ReaderWriterLockSlim(LockRecursionPolicy::NoRecursion);
	}

	ITrack^ TrackPool::GetTrack(metadb_handle_ptr &ptr)
	{
		return GetTrack(ptr, false, false);
	}

	ITrack^ TrackPool::GetTrack(metadb_handle_ptr &ptr, bool update)
	{
		return GetTrack(ptr, update, false);
	}

	ITrack^ TrackPool::GetTrack(metadb_handle_ptr &ptr, bool update, bool doNotCreate)
	{
		if (ptr.is_empty()) return nullptr;

		IntPtr key = (IntPtr)(void*)ptr.get_ptr();
		WeakReference ^r;
		m_lock->EnterUpgradeableReadLock();
		try
		{
			if (!m_tracks->TryGetValue(key, r) || !r->IsAlive)
			{
				if (doNotCreate) return nullptr;

				m_lock->EnterWriteLock();
				try
				{
					r = gcnew WeakReference(gcnew Track(m_library, ptr));
					m_tracks[key] = r;
				}
				finally
				{
					m_lock->ExitWriteLock();
				}
			}
			else if (update)
			{
				static_cast<Track^>(r->Target)->ReadInfo(ptr);
			}

			return static_cast<ITrack^>(r->Target);
		}
		finally
		{
			m_lock->ExitUpgradeableReadLock();
		}
	}

}