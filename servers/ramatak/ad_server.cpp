#include "ad_server.h"

#include "core/project_settings.h"
#include "core/ramatak/monetization_settings.h"
#include "servers/ramatak/ad_plugin.h"

AdServer *AdServer::singleton = nullptr;

void AdServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_available_plugins"), &AdServer::get_available_plugins);

	ClassDB::bind_method(D_METHOD("set_plugin_priority_order", "priorities"), &AdServer::set_plugin_priority_order);
	ClassDB::bind_method(D_METHOD("get_plugin_priority_order"), &AdServer::get_plugin_priority_order);

	ClassDB::bind_method(D_METHOD("_ad_loaded", "request_token"), &AdServer::_ad_loaded);
	ClassDB::bind_method(D_METHOD("_ad_shown", "request_token"), &AdServer::_ad_shown);
	ClassDB::bind_method(D_METHOD("_ad_clicked", "request_token"), &AdServer::_ad_clicked);
	ClassDB::bind_method(D_METHOD("_ad_closed", "request_token"), &AdServer::_ad_closed);
	ClassDB::bind_method(D_METHOD("_ad_reward_earned", "request_token"), &AdServer::_ad_reward_earned);
	ClassDB::bind_method(D_METHOD("_ad_error", "request_token", "message"), &AdServer::_ad_error);

	ADD_SIGNAL(MethodInfo("ad_loaded", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_shown", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_closed", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_clicked", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_reward_earned", PropertyInfo(Variant::INT, "request_id")));
	ADD_SIGNAL(MethodInfo("ad_error", PropertyInfo(Variant::INT, "request_id"), PropertyInfo(Variant::STRING, "message")));

	BIND_ENUM_CONSTANT(AdType::AD_TYPE_BANNER);
	BIND_ENUM_CONSTANT(AdType::AD_TYPE_INTERSTITIAL);
	BIND_ENUM_CONSTANT(AdType::AD_TYPE_REWARDED);

	BIND_ENUM_CONSTANT(BannerAdSize::BANNER_SIZE_SMALL);
	BIND_ENUM_CONSTANT(BannerAdSize::BANNER_SIZE_LARGE);

	BIND_ENUM_CONSTANT(BannerAdLocation::BANNER_LOCATION_BOTTOM);
	BIND_ENUM_CONSTANT(BannerAdLocation::BANNER_LOCATION_TOP);
}

void AdServer::register_ad_plugin(String p_name, Ref<AdPlugin> p_plugin) {
	if (!this->ad_plugins.has(p_name)) {
		this->ad_plugin_list.push_back(p_name);
	}
	this->ad_plugins[p_name] = p_plugin;
}

void AdServer::initialize_modules() {
	for (int i = 0; i < ad_plugin_list.size(); i++) {
		String plugin_name = ad_plugin_list[i];
		Ref<AdPlugin> plugin = this->ad_plugins[plugin_name];
		if (plugin.is_null()) {
			WARN_PRINT_ONCE(vformat("Plugin is null during initialization: %s", plugin_name));
			continue;
		}
		AdPlugin::AdPluginResult result = plugin->init_plugin();
		switch (result) {
			case AdPlugin::AdPluginResult::OK:
				break;
			case AdPlugin::AdPluginResult::PLUGIN_NOT_ENABLED_WARN:
				WARN_PRINT(vformat("Plugin not enabled, but requested: %s", plugin_name));
				return;
			case AdPlugin::AdPluginResult::AUTH_ERROR:
			case AdPlugin::AdPluginResult::NETWORK_ERROR:
			case AdPlugin::AdPluginResult::UNSPECIFIED_ERROR:
				WARN_PRINT(vformat("Plugin initialization failed: %s", plugin_name));
				break;
		}
	}
}

AdServer *AdServer::get_singleton() {
	return AdServer::singleton;
}

Array AdServer::get_available_plugins() const {
	return this->ad_plugin_list;
}

Ref<AdPlugin> AdServer::get_plugin_raw(String p_name) {
	return this->ad_plugins.get(p_name);
}

Array AdServer::get_plugin_priority_order() const {
	return ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
}

void AdServer::set_plugin_priority_order(Array p_priorites) {
	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_plugin_priorities", p_priorites);
	ProjectSettings::get_singleton()->save();
}

Dictionary AdServer::get_plugin_config(String p_plugin) const {
	Dictionary all_configs = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_config");
	return all_configs[p_plugin];
}

