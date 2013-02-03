#pragma once


namespace foo_touchremote
{
	namespace foobar
	{

		class PlayCallback : public service_impl_t<play_callback_static>
		{

		public:

			virtual void on_playback_starting(play_control::t_track_command p_command,bool p_paused);
			virtual void on_playback_new_track(metadb_handle_ptr p_track);
			virtual void on_playback_stop(play_control::t_stop_reason p_reason);
			virtual void on_playback_seek(double p_time);
			virtual void on_playback_pause(bool p_state);
			virtual void on_playback_edited(metadb_handle_ptr p_track);
			virtual void on_playback_dynamic_info(const file_info & p_info);
			virtual void on_playback_dynamic_info_track(const file_info & p_info);
			virtual void on_playback_time(double p_time);
			virtual void on_volume_change(float p_new_val);
			virtual unsigned get_flags();

		};

	}
}