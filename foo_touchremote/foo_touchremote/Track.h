#pragma once

using namespace System;
using namespace System::Drawing;
using namespace TouchRemote::Interfaces;

#pragma managed

namespace foo_touchremote
{

	public ref class Track : public ITrack, public IArtworkSource, public ILiveTrack
	{
	public:
		Track(IMediaLibrary^ library, metadb_handle_ptr &ptr);
		Track(IMediaLibrary^ library, metadb_handle_ptr &ptr, IPlaybackSource^ location);
		~Track();
		!Track();

		virtual property int Id
		{
			int get();
		}

		virtual property __int64 PersistentId
		{
			__int64 get();
		}

		virtual property IPlaybackSource^ Source
		{
			IPlaybackSource^ get();
		}

		virtual property TimeSpan Duration
		{
			TimeSpan get();
		}

		virtual property IArtist^ AlbumArtist
		{
			IArtist^ get();
		}

		virtual property IAlbum^ Album
		{
			IAlbum^ get();
		}

		virtual property String^ Title
		{
			String^ get();
		}

		virtual property String^ ArtistName
		{
			String^ get();
		}

		virtual property String^ AlbumArtistName
		{
			String^ get();
		}

		virtual property String^ AlbumName
		{
			String^ get();
		}

		virtual property String^ GenreName
		{
			String^ get();
		}

		virtual property String^ ComposerName
		{
			String^ get();
		}

		virtual property int TrackNumber
		{
			int get();
		}

		virtual property int DiscNumber
		{
			int get();
		}

		virtual property TouchRemote::Interfaces::Rating Rating
		{
			TouchRemote::Interfaces::Rating get();
			void set(TouchRemote::Interfaces::Rating value);
		}

		virtual property Byte Kind
		{
			Byte get();
		}

        virtual property Boolean IsLiveStream
        {
            Boolean get();
        }

		virtual property String^ LiveTitle
		{
			String^ get();
		}

		virtual property String^ LiveArtistName
		{
			String^ get();
		}

		virtual property String^ LiveAlbumName
		{
			String^ get();
		}

		virtual property String^ LiveGenreName
		{
			String^ get();
		}

		virtual property String^ LiveComposerName
		{
			String^ get();
		}

		virtual Bitmap^ GetCoverImage();

		virtual bool Equals(ITrack ^other);

		virtual int CompareTo(ITrack ^other);

		virtual String^ ToString() override;
		virtual bool Equals(Object^ other) override;
		virtual int GetHashCode() override;

	internal:
		void ReadInfo(metadb_handle_ptr &ptr);
        
        void SetDynamic(const file_info &info);
        void CancelDynamic();

		metadb_handle_ptr GetHandle();

	private:
		void Initialize(metadb_handle_ptr &ptr);

		metadb_handle_ptr *p_native_handle;

		IMediaLibrary^ m_library;
		IPlaybackSource^ m_source;
		TimeSpan m_duration;
		int m_trackNumber;
		int m_discNumber;
		TouchRemote::Interfaces::Rating m_rating;

		IAlbum^ m_albumPtr;
		IArtist^ m_artistPtr;

		String^ m_title;
		String^ m_artist;
		String^ m_genre;
		String^ m_composer;

        String^ m_liveTitle;
        String^ m_liveArtist;
        String^ m_liveGenre;
        String^ m_liveAlbum;
        String^ m_liveComposer;

        Boolean m_isLive;
        Byte m_kind;
	};

}