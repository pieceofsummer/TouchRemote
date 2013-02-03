#pragma once


namespace foo_touchremote
{
	namespace foobar
	{
		
		class AutoPlaylistFactory : public service_impl_t<autoplaylist_client_factory>
		{

		public:

			virtual GUID get_guid();

			virtual autoplaylist_client_ptr instantiate(stream_reader * p_stream,t_size p_sizehint,abort_callback & p_abort);

		};



	}
}