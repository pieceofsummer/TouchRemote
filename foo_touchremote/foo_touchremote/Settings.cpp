#include "stdafx.h"
#include "Guids.h"
#include "Settings.h"

namespace foo_touchremote
{
	namespace settings
	{
		cfg_int_t<t_uint64> DatabaseId(guids::Setting_DatabaseId, 0);

		cfg_string DisplayName(guids::Setting_DisplayName, defaults::DisplayName);
		cfg_uint Port(guids::Setting_Port, defaults::Port);
		cfg_bool HttpLogging(guids::Setting_HttpLogging, defaults::HttpLogging);

		cfg_string SongTitleFormat(guids::Setting_SongTitleFormat, defaults::SongTitleFormat);
		cfg_string SongAlbumFormat(guids::Setting_SongAlbumFormat, defaults::SongAlbumFormat);
		cfg_string SongAlbumSortFormat(guids::Setting_SongAlbumSortFormat, defaults::SongAlbumSortFormat);
		cfg_string SongAlbumArtistFormat(guids::Setting_SongAlbumArtistFormat, defaults::SongAlbumArtistFormat);
		cfg_string SongArtistSortFormat(guids::Setting_SongArtistSortFormat, defaults::SongArtistSortFormat);
		cfg_string SongArtistFormat(guids::Setting_SongArtistFormat, defaults::SongArtistFormat);
		cfg_string SongComposerFormat(guids::Setting_SongComposerFormat, defaults::SongComposerFormat);
		cfg_string SongGenreFormat(guids::Setting_SongGenreFormat, defaults::SongGenreFormat);
		cfg_string SongRatingFormat(guids::Setting_SongRatingFormat, defaults::SongRatingFormat);

		cfg_bool CompressNetworkTraffic(guids::Setting_CompressNetworkTraffic, defaults::CompressNetworkTraffic);
		cfg_bool AllowGuestLogins(guids::Setting_AllowGuestLogins, defaults::AllowGuestLogins);
        cfg_bool StopOnPause(guids::Setting_StopOnPause, defaults::StopOnPause);

		namespace defaults
		{

			const char *DisplayName = "foobar 2000";
			t_uint32 Port = 7000;
			bool HttpLogging = false;
			
			const char *SongTitleFormat = "%title%";
			const char *SongAlbumFormat = "%album%";
			const char *SongAlbumSortFormat = "";
			const char *SongAlbumArtistFormat = "$if2(%album artist%,%artist%)";
			const char *SongArtistSortFormat = "$stripprefix($if2(%album artist%,%artist%))";
			const char *SongArtistFormat = "%artist%";
			const char *SongComposerFormat = "%composer%";
			const char *SongGenreFormat = "%genre%";
			const char *SongRatingFormat = "$if2(%rating%,0)";

			bool CompressNetworkTraffic = false;
			bool AllowGuestLogins = false;
            bool StopOnPause = false;
		}
	}
}