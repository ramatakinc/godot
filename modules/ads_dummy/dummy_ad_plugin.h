#ifndef _DUMMY_AD_PLUGIN_H
#define _DUMMY_AD_PLUGIN_H

#include "core/hash_map.h"
#include "servers/ramatak/ad_plugin.h"
#include "servers/ramatak/ad_server.h"

class DummyAdPlugin : public AdPlugin {
	GDCLASS(DummyAdPlugin, AdPlugin)

	HashMap<String, Array> ad_unit_request_tokens;

public:
	virtual AdPluginResult init_plugin();

	virtual Array get_config_key_tooltip_pairs();
	virtual String get_friendly_name();

	virtual AdPluginResult show_banner(String p_ad_unit, Variant p_request_token, String p_size, String p_location);
	virtual AdPluginResult show_other(String p_ad_unit, Variant p_request_token, AdServer::AdType p_ad_type);
	virtual AdPluginResult hide(String p_ad_unit, Variant p_request_token);

	virtual AdPluginResult hide_all(Variant p_request_token);

	virtual Array get_shown_ad_units();

	virtual String get_android_plugin_config();

	DummyAdPlugin();
	virtual ~DummyAdPlugin();
};

#endif // _DUMMY_AD_PLUGIN_H