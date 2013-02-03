#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote
{

	private ref class TrackPool
	{

	public:
		TrackPool(IMediaLibrary^ library);

		virtual ITrack^ GetTrack(metadb_handle_ptr &ptr);
		virtual ITrack^ GetTrack(metadb_handle_ptr &ptr, bool update);
		virtual ITrack^ GetTrack(metadb_handle_ptr &ptr, bool update, bool doNotCreate);

	private:
		IMediaLibrary^ m_library;
		Dictionary<IntPtr, WeakReference^>^ m_tracks;
		ReaderWriterLockSlim^ m_lock;

	};

}