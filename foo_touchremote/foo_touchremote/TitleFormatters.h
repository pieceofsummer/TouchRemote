#pragma once


namespace foo_touchremote
{
	namespace foobar
	{
		namespace titleformat
		{
			void Initialize();

			extern titleformat_object::ptr title;
			extern titleformat_object::ptr album;
			extern titleformat_object::ptr artist;
			extern titleformat_object::ptr albumartist;
			extern titleformat_object::ptr genre;
			extern titleformat_object::ptr composer;
			extern titleformat_object::ptr rating;

		}
	}
}