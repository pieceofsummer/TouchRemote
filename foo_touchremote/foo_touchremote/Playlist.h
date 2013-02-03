#pragma once

using namespace System;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;
using namespace System::Collections::Generic;

#pragma managed

namespace foo_touchremote
{

	public ref class Playlist : public IEditablePlaylist, public IPlayableTrackCollection
	{

	public:
		Playlist(IMediaLibrary^ library, int index, String^ name);
		Playlist(IMediaLibrary^ library, int index, String^ name, int trackCount);
		~Playlist();

		virtual property int Id
		{
			int get();
		}

		virtual property __int64 PersistentId
		{
			__int64 get();
		}

		virtual property System::Collections::Generic::IEnumerable<ITrack^>^ Tracks
		{
			System::Collections::Generic::IEnumerable<ITrack^>^ get();
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

		virtual bool Equals(IPlaylist ^other);
		virtual String^ ToString() override;
		virtual bool Equals(Object^ other) override;
		virtual int GetHashCode() override;

		virtual property int Index
		{
			int get();
		}

		virtual void PlayRandom();
		virtual void Play(int index);

		virtual void Delete();
		virtual void Clear();
		virtual void AddTrack(ITrack^ track);
		virtual void AddTrackRange(System::Collections::Generic::IEnumerable<ITrack^>^ tracks);
		virtual void RemoveTrack(ITrack^ track);
		virtual void RemoveTrackRange(System::Collections::Generic::IEnumerable<ITrack^>^ tracks);
		virtual void ReorderTracks(int trackToMove, int moveAfter);

	internal:
		void Invalidate();

	private:
		String^ m_name;
		int m_trackCount;
		IMediaLibrary^ m_library;
		int m_index;
		List<ITrack^>^ m_tracks;
	};

}