#include "stdafx.h"
#include "InitQuit.h"
#include "LibraryCallback.h"
#include "PlaylistCallback.h"
#include "PlayCallback.h"
#include "AutoPlaylistFactory.h"
#include "PreferencesPage.h"
#include "QueueCallback.h"
#include "Guids.h"

DECLARE_COMPONENT_VERSION
(
    "TouchRemote DACP server for foobar2000",
    "0.2.8.4",
    "(c) winTense 2009-2013\n"
	"\n"
	"This is a preview version, correct work is not guaranteed\n"
	"\n"
	"Compiled: " __DATE__ "\n"
)

VALIDATE_COMPONENT_FILENAME("foo_touchremote.dll")

static initquit_factory_t<foo_touchremote::foobar::InitQuit> _InitQuit;
static library_callback_factory_t<foo_touchremote::foobar::LibraryCallback> _LibraryCallback;
static service_factory_t<foo_touchremote::foobar::PlaylistCallback> _PlaylistCallback;
static play_callback_static_factory_t<foo_touchremote::foobar::QueueCallback> _QueueCallback;
static play_callback_static_factory_t<foo_touchremote::foobar::PlayCallback> _PlayCallback;
static service_factory_t<foo_touchremote::foobar::AutoPlaylistFactory> _AutoPlaylistFactory;
static preferences_page_factory_t<foo_touchremote::foobar::PreferencesPage> _PreferencesPage;

static advconfig_branch_factory _AdvConfig("TouchRemote DACP Server", foo_touchremote::guids::AdvConfigBranch, advconfig_entry::guid_root, 0);
advconfig_string_factory _AdvConfig_HostName("Host SRV name", foo_touchremote::guids::AdvConfig_HostName, foo_touchremote::guids::AdvConfigBranch, 0, "", preferences_state::needs_restart);
