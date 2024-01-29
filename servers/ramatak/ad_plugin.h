#ifndef _AD_PLUGIN_H
#define _AD_PLUGIN_H

#include "core/class_db.h"
#include "core/math/rect2.h"
#include "core/reference.h"
#include "servers/ramatak/ad_server.h"

// If you add settings here you must also add them to the preset settings in EditorExportPlatformIOS::get_export_options and  EditorExportPlatformAndroid::get_export_options
#define AD_PLUGIN_PRIORITIES_KEY "ramatak/monetization/ad_plugin_priorities"
#define AD_DEBUG_MODE_KEY "ramatak/monetization/debug_mode"
#define AD_CHILD_DIRECTED_KEY "ramatak/monetization/child_directed"

class AdPlugin : public Reference {
	GDCLASS(AdPlugin, Reference);

protected:
	static void _bind_methods();

	bool _get_debug_mode() const;
	bool _get_child_directed() const;

public:
	enum AdPluginResult {
		OK,
		PLUGIN_NOT_ENABLED_WARN,
		AUTH_ERROR,
		NETWORK_ERROR,
		UNSPECIFIED_ERROR,
	};

	virtual AdPluginResult init_plugin() = 0;

	virtual Array get_config_key_tooltip_name_triples() = 0;
	virtual String get_friendly_name() = 0;

	virtual AdPluginResult show_other(String p_ad_unit, Variant p_request_token, AdServer::AdType p_ad_type) = 0;
	virtual AdPluginResult show_banner(String p_ad_unit, Variant p_request_token, String p_size, String p_location) = 0;
	virtual AdPluginResult hide(String p_ad_unit, Variant p_request_token) = 0;

	virtual AdPluginResult hide_all(Variant p_request_token) = 0;

	virtual Array get_shown_ad_units() = 0;
	// virtual Rect2i get_safe_zone() = 0;

	virtual String get_android_plugin_config() = 0;
};

VARIANT_ENUM_CAST(AdPlugin::AdPluginResult);

#endif // _AD_PLUGIN_H