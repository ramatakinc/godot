#include "ad_controller.h"

#include "servers/ramatak/ad_server.h"

void AdController::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_network", "network"), &AdController::set_network);
	ClassDB::bind_method(D_METHOD("get_network"), &AdController::get_network);

	ClassDB::bind_method(D_METHOD("set_ad_unit", "ad_unit"), &AdController::set_ad_unit);
	ClassDB::bind_method(D_METHOD("get_ad_unit"), &AdController::get_ad_unit);

	ClassDB::bind_method(D_METHOD("show"), &AdController::show);
	ClassDB::bind_method(D_METHOD("hide"), &AdController::hide);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ad_unit", PropertyHint::PROPERTY_HINT_AD_UNIT), "set_ad_unit", "get_ad_unit");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "network"), "set_network", "get_network");

	ADD_SIGNAL(MethodInfo("shown"));
	ADD_SIGNAL(MethodInfo("hidden"));
	ADD_SIGNAL(MethodInfo("closed"));
	ADD_SIGNAL(MethodInfo("clicked"));
	ADD_SIGNAL(MethodInfo("finished"));
	ADD_SIGNAL(MethodInfo("error"));
}
void AdController::_notification(int p_what) {
	// TODO: If this controller leaves the tree, consider hiding the ad.
}

void AdController::set_network(String p_network_alias) {
	// TODO: Validate this and emit warning once if needed.
	this->network_alias = p_network_alias;
}
String AdController::get_network() const {
	return this->network_alias;
}

void AdController::set_ad_unit(String p_ad_unit) {
	ad_unit = p_ad_unit;
}

String AdController::get_ad_unit() const {
	return ad_unit;
}

void AdController::show() {
	AdServer::get_singleton()->show(ad_unit);
}

void AdController::hide() {
	AdServer::get_singleton()->show(ad_unit);
}

AdController::AdController() {}
