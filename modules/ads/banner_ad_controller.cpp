#include "banner_ad_controller.h"

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

	BIND_ENUM_CONSTANT(BannerAdSize::SMALL);
	BIND_ENUM_CONSTANT(BannerAdSize::LARGE);

	BIND_ENUM_CONSTANT(BannerAdLocation::BOTTOM);
	BIND_ENUM_CONSTANT(BannerAdLocation::TOP);
}
void BannerAdController::_notification(int p_what) {}

bool BannerAdController::get_auto_show() const {
	return this->auto_show;
}

void BannerAdController::set_auto_show(bool p_auto_show) {
	this->auto_show = p_auto_show;
}

BannerAdController::BannerAdSize BannerAdController::get_size() const {
	return this->size;
}

void BannerAdController::set_size(BannerAdController::BannerAdSize p_size) {
	this->size = p_size;
}

BannerAdController::BannerAdLocation BannerAdController::get_location() const {
	return this->location;
}

void BannerAdController::set_location(BannerAdController::BannerAdLocation p_location) {
	this->location = p_location;
}

BannerAdController::BannerAdController() :
		auto_show(false), location(BOTTOM), size(SMALL) {}
