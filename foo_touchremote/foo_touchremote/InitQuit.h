#pragma once

namespace foo_touchremote
{
	namespace foobar
	{

		class NOVTABLE InitQuit : public service_impl_t<initquit>
		{
		public:
			InitQuit();
			virtual void on_init();
			virtual void on_quit();
		};

	}
}