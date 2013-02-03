#include "stdafx.h"
#include "AutoPlaylistFactory.h"
#include "AutoPlaylistClient.h"

#pragma managed

namespace foo_touchremote
{
	namespace foobar
	{


		GUID AutoPlaylistFactory::get_guid()
		{
			return foo_touchremote::guids::AutoPlaylistClient;
		}

		autoplaylist_client_ptr AutoPlaylistFactory::instantiate(stream_reader * p_stream,t_size p_sizehint,abort_callback & p_abort)
		{
			return new AutoPlaylistClient(nullptr);
		}

	}
}