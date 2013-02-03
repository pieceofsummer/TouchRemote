#pragma once

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote
{
	// forward reference
	ref class Playlist;

	private ref class PlaylistPool
	{

	public:
		PlaylistPool(IMediaLibrary^ library);

		property array<Playlist^>^ Playlists
		{
			array<Playlist^>^ get();
		}

		property Playlist^ default[t_size]
		{
			Playlist^ get(t_size index);
		}

		void InvalidateAll();

	private:
		void ValidateAll();

		IMediaLibrary^ m_library;
		array<Playlist^>^ m_playlists;
		ReaderWriterLockSlim^ m_lock;
	};

}