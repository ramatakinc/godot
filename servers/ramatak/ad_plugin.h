#ifndef _AD_PLUGIN_H
#define _AD_PLUGIN_H

#include "core/math/rect2i.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"

class AdPlugin : public RefCounted {
	GDCLASS(AdPlugin, RefCounted);

protected:
	static void _bind_methods();

public:
	enum AdPluginResult {
		OK,
		AUTH_ERROR,
		NETWORK_ERROR,
		UNSPECIFIED_ERROR,
	};

	virtual AdPluginResult init_plugin(String p_api_key) = 0;

	virtual AdPluginResult show(StringName p_ad_unit, Variant p_request_token) = 0;
	virtual AdPluginResult hide(StringName p_ad_unit, Variant p_request_token) = 0;

	virtual AdPluginResult hide_all(Variant p_request_token) = 0;

	virtual TypedArray<StringName> get_shown_ad_units() = 0;
	// virtual Rect2i get_safe_zone() = 0;
};

VARIANT_ENUM_CAST(AdPlugin::AdPluginResult);

#endif // _AD_PLUGIN_H