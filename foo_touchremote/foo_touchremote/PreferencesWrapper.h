#pragma once

using namespace System;
using namespace System::Collections;
using namespace System::Diagnostics;
using namespace TouchRemote::Interfaces;

namespace foo_touchremote 
{

	public ref class PreferencesWrapper : public IPreferences
	{
	public:
		PreferencesWrapper();
		
		virtual property bool SortTracksAlphabetically
		{
			bool get();
		}

		virtual property bool CustomSortAlbums
		{
			bool get();
		}

		virtual property bool CustomSortArtists
		{
			bool get();
		}

		virtual property bool CompressNetworkTraffic
		{
			bool get();
		}

		virtual property bool AllowGuestLogins
		{
			bool get();
		}

	};

}