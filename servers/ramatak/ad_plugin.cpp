#include "ad_plugin.h"

void AdPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init_plugin", "api_key"), &AdPlugin::init_plugin);

	ClassDB::bind_method(D_METHOD("show", "ad_unit", "request_token"), &AdPlugin::show);
	ClassDB::bind_method(D_METHOD("hide", "ad_unit", "request_token"), &AdPlugin::hide);

	ClassDB::bind_method(D_METHOD("hide_all", "request_token"), &AdPlugin::hide_all);

	ADD_SIGNAL(MethodInfo("shown"));
	ADD_SIGNAL(MethodInfo("hidden"));
	ADD_SIGNAL(MethodInfo("closed"));
	ADD_SIGNAL(MethodInfo("clicked"));
	ADD_SIGNAL(MethodInfo("finished"));
	ADD_SIGNAL(MethodInfo("error"));

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(AUTH_ERROR);
	BIND_ENUM_CONSTANT(NETWORK_ERROR);
	BIND_ENUM_CONSTANT(UNSPECIFIED_ERROR);
}