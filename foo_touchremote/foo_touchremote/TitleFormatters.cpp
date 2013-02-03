#include "stdafx.h"
#include "TitleFormatters.h"
#include "Settings.h"

namespace foo_touchremote
{
	namespace foobar
	{
		namespace titleformat
		{
			static bool is_initialized = false;

			void Initialize()
			{
				if (is_initialized) return;

				static_api_ptr_t<titleformat_compiler> compiler;

				if (!compiler->compile(title, settings::SongTitleFormat))
					console::print("failed to compile title template");
				if (!compiler->compile(album, settings::SongAlbumFormat))
					console::print("failed to compile album template");
				if (!compiler->compile(artist, settings::SongArtistFormat))
					console::print("failed to compile artist template");
				if (!compiler->compile(albumartist, settings::SongAlbumArtistFormat))
					console::print("failed to compile album artist template");
				if (!compiler->compile(genre, settings::SongGenreFormat))
					console::print("failed to compile genre template");
				if (!compiler->compile(composer, settings::SongComposerFormat))
					console::print("failed to compile composer template");
				if (!compiler->compile(rating, settings::SongRatingFormat))
					console::print("failed to compile rating template");

				is_initialized = true;
			}

			titleformat_object::ptr title;
			titleformat_object::ptr album;
			titleformat_object::ptr artist;
			titleformat_object::ptr albumartist;
			titleformat_object::ptr genre;
			titleformat_object::ptr composer;
			titleformat_object::ptr rating;

		}
	}
}