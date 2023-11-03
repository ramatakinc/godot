#include "dummy_ad_plugin.h"
#include "core/array.h"

AdPlugin::AdPluginResult DummyAdPlugin::init_plugin() {
	print_line("[Dummy ad plugin] Initializing.");
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::show_banner(String p_ad_unit, Variant p_request_token, String p_size, String p_location) {
	print_line(vformat("[Dummy ad plugin] Showing banner ad unit `%s`", p_ad_unit));
	Variant ad_unit_variant = p_ad_unit;
	Variant *args[] = { &ad_unit_variant, &p_request_token };
	emit_signal("loaded", &args, 2);
	emit_signal("shown", &args, 2);
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::show_other(String p_ad_unit, Variant p_request_token, AdServer::AdType p_ad_type) {
	print_line(vformat("[Dummy ad plugin] Showing other ad unit `%s`", p_ad_unit));
	Variant ad_unit_variant = p_ad_unit;
	Variant *args[] = { &ad_unit_variant, &p_request_token };
	emit_signal("loaded", &args, 2);
	emit_signal("shown", &args, 2);
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::hide(String p_ad_unit, Variant p_request_token) {
	print_line(vformat("[Dummy ad plugin] Hiding ad unit `%s`", p_ad_unit));
	Variant ad_unit_variant = p_ad_unit;
	Variant *args[] = { &ad_unit_variant, &p_request_token };
	emit_signal("hidden", args, 2);
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::hide_all(Variant p_request_token) {
	print_line("[Dummy ad plugin] Hiding all ad units.");
	Array ad_units;
	List<String> keys;
	ad_unit_request_tokens.get_key_list(&keys);
	for (int i = 0; i < keys.size(); i++) {
		hide(keys[i], p_request_token);
	}
	return AdPluginResult::OK;
}

Array DummyAdPlugin::get_shown_ad_units() {
	Array ad_units;
	List<String> keys;
	ad_unit_request_tokens.get_key_list(&keys);
	for (int i = 0; i < keys.size(); i++) {
		ad_units.push_back(keys[i]);
	}
	return ad_units;
}

Array DummyAdPlugin::get_config_key_tooltip_pairs() {
	Array keys;
	return keys;
}

String DummyAdPlugin::get_friendly_name() {
	return "Dummy";
}

String DummyAdPlugin::get_android_plugin_config() {
	return "";
}

DummyAdPlugin::DummyAdPlugin() {}
DummyAdPlugin::~DummyAdPlugin() {}