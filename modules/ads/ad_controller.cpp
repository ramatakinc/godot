#include "ad_controller.h"

#include "servers/ramatak/ad_server.h"

void AdController::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ad_placement", "ad_placement"), &AdController::set_ad_placement);
	ClassDB::bind_method(D_METHOD("get_ad_placement"), &AdController::get_ad_placement);

	ClassDB::bind_method(D_METHOD("show"), &AdController::show);
	ClassDB::bind_method(D_METHOD("hide"), &AdController::hide);

	ClassDB::bind_method(D_METHOD("_ad_loaded", "request_token"), &AdController::_ad_loaded);
	ClassDB::bind_method(D_METHOD("_ad_shown", "request_token"), &AdController::_ad_shown);
	ClassDB::bind_method(D_METHOD("_ad_closed", "request_token"), &AdController::_ad_closed);
	ClassDB::bind_method(D_METHOD("_ad_clicked", "request_token"), &AdController::_ad_clicked);
	ClassDB::bind_method(D_METHOD("_ad_reward_earned", "request_token"), &AdController::_ad_reward_earned);
	ClassDB::bind_method(D_METHOD("_ad_error", "request_token", "message"), &AdController::_ad_error);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "ad_placement", PropertyHint::PROPERTY_HINT_AD_UNIT), "set_ad_placement", "get_ad_placement");

	ADD_SIGNAL(MethodInfo("ad_loaded"));
	ADD_SIGNAL(MethodInfo("ad_shown"));
	ADD_SIGNAL(MethodInfo("ad_closed"));
	ADD_SIGNAL(MethodInfo("ad_clicked"));
	ADD_SIGNAL(MethodInfo("ad_reward_earned"));
	ADD_SIGNAL(MethodInfo("ad_error", PropertyInfo(Variant::STRING, "message")));
}
void AdController::_notification(int p_what) {
	if (p_what == NOTIFICATION_EXIT_TREE) {
		hide();
	}
}

void AdController::set_ad_placement(String p_ad_placement) {
	ad_placement = p_ad_placement;
}

String AdController::get_ad_placement() const {
	return ad_placement;
}

void AdController::show() {
	if (ad_placement == "") {
		WARN_PRINT("Attempting to show advertisement, but no ad placement selected.");
		return;
	}
	request_tokens.append(AdServer::get_singleton()->show_other(ad_placement));
}

void AdController::hide() {
	AdServer::get_singleton()->hide(ad_placement);
}

void AdController::_ad_loaded(Variant p_request_token) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_loaded");
	}
}

void AdController::_ad_shown(Variant p_request_token) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_shown");
	}
}

void AdController::_ad_clicked(Variant p_request_token) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_clicked");
	}
}

void AdController::_ad_closed(Variant p_request_token) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_closed");
	}
}

void AdController::_ad_reward_earned(Variant p_request_token) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_reward_earned");
	}
}

void AdController::_ad_error(Variant p_request_token, Variant message) {
	if (request_tokens.has(p_request_token)) {
		emit_signal("ad_error", message);
	}
}

AdController::AdController() {
	AdServer::get_singleton()->connect("ad_loaded", this, "_ad_loaded");
	AdServer::get_singleton()->connect("ad_shown", this, "_ad_shown");
	AdServer::get_singleton()->connect("ad_clicked", this, "_ad_clicked");
	AdServer::get_singleton()->connect("ad_closed", this, "_ad_closed");
	AdServer::get_singleton()->connect("ad_reward_earned", this, "_ad_reward_earned");
	AdServer::get_singleton()->connect("ad_error", this, "_ad_error");
}
