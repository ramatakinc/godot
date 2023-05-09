#include "ad_controller.h"

#include "servers/ramatak/ad_server.h"

void AdController::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ad_unit", "ad_unit"), &AdController::set_ad_unit);
	ClassDB::bind_method(D_METHOD("get_ad_unit"), &AdController::get_ad_unit);

	ClassDB::bind_method(D_METHOD("show"), &AdController::show);
	ClassDB::bind_method(D_METHOD("hide"), &AdController::hide);

	ClassDB::bind_method(D_METHOD("_ad_clicked", "request_token"), &AdController::_ad_clicked);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "ad_unit", PropertyHint::PROPERTY_HINT_AD_UNIT), "set_ad_unit", "get_ad_unit");

	ADD_SIGNAL(MethodInfo("ad_shown"));
	ADD_SIGNAL(MethodInfo("ad_hidden"));
	ADD_SIGNAL(MethodInfo("ad_closed"));
	ADD_SIGNAL(MethodInfo("ad_clicked"));
	ADD_SIGNAL(MethodInfo("ad_reward_earned"));
	ADD_SIGNAL(MethodInfo("ad_error"));
}
void AdController::_notification(int p_what) {
	if (p_what == NOTIFICATION_EXIT_TREE) {
		hide();
	}
}

void AdController::set_ad_unit(String p_ad_unit) {
	ad_unit = p_ad_unit;
}

String AdController::get_ad_unit() const {
	return ad_unit;
}

void AdController::show() {
	if (ad_unit == "") {
		WARN_PRINT("Attempting to show advertisement, but no ad unit selected.");
		return;
	}
	request_tokens.append(AdServer::get_singleton()->show_other(ad_unit));
}

void AdController::hide() {
	AdServer::get_singleton()->hide(ad_unit);
}

void AdController::_ad_clicked(Variant p_request_token) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_clicked");
	}
}

AdController::AdController() {
	AdServer::get_singleton()->connect("ad_clicked", this, "_ad_clicked");
}
