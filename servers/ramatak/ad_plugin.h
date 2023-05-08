#ifndef _AD_PLUGIN_H
#define _AD_PLUGIN_H

#include "core/class_db.h"
#include "core/reference.h"
#include "core/math/rect2.h"

class AdPlugin : public Reference {
	GDCLASS(AdPlugin, Reference);

protected:
	static void _bind_methods();

public:
	enum AdPluginResult {
		OK,
		AUTH_ERROR,
		NETWORK_ERROR,
		UNSPECIFIED_ERROR,
	};

	virtual AdPluginResult init_plugin() = 0;

	virtual AdPluginResult show_other(String p_ad_unit, Variant p_request_token) = 0;
	virtual AdPluginResult show_banner(String p_ad_unit, Variant p_request_token, String p_size, String p_location) = 0;
	virtual AdPluginResult hide(String p_ad_unit, Variant p_request_token) = 0;

	virtual AdPluginResult hide_all(Variant p_request_token) = 0;

	virtual Array get_shown_ad_units() = 0;
	// virtual Rect2i get_safe_zone() = 0;
};

VARIANT_ENUM_CAST(AdPlugin::AdPluginResult);

#endif // _AD_PLUGIN_H