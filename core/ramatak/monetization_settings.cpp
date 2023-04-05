#include "monetization_settings.h"
#include "core/config/project_settings.h"

MonetizationSettings *MonetizationSettings::singleton = nullptr;

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
	String ad_unit_setting = "ramatak/monetization/ad_units";
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict.get(p_ad_unit, Dictionary());
	ad_unit_ids.erase(p_ad_unit);
	dict[p_ad_unit] = ad_unit_ids;
	project_settings->set(ad_unit_setting, dict);
}

TypedArray<StringName> MonetizationSettings::get_ad_units() const {
	return ad_units;
}

String MonetizationSettings::get_ad_unit_network_id(StringName p_ad_unit, String p_network) {
	String ad_unit_setting = "ramatak/monetization/ad_units";
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict.get(p_ad_unit, Dictionary());
	return ad_unit_ids[p_network];
}

void MonetizationSettings::set_ad_unit_network_id(StringName p_ad_unit, String p_network, String p_id) {
	String ad_unit_setting = "ramatak/monetization/ad_units";
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_ids = dict.get(p_ad_unit, Dictionary());
	ad_unit_ids[p_network] = p_id;
	dict[p_ad_unit] = ad_unit_ids;
	project_settings->set(ad_unit_setting, dict);
}
