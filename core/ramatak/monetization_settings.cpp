#include "monetization_settings.h"
#include "core/project_settings.h"

MonetizationSettings *MonetizationSettings::singleton = nullptr;
const static String ad_unit_setting = "ramatak/monetization/ad_units";

MonetizationSettings::MonetizationSettings() {
	singleton = this;
}

MonetizationSettings::~MonetizationSettings() {
}

MonetizationSettings *MonetizationSettings::get_singleton() {
	return singleton;
}

void MonetizationSettings::_bind_methods() {
	ClassDB::bind_method(D_METHOD("delete_ad_unit", "ad_unit"), &MonetizationSettings::delete_ad_unit);
	ClassDB::bind_method(D_METHOD("set_ad_unit_network_id", "ad_unit", "network", "network_specific_id"), &MonetizationSettings::set_ad_unit_network_id);
	ClassDB::bind_method(D_METHOD("get_ad_unit_network_id", "ad_unit", "network"), &MonetizationSettings::get_ad_unit_network_id);
	ClassDB::bind_method(D_METHOD("get_ad_units"), &MonetizationSettings::get_ad_units);
}

void MonetizationSettings::delete_ad_unit(StringName p_ad_unit) {
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict.get(p_ad_unit, Dictionary());
	ad_unit_ids.erase(p_ad_unit);
	dict[p_ad_unit] = ad_unit_ids;
	project_settings->set(ad_unit_setting, dict);
}

Array MonetizationSettings::get_ad_units() const {
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	return dict.keys();
}

AdServer::AdType MonetizationSettings::get_ad_type(StringName p_ad_unit) const {
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict.get(p_ad_unit, Dictionary());
	return (AdServer::AdType)(int)ad_unit_ids.get("ad_unit_type", AdServer::AD_TYPE_BANNER);
}

String MonetizationSettings::get_ad_unit_network_id(StringName p_ad_unit, String p_network) {
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict[p_ad_unit];
	return ad_unit_ids.get(p_network, "");
}

void MonetizationSettings::set_ad_unit_network_id(StringName p_ad_unit, String p_network, String p_id) {
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict.get(p_ad_unit, Dictionary());
	ad_unit_ids[p_network] = p_id;
	dict[p_ad_unit] = ad_unit_ids;
	project_settings->set(ad_unit_setting, dict);
}
