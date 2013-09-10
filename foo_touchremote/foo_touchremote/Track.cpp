#include "stdafx.h"
#include "Track.h"
#include "FilePlaybackSource.h"
#include "Library.h"
#include "IDProvider.h"
#include "Utils.h"
#include "MainThreadCallback.h"
#include "ManagedHost.h"
#include "TitleFormatters.h"

#pragma managed

using namespace System::IO;
using namespace TouchRemote::Core;

namespace foo_touchremote
{

	static int get_int(const char *value)
	{
		if (value == NULL || *value == 0) return 0;

		return atoi(value);
	}

	static const char* get_str(const char *value, const char *def = "")
	{
		if (value == NULL) return def;

		return value;
	}

	static String^ format_string(metadb_handle_ptr &track, const titleformat_object::ptr &format, const file_info *info, const char *fallback)
	{
		pfc::string8_fastalloc s_value;

		if (format.is_valid() && track->format_title_nonlocking(NULL, s_value, format, NULL))
		{
			if (strcmp(s_value, "?") == 0) return String::Empty;
			return FromUtf8String(s_value);
		}

		if (info->meta_exists(fallback))
			return FromUtf8String(info->meta_get(fallback, 0));
		
		return String::Empty;
	}

	static String^ format_string_live(metadb_handle_ptr &track, const titleformat_object::ptr &format, const file_info &info, const char *fallback)
	{
		pfc::string8_fastalloc s_value;

		if (format.is_valid())
		{
            track->format_title_from_external_info_nonlocking(info, NULL, s_value, format, NULL);

			if (strcmp(s_value, "?") == 0) return String::Empty;
			return FromUtf8String(s_value);
		}

		if (info.meta_exists(fallback))
			return FromUtf8String(info.meta_get(fallback, 0));
		
		return String::Empty;
	}

	Track::Track(IMediaLibrary^ library, metadb_handle_ptr &ptr)
	{
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");

		if (ptr.is_empty())
			throw gcnew ArgumentNullException("ptr");

		m_library = library;
		m_source = gcnew FilePlaybackSource(ptr->get_location());
        m_isLive = false;

		Initialize(ptr);
	}

	Track::Track(IMediaLibrary^ library, metadb_handle_ptr &ptr, IPlaybackSource^ location)
	{	
		if (library == nullptr)
			throw gcnew ArgumentNullException("library");

		if (ptr.is_empty())
			throw gcnew ArgumentNullException("ptr");

		if (location == nullptr)
			throw gcnew ArgumentNullException("location");

		m_library = library;
		m_source = location;

		Initialize(ptr);
	}

	void Track::Initialize(metadb_handle_ptr &ptr)
	{
		foobar::titleformat::Initialize();

		ReadInfo(ptr);

        p_native_handle = new metadb_handle_ptr(ptr);
	}

	void Track::ReadInfo(metadb_handle_ptr &ptr)
	{
		in_metadb_sync_fromhandle l_sync(ptr);
		
		const file_info * info = NULL;
		if (!ptr->get_info_locked(info))
			throw gcnew ArgumentException("failed to get info for " + m_source->ToString(), "ptr");
	
        m_duration = TimeSpan::FromSeconds(info->get_length());
		m_title = format_string(ptr, foobar::titleformat::title, info, "TITLE");
		if (String::IsNullOrEmpty(m_title))
			m_title = FromUtf8String(pfc::string_filename(ptr->get_path()).get_ptr());

		{
			String^ album_artist = format_string(ptr, foobar::titleformat::albumartist, info, "ALBUM ARTIST");
			String^ artist = format_string(ptr, foobar::titleformat::artist, info, "ARTIST");
			String^ album = format_string(ptr, foobar::titleformat::album, info, "ALBUM");

			if (!String::IsNullOrEmpty(artist) && !String::IsNullOrEmpty(album_artist) && String::Equals(artist, album_artist, StringComparison::InvariantCultureIgnoreCase))
			{
				artist = nullptr;
			}
			else if (String::IsNullOrEmpty(album_artist))
			{
				album_artist = artist;
				artist = nullptr;
			}

			m_artist = artist;
			((Library^)m_library)->RegisterAlbumAndArtist(album_artist, album, m_artistPtr, m_albumPtr);
		}

		m_genre = format_string(ptr, foobar::titleformat::genre, info, "GENRE");
		m_composer = format_string(ptr, foobar::titleformat::composer, info, "COMPOSER");

		m_trackNumber = get_int(info->meta_get("TRACKNUMBER", 0));
		m_discNumber = get_int(info->meta_get("DISCNUMBER", 0));

		String^ rating = format_string(ptr, foobar::titleformat::rating, info, "RATING");
		int n_rating;
		if (!String::IsNullOrEmpty(rating) && int::TryParse(rating, n_rating))
			m_rating = (TouchRemote::Interfaces::Rating)Math::Max(0, Math::Min(n_rating, 5));
		else
			m_rating = TouchRemote::Interfaces::Rating::None;

        m_kind = (Byte)MediaKind::Track;
	}
    
