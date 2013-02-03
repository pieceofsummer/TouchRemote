#include "StdAfx.h"
#include "PreferencesWrapper.h"
#include "Utils.h"
#include "Settings.h"

#pragma managed

namespace foo_touchremote
{

	PreferencesWrapper::PreferencesWrapper()
	{
	}

	bool PreferencesWrapper::SortTracksAlphabetically::get()
	{
		return false;
	}

	bool PreferencesWrapper::CustomSortArtists::get()
	{
		return false;
	}

	bool PreferencesWrapper::CustomSortAlbums::get()
	{
		return false;
	}
	
	bool PreferencesWrapper::CompressNetworkTraffic::get()
	{
		return settings::CompressNetworkTraffic;
	}

	bool PreferencesWrapper::AllowGuestLogins::get()
	{
		return settings::AllowGuestLogins;
	}

}