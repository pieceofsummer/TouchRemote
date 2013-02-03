#include "stdafx.h"
#include "Track.h"
#include "JukeboxTrack.h"
#include "FilePlaybackSource.h"
#include "Library.h"
#include "IDProvider.h"
#include "Utils.h"
#include "MainThreadCallback.h"
#include "ManagedHost.h"
#include "TitleFormatters.h"

#pragma managed

using namespace System::IO;

namespace foo_touchremote
{

	JukeboxTrack::JukeboxTrack(IMediaLibrary^ library, metadb_handle_ptr &ptr) : Track(library, ptr)
	{
	}

	JukeboxTrack::JukeboxTrack(IMediaLibrary^ library, metadb_handle_ptr &ptr, IPlaybackSource^ location) : Track(library, ptr, location)
	{
	}

	void JukeboxTrack::Extend(System::Collections::Generic::IDictionary<String^, Object^>^ data)
	{
		data["ceJV"] = (Object^)0;
		data["ceJC"] = true;
		//data["asdb"] = false;
	}

}