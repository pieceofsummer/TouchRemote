#pragma once

namespace foo_touchremote
{
	namespace foobar
	{

		class QueueCallback : public service_impl_t<playback_queue_callback>
		{
		public:
			virtual void on_changed(t_change_origin p_origin);
		};

	}
}