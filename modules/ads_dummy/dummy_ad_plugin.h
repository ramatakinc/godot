#ifndef _DUMMY_AD_PLUGIN_H
#define _DUMMY_AD_PLUGIN_H

#include "servers/ramatak/ad_plugin.h"
#include "core/hash_map.h"

class DummyAdPlugin : public AdPlugin {
	GDCLASS(DummyAdPlugin, AdPlugin)

	HashMap<String, Array> ad_unit_request_tokens;

public:
	virtual AdPluginResult init_plugin();

	virtual AdPluginResult show_banner(String p_ad_unit, Variant p_request_token, String p_size, String p_location);
	virtual AdPluginResult show_other(String p_ad_unit, Variant p_request_token);
	virtual AdPluginResult hide(String p_ad_unit, Variant p_request_token);

	virtual AdPluginResult hide_all(Variant p_request_token);

	virtual Array get_shown_ad_units();

	DummyAdPlugin();
	virtual ~DummyAdPlugin();
};

#endif // _DUMMY_AD_PLUGIN_H