    void Track::SetDynamic(const file_info &info)
    {
        if (p_native_handle == NULL) return;

        metadb_handle_ptr ptr = *p_native_handle;

        m_liveTitle = format_string_live(ptr, foobar::titleformat::title, info, "TITLE");
			
        String^ album_artist = format_string_live(ptr, foobar::titleformat::albumartist, info, "ALBUM ARTIST");
		String^ artist = format_string_live(ptr, foobar::titleformat::artist, info, "ARTIST");

		if (!String::IsNullOrEmpty(artist))
			m_liveArtist = artist;
		else if (!String::IsNullOrEmpty(album_artist))
			m_liveArtist = album_artist;
        else
            m_liveArtist = nullptr;

		m_liveAlbum = format_string_live(ptr, foobar::titleformat::album, info, "ALBUM");

		m_liveGenre = format_string_live(ptr, foobar::titleformat::genre, info, "GENRE");
		m_liveComposer = format_string_live(ptr, foobar::titleformat::composer, info, "COMPOSER");

        m_isLive = true;
    }

    void Track::CancelDynamic()
    {
        m_isLive = false;
        m_liveTitle = nullptr;
        m_liveArtist = nullptr;
        m_liveAlbum = nullptr;
        m_liveGenre = nullptr;
        m_liveComposer = nullptr;
    }

	Track::~Track()
	{
		//this->!Track();
	}

	Track::!Track()
	{
		if (p_native_handle != NULL)
		{
			if (core_api::are_services_available())
				p_native_handle->release();
			else
				p_native_handle->detach();
            delete p_native_handle;
            p_native_handle = NULL;
		}
	}

	IPlaybackSource^ Track::Source::get()
	{
		return m_source;
	}

	TimeSpan Track::Duration::get()
	{
		return m_duration;
	}

	IArtist^ Track::AlbumArtist::get()
	{
		return m_artistPtr;
	}

	IAlbum^ Track::Album::get()
	{
		return m_albumPtr;
	}
	
	String^ Track::ArtistName::get()
	{
		if (!String::IsNullOrEmpty(m_artist))
			return m_artist;

		if (m_artistPtr != nullptr)
			return m_artistPtr->Name;

		return ""; // "Unknown Artist";
	}

	String^ Track::AlbumArtistName::get()
	{
		if (m_artistPtr != nullptr)
			return m_artistPtr->Name;

		return ""; // "Unknown Artist";
	}

	String^ Track::AlbumName::get()
	{
		if (m_albumPtr != nullptr)
			return m_albumPtr->Title;
			
		return ""; // "Unknown Album";
	}
	
	String^ Track::Title::get()
	{
		return m_title;
	}

	String^ Track::GenreName::get()
	{
		return m_genre;
	}

	String^ Track::ComposerName::get()
	{
		return m_composer;
	}

	int Track::TrackNumber::get()
	{
		return m_trackNumber;
	}

	int Track::DiscNumber::get()
	{
		return m_discNumber;
	}

    Boolean Track::IsLiveStream::get()
    {
        return m_isLive;
    }

    String^ Track::LiveArtistName::get()
	{
		if (m_liveArtist != nullptr)
			return m_liveArtist;

		return ArtistName;
	}

	String^ Track::LiveAlbumName::get()
	{
		if (m_liveAlbum != nullptr)
			return m_liveAlbum;

		return AlbumName;
	}
	
	String^ Track::LiveTitle::get()
	{
		if (m_liveTitle != nullptr)
			return m_liveTitle;

		return Title;
	}

	String^ Track::LiveGenreName::get()
	{
		if (!String::IsNullOrEmpty(m_liveGenre))
			return m_liveGenre;

		return GenreName;	
	}

	String^ Track::LiveComposerName::get()
	{
		if (!String::IsNullOrEmpty(m_liveComposer))
			return m_liveComposer;

		return ComposerName;
	}

	TouchRemote::Interfaces::Rating Track::Rating::get()
	{
		return m_rating;
	}

