#pragma once

namespace foo_touchremote
{
	ref class PreferencesPageDialog;

	namespace foobar
	{

		class PreferencesPageInstance : public preferences_page_instance
		{
		public:
			PreferencesPageInstance(HWND parent, preferences_page_callback::ptr callback);
            virtual ~PreferencesPageInstance();

			virtual t_uint32 get_state();
			virtual HWND get_wnd();
			virtual void apply();
			virtual void reset();
                        
	        int FB2KAPI service_release() throw();
            int FB2KAPI service_add_ref() throw();

            void SetDestroyInProgress() { m_destroyWindowInProgress = true; }

		private:
            pfc::refcounter m_counter;
            volatile bool m_destroyWindowInProgress;
            volatile LONG m_delayedDestroyInProgress;
            HWND m_parent;
			preferences_page_callback::ptr m_callback;
			gcroot<PreferencesPageDialog^> m_ui;
		};

	}
}
