
#include "register_types.h"

#include "ad_controller.h"
#include "banner_ad_controller.h"
#include "servers/ramatak/ad_server.h"

void initialize_ads_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(AdController);
	GDREGISTER_CLASS(BannerAdController);
}

void uninitialize_ads_module(ModuleInitializationLevel p_level) {
}
