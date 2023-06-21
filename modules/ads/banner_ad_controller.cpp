#include "banner_ad_controller.h"
#include "core/engine.h"
#include "servers/ramatak/ad_server.h"

void BannerAdController::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_auto_show", "auto_show"), &BannerAdController::set_auto_show);
	ClassDB::bind_method(D_METHOD("get_auto_show"), &BannerAdController::get_auto_show);

	ClassDB::bind_method(D_METHOD("set_size", "size"), &BannerAdController::set_size);
	ClassDB::bind_method(D_METHOD("get_size"), &BannerAdController::get_size);

	ClassDB::bind_method(D_METHOD("set_location", "location"), &BannerAdController::set_location);
	ClassDB::bind_method(D_METHOD("get_location"), &BannerAdController::get_location);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_show"), "set_auto_show", "get_auto_show");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "size", PROPERTY_HINT_ENUM, "Small,Large"), "set_size", "get_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "location", PROPERTY_HINT_ENUM, "Bottom,Top"), "set_location", "get_location");
}
void BannerAdController::_notification(int p_what) {
	AdController::_notification(p_what);
	if (p_what == NOTIFICATION_READY && auto_show && !Engine::get_singleton()->is_editor_hint()) {
		show();
	}
}

bool BannerAdController::get_auto_show() const {
	return this->auto_show;
}

void BannerAdController::set_auto_show(bool p_auto_show) {
	this->auto_show = p_auto_show;
}

void BannerAdController::show() {
	if (ad_unit == "") {
		WARN_PRINT("Attempting to show banner advertisement, but no ad unit selected.");
		return;
	}
	request_tokens.append(AdServer::get_singleton()->show_banner(ad_unit, size, location));
}

AdServer::BannerAdSize BannerAdController::get_size() const {
	return this->size;
}

void BannerAdController::set_size(AdServer::BannerAdSize p_size) {
	this->size = p_size;
}

AdServer::BannerAdLocation BannerAdController::get_location() const {
	return this->location;
}

void BannerAdController::set_location(AdServer::BannerAdLocation p_location) {
	this->location = p_location;
}

BannerAdController::BannerAdController() :
		auto_show(false), location(AdServer::BANNER_LOCATION_BOTTOM), size(AdServer::BANNER_SIZE_SMALL) {}
