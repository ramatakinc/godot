#include "register_types.h"

#include "dummy_ad_plugin.h"
#include "core/class_db.h"
#include "servers/ramatak/ad_server.h"

static Ref<DummyAdPlugin> dummy_plugin;

void register_ads_dummy_types() {
	ClassDB::register_virtual_class<DummyAdPlugin>();
	dummy_plugin.instance();
	AdServer::get_singleton()->register_ad_plugin("DUMMY", dummy_plugin);
}

void unregister_ads_dummy_types() {
}
