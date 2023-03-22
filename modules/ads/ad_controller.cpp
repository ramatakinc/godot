#include "ad_controller.h"

void AdController::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_network", "network"), &AdController::set_network);
	ClassDB::bind_method(D_METHOD("get_network"), &AdController::get_network);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "network"), "set_network", "get_network");

	ADD_SIGNAL(MethodInfo("loaded"));
	ADD_SIGNAL(MethodInfo("shown"));
	ADD_SIGNAL(MethodInfo("hidden"));
	ADD_SIGNAL(MethodInfo("closed"));
	ADD_SIGNAL(MethodInfo("clicked"));
	ADD_SIGNAL(MethodInfo("finished"));
	ADD_SIGNAL(MethodInfo("error"));
}
void AdController::_notification(int p_what) {}

void AdController::set_network(String p_network_alias) {
	// TODO: Validate this and emit warning once if needed.
	this->network_alias = p_network_alias;
}
String AdController::get_network() const {
	return this->network_alias;
}

AdController::AdController() {}
