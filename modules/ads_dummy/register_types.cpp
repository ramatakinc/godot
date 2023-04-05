
#include "register_types.h"

#include "dummy_ad_plugin.h"
#include "servers/ramatak/ad_server.h"

static Ref<DummyAdPlugin> dummy_plugin;

void initialize_ads_dummy_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(DummyAdPlugin);
	dummy_plugin.instantiate();
	AdServer::get_singleton()->register_ad_plugin("DUMMY", dummy_plugin);
}

void uninitialize_ads_dummy_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