	CALLBACK_START_UU(TrackRating_set, int, int)
		switch (arg)
		{
		case 0:
			menu_helpers::run_command_context_now_playing(foo_playcount::guids::rating_menu_item, foo_playcount::guids::rating_none);
			break;
		case 1:
			menu_helpers::run_command_context_now_playing(foo_playcount::guids::rating_menu_item, foo_playcount::guids::rating_one_star);
			break;
		case 2:
			menu_helpers::run_command_context_now_playing(foo_playcount::guids::rating_menu_item, foo_playcount::guids::rating_two_stars);
			break;
		case 3:
			menu_helpers::run_command_context_now_playing(foo_playcount::guids::rating_menu_item, foo_playcount::guids::rating_three_stars);
			break;
		case 4:
			menu_helpers::run_command_context_now_playing(foo_playcount::guids::rating_menu_item, foo_playcount::guids::rating_four_stars);
			break;
		case 5:
			menu_helpers::run_command_context_now_playing(foo_playcount::guids::rating_menu_item, foo_playcount::guids::rating_five_stars);
			break;
		}
		return 0;
	CALLBACK_END()

	void Track::Rating::set(TouchRemote::Interfaces::Rating value)
	{
		(new TrackRating_set())->Run(ManagedHost::Instance, (int)value);
		m_rating = value;
	}

	Byte Track::Kind::get()
	{
		return m_kind;
	}

	int Track::Id::get()
	{
		return ((Library^)m_library)->Identifiers->GetId(this);
	}

	__int64 Track::PersistentId::get()
	{
		return ((Library^)m_library)->Identifiers->GetPersistentId(this);
	}

	metadb_handle_ptr Track::GetHandle()
	{
		if (p_native_handle == NULL)
			throw gcnew InvalidOperationException("Track has empty native handle");

		return metadb_handle_ptr(*p_native_handle);
	}

    CALLBACK_START_MU(AlbumArt_extract, Bitmap^, metadb_handle_ptr)
		pfc::list_single_ref_t<metadb_handle_ptr> tracks(arg);
		pfc::list_t<GUID> formats;
		formats.add_item(album_art_ids::cover_front);

        album_art_extractor_instance_v2::ptr extractor;
        try
        {
            extractor = static_api_ptr_t<album_art_manager_v2>()->open(tracks, formats, foobar2000_io::abort_callback_dummy());
        }
        catch (const std::exception & ex)
        {
            console::printf("open error: %s", ex.what());
            return nullptr;
        }

        album_art_data_ptr data;
		try
		{
			data = extractor->query(album_art_ids::cover_front, foobar2000_io::abort_callback_dummy());
		}
		catch (const exception_album_art_not_found &)
		{
			return nullptr;
		}
		catch (const exception_album_art_unsupported_entry &)
		{
			return nullptr;
		}
        catch (const std::exception & ex)
        {
            console::printf("query error: %s", ex.what());
            return nullptr;
        }

		UnmanagedMemoryStream^ stream = gcnew UnmanagedMemoryStream((unsigned char *)data->get_ptr(), data->get_size());

		Bitmap^ b = nullptr;
		try
		{
			b = gcnew Bitmap(stream);
            // Bitmap created from stream corrupts when the stream is closed
            // So we create a copy and immediately dispose original bitmap
			return gcnew Bitmap(b);
		}
		catch (Exception^ ex)
		{
			return nullptr;
		}
		finally
		{
			if (b != nullptr)
				delete b;
			delete stream;
		}
	CALLBACK_END()

	Bitmap^ Track::GetCoverImage()
	{
	    return (new AlbumArt_extract())->Run(ManagedHost::Instance, GetHandle());
	}

	bool Track::Equals(ITrack^ other)
	{
		if (ReferenceEquals(other, nullptr)) return false;
		if (ReferenceEquals(other, this)) return true;

		Track^ o = safe_cast<Track^> (other);
		if (o == nullptr) return false;

		//return GetHandle() == o->GetHandle();

		return ((IEquatable<IPlaybackSource^>^)m_source)->Equals(o->m_source);
	}

	int Track::CompareTo(ITrack^ other)
	{
		if (ReferenceEquals(other, nullptr)) return -1;
		if (ReferenceEquals(other, this)) return 0;

		//if (p_native_handle == NULL)
		//	throw gcnew InvalidOperationException("Track has empty native handle");

		Track^ o = safe_cast<Track^> (other);
		if (o == nullptr) return -1;

		//if (o->p_native_handle == NULL)
		//	throw gcnew ArgumentException("Track has empty native handle", "other");

		//return Math::Sign((__int64)(byte*)(p_native_handle->get_ptr()) - (__int64)(byte*)(o->p_native_handle->get_ptr()));

		return m_source->CompareTo(o->m_source);
	}

	String^ Track::ToString()
	{
		return String::Format("{0} - {1} - {2}", ArtistName, AlbumName, Title);
	}

	int Track::GetHashCode()
	{
		//if (p_native_handle == NULL)
		//	throw gcnew InvalidOperationException("Track has empty native handle");
		//return ((__int64)(byte*)(p_native_handle->get_ptr()) & 0xFFFFFFFF);
		return m_source->GetHashCode();
	}

	bool Track::Equals(System::Object ^other)
	{
		return Equals(safe_cast<ITrack^>(other));
	}

}