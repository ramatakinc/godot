
#include "register_types.h"

#include "ad_controller.h"
#include "banner_ad_controller.h"
#include "servers/ramatak/ad_server.h"

void register_ads_types() {
	ClassDB::register_class<AdController>();
	ClassDB::register_class<BannerAdController>();
}

void unregister_ads_types() {
}