Variant AdServer::show_other(String p_ad_unit) {
	Array ad_plugin_priorities = (Array)ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
	String ad_unit_setting = "ramatak/monetization/ad_units";
	ProjectSettings *project_settings = ProjectSettings::get_singleton();

	Dictionary dict = project_settings->get(ad_unit_setting);
	Dictionary ad_unit_config = dict[p_ad_unit];
	int ad_type = (int)ad_unit_config["ad_unit_type"];

	for (int i = 0; i < ad_plugin_priorities.size(); i++) {
		String network_specific_id = MonetizationSettings::get_singleton()->get_ad_unit_network_id(p_ad_unit, ad_plugin_priorities[i]);
		if (network_specific_id.length() > 0) {
			// Plugin `i` can handle the specified ad_unit.
			Variant request_token = rand.rand() & 0x7FFFFFFF;
			ad_plugins[ad_plugin_priorities[i]]->show_other(network_specific_id, request_token, (AdServer::AdType)ad_type);
			return request_token;
		}
	}
	WARN_PRINT_ONCE("No ad plugin is capable of handling ad unit.");
	// There might be a better return value here.
	return Variant();
}

Variant AdServer::show_banner(String p_ad_unit, BannerAdSize p_size, BannerAdLocation p_location) {
	// This looks gross, and it is, but these values are going to cross an FFI boundary into a plugin and
	// it's much less error-prone to pass a string than an integer, especially because these plugins might
	// not even be first-party.
	String size;
	if (p_size == BannerAdSize::BANNER_SIZE_SMALL) {
		size = "small";
	} else if (p_size == BannerAdSize::BANNER_SIZE_LARGE) {
		size = "large";
	} else {
		ERR_FAIL_V_MSG(Variant(), "Ad size not handled");
	}
	String location;
	if (p_location == BannerAdLocation::BANNER_LOCATION_BOTTOM) {
		location = "bottom";
	} else if (p_location == BannerAdLocation::BANNER_LOCATION_TOP) {
		location = "top";
	} else {
		ERR_FAIL_V_MSG(Variant(), "Ad location not handled");
	}
	Array ad_plugin_priorities = (Array)ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
	for (int i = 0; i < ad_plugin_priorities.size(); i++) {
		String network_specific_id = MonetizationSettings::get_singleton()->get_ad_unit_network_id(p_ad_unit, ad_plugin_priorities[i]);
		if (network_specific_id.length() > 0) {
			WARN_PRINT_ONCE(ad_plugin_priorities[i]);
			// Plugin `i` can handle the specified ad_unit.
			Variant request_token = rand.rand() & 0x7FFFFFFF;
			ad_plugins[ad_plugin_priorities[i]]->show_banner(network_specific_id, request_token, size, location);
			return request_token;
		}
	}
	WARN_PRINT_ONCE("No ad plugin is capable of handling ad unit.");
	return Variant();
}

Variant AdServer::hide(String p_ad_unit) {
	Array ad_plugin_priorities = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_priorities");
	Variant request_token = rand.rand();
	// For now just send a hide request to every ad plugin that can handle this ad unit.
	for (int i = 0; i < ad_plugin_priorities.size(); i++) {
		String network_specific_id = MonetizationSettings::get_singleton()->get_ad_unit_network_id(p_ad_unit, ad_plugin_priorities[i]);
		if (network_specific_id.length() > 0) {
			// Plugin `i` can handle the specified ad_unit.
			ad_plugins[ad_plugin_priorities[i]]->hide(network_specific_id, request_token);
		}
	}
	return request_token;
}

void AdServer::_ad_loaded(Variant p_request_token) {
	emit_signal("ad_loaded", p_request_token);
}

void AdServer::_ad_shown(Variant p_request_token) {
	emit_signal("ad_shown", p_request_token);
}

void AdServer::_ad_clicked(Variant p_request_token) {
	emit_signal("ad_clicked", p_request_token);
}

void AdServer::_ad_closed(Variant p_request_token) {
	emit_signal("ad_closed", p_request_token);
}

void AdServer::_ad_reward_earned(Variant p_request_token) {
	emit_signal("ad_reward_earned", p_request_token);
}

void AdServer::_ad_error(Variant p_request_token, Variant message) {
	emit_signal("ad_error", p_request_token, message);
}

AdServer::AdServer() :
		ad_request_counter(0) {
	AdServer::singleton = this;
}

AdServer::~AdServer() {
	AdServer::singleton = nullptr;
}