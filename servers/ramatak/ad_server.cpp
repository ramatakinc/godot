#include "ad_server.h"

#include "core/config/project_settings.h"
#include "core/ramatak/monetization_settings.h"

AdServer *AdServer::singleton = nullptr;

void AdServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_available_plugins"), &AdServer::get_available_plugins);

	ClassDB::bind_method(D_METHOD("set_plugin_priority_order", "priorities"), &AdServer::set_plugin_priority_order);
	ClassDB::bind_method(D_METHOD("get_plugin_priority_order"), &AdServer::get_plugin_priority_order);
}

void AdServer::register_ad_plugin(StringName p_name, Ref<AdPlugin> p_plugin) {
	bool have_api_key = false;
	Variant api_key = ProjectSettings::get_singleton()->get(vformat("ad_plugin/%s/api_key", p_name), &have_api_key);
	if (have_api_key) {
		p_plugin->init_plugin(api_key);
	} else {
		p_plugin->init_plugin("");
	}
	if (this->ad_plugins.find(p_name) == nullptr) {
		this->ad_plugin_list.push_back(p_name);
	}
	this->ad_plugins[p_name] = p_plugin;
}

AdServer *AdServer::get_singleton() {
	return AdServer::singleton;
}

TypedArray<StringName> AdServer::get_available_plugins() const {
	return this->ad_plugin_list;
}

TypedArray<StringName> AdServer::get_plugin_priority_order() const {
	return ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
}

void AdServer::set_plugin_priority_order(TypedArray<StringName> p_priorites) {
	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_plugin_priorities", p_priorites);
}

Variant AdServer::show(StringName p_ad_unit) {
	TypedArray<StringName> ad_plugin_priorities = (TypedArray<StringName>)ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
	// There might be a better return value here.
	for (int i = 0; i < ad_plugin_priorities.size(); i++) {
		String network_specific_id = MonetizationSettings::get_singleton()->get_ad_unit_network_id(p_ad_unit, ad_plugin_priorities[i]);
		if (network_specific_id.length() > 0) {
			// Plugin `i` can handle the specified StringName.
			Variant request_token = rand.rand();
			ad_plugins[ad_plugin_priorities[i]]->show(network_specific_id, request_token);
			return request_token;
		}
	}
	WARN_PRINT_ONCE("No ad plugin is capable of handling ad unit.");
	return "";
}

Variant AdServer::hide(StringName p_ad_unit) {
	TypedArray<StringName> ad_plugin_priorities = (TypedArray<StringName>)ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
	// For now just send a hide request to every ad plugin that can handle this ad unit.
	for (int i = 0; i < ad_plugin_priorities.size(); i++) {
		String network_specific_id = MonetizationSettings::get_singleton()->get_ad_unit_network_id(p_ad_unit, ad_plugin_priorities[i]);
		if (network_specific_id.length() > 0) {
			// Plugin `i` can handle the specified StringName.
			Variant request_token = rand.rand();
			ad_plugins[ad_plugin_priorities[i]]->hide(network_specific_id, request_token);
			return request_token;
		}
	}
	WARN_PRINT_ONCE("No ad plugin is capable of handling ad unit.");
	return "";
}

AdServer::AdServer() :
		ad_request_counter(0) {
	AdServer::singleton = this;
}

AdServer::~AdServer() {
	AdServer::singleton = nullptr;
}