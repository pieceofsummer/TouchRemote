#include "stdafx.h"
#include "PreferencesPage.h"
#include "PreferencesPageInstance.h"
#include "Guids.h"

namespace foo_touchremote
{
	namespace foobar
	{

		const char* PreferencesPage::get_name()
		{
			return "TouchRemote DACP server";
		}

		bool PreferencesPage::get_help_url(pfc::string_base &p_out)
		{
			p_out = "http://wintense.com/plugins/foo_touchremote";
			return true;
		}

		GUID PreferencesPage::get_guid()
		{
			return foo_touchremote::guids::PreferencesPage;
		}

		GUID PreferencesPage::get_parent_guid()
		{
			return preferences_page::guid_media_library;
		}

		preferences_page_instance::ptr PreferencesPage::instantiate(HWND parent, preferences_page_callback::ptr callback)
		{
            return new PreferencesPageInstance(parent, callback);
		}

	}
}