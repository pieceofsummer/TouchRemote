#pragma once

namespace foo_touchremote
{
	namespace foobar
	{

		class LibraryCallback : public service_impl_t<library_callback>
		{

		public:
			LibraryCallback();

			virtual void on_items_added(metadb_handle_list_cref p_data);
			virtual void on_items_removed(metadb_handle_list_cref p_data);
			virtual void on_items_modified(metadb_handle_list_cref p_data);

		};


	}
}