#include "ad_plugin.h"
#include "core/project_settings.h"

void AdPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init_plugin"), &AdPlugin::init_plugin);

	ClassDB::bind_method(D_METHOD("show_banner", "ad_unit", "request_token", "size", "location"), &AdPlugin::show_banner);
	ClassDB::bind_method(D_METHOD("show_other", "ad_unit", "request_token"), &AdPlugin::show_other);
	ClassDB::bind_method(D_METHOD("hide", "ad_unit", "request_token"), &AdPlugin::hide);

	ClassDB::bind_method(D_METHOD("hide_all", "request_token"), &AdPlugin::hide_all);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(AUTH_ERROR);
	BIND_ENUM_CONSTANT(NETWORK_ERROR);
	BIND_ENUM_CONSTANT(UNSPECIFIED_ERROR);
}

bool AdPlugin::_get_debug_mode() const {
	return ProjectSettings::get_singleton()->get_setting(AD_DEBUG_MODE_KEY);
}

bool AdPlugin::_get_child_directed() const {
	return ProjectSettings::get_singleton()->get_setting(AD_CHILD_DIRECTED_KEY);
}
