#pragma once

/*  */

#ifdef _DEBUG
#define CALLBACK_TRACE(x) console::print(x);
#else
#define CALLBACK_TRACE(x) 
#endif

#define CALLBACK_COMMON(name, T, A) \
		CALLBACK_TRACE("TouchRemote Debug: entered " #name) \
		m_this = p_this; \
		m_arg = arg; \
		service_add_ref(); \
		try \
		{ \
			if (core_api::is_main_thread()) { \
				m_hWaitFor = NULL; \
				callback_run(); \
			} else { \
				/*System::Threading::Monitor::Enter(p_this);*/ \
				try { \
					m_hWaitFor = CreateEvent(NULL, TRUE, FALSE, NULL); \
					static_api_ptr_t<main_thread_callback_manager>()->add_callback(this); \
					WaitForSingleObject(m_hWaitFor, INFINITE); \
					CloseHandle(m_hWaitFor); \
				} \
				finally \
				{ \
					/*System::Threading::Monitor::Exit(p_this);*/ \
				} \
			} \
			return (T) m_result; \
		} finally { \
			service_release(); \
			CALLBACK_TRACE("TouchRemote Debug: leaved " #name) \
		} \
	} \
private: T DoWork(A arg) {


#define CALLBACK_START_MU(name, T, A) \
class name : public service_impl_t<main_thread_callback> { \
private: \
	HANDLE m_hWaitFor; \
	gcroot< T > m_result; \
	A m_arg; \
	gcroot< Object^ > m_this; \
public: T Run(Object^ p_this, A arg = NULL) { \
	CALLBACK_COMMON(name, T, A)


#define CALLBACK_START_MM(name, T, A) \
class name : public service_impl_t<main_thread_callback> { \
private: \
	HANDLE m_hWaitFor; \
	gcroot< T > m_result; \
	gcroot< A > m_arg; \
	gcroot< Object^ > m_this; \
public: T Run(Object^ p_this, A arg = nullptr) { \
	CALLBACK_COMMON(name, T, A)


#define CALLBACK_START_UU(name, T, A) \
class name : public service_impl_t<main_thread_callback> { \
private: \
	HANDLE m_hWaitFor; \
	T m_result; \
	A m_arg; \
	gcroot< Object^ > m_this; \
public: T Run(Object^ p_this, A arg = NULL) { \
	CALLBACK_COMMON(name, T, A)


#define CALLBACK_START_UM(name, T, A) \
class name : public service_impl_t<main_thread_callback> { \
private: \
	HANDLE m_hWaitFor; \
	T m_result; \
	gcroot< A > m_arg; \
	gcroot< Object^ > m_this; \
public: T Run(Object^ p_this, A arg = nullptr) { \
	CALLBACK_COMMON(name, T, A)


#define CALLBACK_END() \
	} \
public: virtual void callback_run() { \
		CALLBACK_TRACE("TouchRemote Debug: in callback") \
		try { \
			m_result = DoWork(m_arg); \
		} catch (Exception^ ex) { \
			_console::error(ex->ToString()); \
		} catch (pfc::exception &ex) { \
			console::error(ex.what()); \
		} finally { \
			CALLBACK_TRACE("TouchRemote Debug: out of callback") \
			if (m_hWaitFor != NULL) SetEvent(m_hWaitFor); \
		} \
	} \
};