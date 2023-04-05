#ifndef _DUMMY_AD_PLUGIN_H
#define _DUMMY_AD_PLUGIN_H

#include "servers/ramatak/ad_plugin.h"

class DummyAdPlugin : public AdPlugin {
	GDCLASS(DummyAdPlugin, AdPlugin)

	HashMap<StringName, TypedArray<Variant>> ad_unit_request_tokens;

public:
	virtual AdPluginResult init_plugin(String p_api_key);

	virtual AdPluginResult show(StringName p_ad_unit, Variant p_request_token);
	virtual AdPluginResult hide(StringName p_ad_unit, Variant p_request_token);

	virtual AdPluginResult hide_all(Variant p_request_token);

	virtual TypedArray<StringName> get_shown_ad_units();

	DummyAdPlugin();
	virtual ~DummyAdPlugin();
};

#endif // _DUMMY_AD_PLUGIN_H