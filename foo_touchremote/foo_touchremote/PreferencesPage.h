#pragma once

namespace foo_touchremote
{
	namespace foobar
	{

		class PreferencesPage : public service_impl_t<preferences_page_v3>
		{
		public:
			virtual const char * get_name();

			virtual GUID get_guid();

			virtual GUID get_parent_guid();
			
			virtual bool get_help_url(pfc::string_base & p_out);

			virtual preferences_page_instance::ptr instantiate(HWND parent, preferences_page_callback::ptr callback);

        private:
            preferences_page_instance::ptr m_instance;
		};


	}
}