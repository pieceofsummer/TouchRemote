#pragma once

namespace foo_touchremote
{
	namespace settings
	{
		extern cfg_int_t<t_uint64> DatabaseId;
	
		extern cfg_string DisplayName;
		extern cfg_uint Port;
		extern cfg_bool HttpLogging;

		extern cfg_string SongTitleFormat;
		extern cfg_string SongAlbumFormat;
		extern cfg_string SongAlbumSortFormat;
		extern cfg_string SongAlbumArtistFormat;
		extern cfg_string SongArtistSortFormat;
		extern cfg_string SongArtistFormat;
		extern cfg_string SongComposerFormat;
		extern cfg_string SongGenreFormat;
		extern cfg_string SongRatingFormat;

		extern cfg_bool CompressNetworkTraffic;
		extern cfg_bool AllowGuestLogins;
        extern cfg_bool StopOnPause;

		namespace defaults
		{
			extern const char *DisplayName;
			extern const char *ServiceName;
			extern t_uint32 Port;
			extern bool HttpLogging;
			
			extern const char *SongTitleFormat;
			extern const char *SongAlbumFormat;
			extern const char *SongAlbumSortFormat;
			extern const char *SongAlbumArtistFormat;
			extern const char *SongArtistSortFormat;
			extern const char *SongArtistFormat;
			extern const char *SongComposerFormat;
			extern const char *SongGenreFormat;
			extern const char *SongRatingFormat;

			extern bool CompressNetworkTraffic;
			extern bool AllowGuestLogins;
            extern bool StopOnPause;
		}
	}

}