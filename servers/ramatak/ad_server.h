#ifndef _AD_SERVER_H
#define _AD_SERVER_H

#include "core/class_db.h"
#include "core/object.h"
#include "core/list.h"
#include "core/reference.h"
#include "core/math/random_pcg.h"

class AdPlugin;

class AdServer : public Object {
	GDCLASS(AdServer, Object);

public:
	// Add new ad types to the end of this list before *_MAX to avoid messing up existing projects.
	enum AdType {
		AD_TYPE_BANNER,
		AD_TYPE_INTERSTITIAL,
		AD_TYPE_REWARDED,
		AD_TYPE_MAX,
	};

	enum BannerAdSize {
		BANNER_SIZE_SMALL,
		BANNER_SIZE_LARGE,
	};

	enum BannerAdLocation {
		BANNER_LOCATION_BOTTOM,
		BANNER_LOCATION_TOP,
	};

private:
	static AdServer *singleton;

	RandomPCG rand;

	HashMap<String, Ref<AdPlugin>> ad_plugins;
	Array ad_plugin_list;

	int64_t ad_request_counter;

protected:
	static void _bind_methods();

public:
	static AdServer *get_singleton();

	void register_ad_plugin(String p_name, Ref<AdPlugin> p_plugin);

	Array get_available_plugins() const;

	Array get_plugin_priority_order() const;
	void set_plugin_priority_order(Array p_priorites);

	Dictionary get_plugin_config(String p_plugin) const;

	Variant show_banner(String p_ad_unit, BannerAdSize p_size, BannerAdLocation p_location);
	Variant show_other(String p_ad_unit);
	Variant hide(String p_ad_unit);

	AdServer();
	virtual ~AdServer();
};

VARIANT_ENUM_CAST(AdServer::AdType)
VARIANT_ENUM_CAST(AdServer::BannerAdSize)
VARIANT_ENUM_CAST(AdServer::BannerAdLocation)

#endif // _AD_SERVER_H