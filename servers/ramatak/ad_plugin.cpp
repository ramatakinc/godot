#include "ad_plugin.h"

void AdPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init_plugin"), &AdPlugin::init_plugin);

	ClassDB::bind_method(D_METHOD("show_banner", "ad_unit", "request_token", "size", "location"), &AdPlugin::show_banner);
	ClassDB::bind_method(D_METHOD("show_other", "ad_unit", "request_token"), &AdPlugin::show_other);
	ClassDB::bind_method(D_METHOD("hide", "ad_unit", "request_token"), &AdPlugin::hide);

	ClassDB::bind_method(D_METHOD("hide_all", "request_token"), &AdPlugin::hide_all);

	ADD_SIGNAL(MethodInfo("ad_shown", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_hidden", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_closed", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_clicked", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_reward_earned", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_error", PropertyInfo(Variant::INT, "request_id")));

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(AUTH_ERROR);
	BIND_ENUM_CONSTANT(NETWORK_ERROR);
	BIND_ENUM_CONSTANT(UNSPECIFIED_ERROR);
}