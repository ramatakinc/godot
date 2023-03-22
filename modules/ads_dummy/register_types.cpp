
#include "register_types.h"

#include "ad_plugin_dummy.h"
#include "servers/ramatak/ad_server.h"

static Ref<AdPluginDummy> dummy_plugin;

void initialize_ads_dummy_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(AdPluginDummy);
	dummy_plugin.instantiate();
	AdServer::get_singleton()->register_ad_plugin("Dummy", dummy_plugin);
}

void uninitialize_ads_dummy_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
