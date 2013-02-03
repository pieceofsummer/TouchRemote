#pragma once

using namespace System;
using namespace System::Drawing;
using namespace TouchRemote::Interfaces;

#pragma managed

namespace foo_touchremote
{

	public ref class JukeboxTrack : public Track, public IPropertyExtender
	{
	public:
		JukeboxTrack(IMediaLibrary^ library, metadb_handle_ptr &ptr);
		JukeboxTrack(IMediaLibrary^ library, metadb_handle_ptr &ptr, IPlaybackSource^ location);

		virtual void Extend(System::Collections::Generic::IDictionary<String^, Object^>^ data);

	};

}