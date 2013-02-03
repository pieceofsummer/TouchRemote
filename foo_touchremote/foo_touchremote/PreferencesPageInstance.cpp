#include "stdafx.h"
#include "PreferencesPageInstance.h"
#include "PreferencesPageDialog.h"
#include "resource1.h"

#pragma managed

namespace foo_touchremote
{
	namespace foobar
	{

		PreferencesPageInstance::PreferencesPageInstance(HWND parent, preferences_page_callback::ptr callback)
		{
            m_destroyWindowInProgress = false;
            m_delayedDestroyInProgress = 0;
            m_parent = parent;
			m_callback = callback;
            
            m_ui = gcnew PreferencesPageDialog(this, (IntPtr)(void*)m_parent, m_callback.get_ptr());
		}

        PreferencesPageInstance::~PreferencesPageInstance()
        {
            m_ui->DestroyWindowHandle();
            m_ui->SetPageNull();
            m_ui = nullptr;
        }

		t_uint32 PreferencesPageInstance::get_state()
		{
			return (t_uint32)(m_ui->State);
		}

		HWND PreferencesPageInstance::get_wnd()
		{
            return (HWND)(void*)m_ui->Handle;
		}
		
		void PreferencesPageInstance::apply()
		{
			m_ui->Apply();
		}

		void PreferencesPageInstance::reset()
		{
			m_ui->Reset();
		}

        int PreferencesPageInstance::service_release() throw()
        {
		    int ret = --m_counter; 
		    if (ret == 0) 
            {
			    if (!InterlockedExchange(&m_delayedDestroyInProgress,1)) 
                {
				    PFC_ASSERT_NO_EXCEPTION( service_impl_helper::release_object_delayed(this); );
			    } 
                else if (m_ui->IsHandleCreated) 
                {
                    service_ptr bump(this);
				    if (!m_destroyWindowInProgress) 
                    {
                        // don't double-destroy in weird scenarios
                        m_ui->DestroyWindowHandle();
                    }
				}
			    else 
                {
				    PFC_ASSERT_NO_EXCEPTION( delete this );
			    }
        	}
        	return ret;
        }

        int PreferencesPageInstance::service_add_ref() throw()
        {
            return ++m_counter;
        }
	}
}
