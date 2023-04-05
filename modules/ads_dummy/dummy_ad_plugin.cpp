#include "dummy_ad_plugin.h"
#include "core/variant/typed_array.h"

AdPlugin::AdPluginResult DummyAdPlugin::init_plugin(String p_api_key) {
	print_line("[Dummy ad plugin] Initializing.");
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::show(StringName p_ad_unit, Variant p_request_token) {
	print_line(vformat("[Dummy ad plugin] Showing ad unit `%s`", p_ad_unit));
	Variant ad_unit_variant = p_ad_unit;
	Variant *args[] = { &ad_unit_variant, &p_request_token };
	emit_signal("loaded", &args, 2);
	emit_signal("shown", &args, 2);
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::hide(StringName p_ad_unit, Variant p_request_token) {
	print_line(vformat("[Dummy ad plugin] Hiding ad unit `%s`", p_ad_unit));
	Variant ad_unit_variant = p_ad_unit;
	Variant *args[] = { &ad_unit_variant, &p_request_token };
	emit_signal("hidden", args, 2);
	return AdPluginResult::OK;
}

AdPlugin::AdPluginResult DummyAdPlugin::hide_all(Variant p_request_token) {
	print_line("[Dummy ad plugin] Hiding all ad units.");
	TypedArray<StringName> ad_units;
	for (KeyValue<StringName, TypedArray<Variant>> &E : ad_unit_request_tokens) {
		hide(E.key, p_request_token);
	}
	return AdPluginResult::OK;
}

TypedArray<StringName> DummyAdPlugin::get_shown_ad_units() {
	TypedArray<StringName> ad_units;
	for (KeyValue<StringName, TypedArray<Variant>> &E : ad_unit_request_tokens) {
		ad_units.push_back(E.key);
	}
	return ad_units;
}

DummyAdPlugin::DummyAdPlugin() {}
DummyAdPlugin::~DummyAdPlugin() {}