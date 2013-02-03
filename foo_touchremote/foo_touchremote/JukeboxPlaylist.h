#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;
using namespace TouchRemote::Core::Library;
using namespace System::Collections::Generic;

#pragma managed

namespace foo_touchremote
{

	public ref class JukeboxPlaylist : public SpecialPlaylistBase, public IJukeboxPlaylist
	{

	public:
		JukeboxPlaylist(IMediaLibrary^ library, String^ name);
		JukeboxPlaylist(IMediaLibrary^ library, String^ name, int trackCount);
		~JukeboxPlaylist();

		virtual property System::Collections::Generic::IEnumerable<ITrack^>^ Tracks
		{
			System::Collections::Generic::IEnumerable<ITrack^>^ get() override;
		}

		virtual property int TrackCount
		{
			int get() override;
		}

		virtual void Extend(System::Collections::Generic::IDictionary<String^, Object^>^ data) override;

		virtual void Vote(ITrack^ track);

	internal:
		void Invalidate();

	private:
		String^ m_name;
		int m_trackCount;
		IMediaLibrary^ m_library;
		List<ITrack^>^ m_tracks;
	};

}