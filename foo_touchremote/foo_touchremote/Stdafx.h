#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <vcclr.h>

#pragma unmanaged

#include <windows.h>
#include <shellapi.h>
#include <ole2.h>

#pragma comment(lib, "../../foobarsdk/foobar2000/shared/shared.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")

// TODO: reference additional headers your program requires here
#include "../../foobarsdk/foobar2000/SDK/foobar2000.h"
//#include "../../foobarsdk/foobar2000/helpers/helpers.h"

#include "Guids.h"

#define PATH_COMPARE_MODE StringComparison::OrdinalIgnoreCase
#define SELECTION_PLAYLIST_NAME "TouchRemote Selection"