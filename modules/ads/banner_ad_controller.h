
#ifndef _BANNER_AD_CONTROLLER_H
#define _BANNER_AD_CONTROLLER_H

#include "ad_controller.h"

class BannerAdController : public AdController {
	GDCLASS(BannerAdController, AdController);

public:
	enum BannerAdLocation {
		BOTTOM,
		TOP,
	};

	enum BannerAdSize {
		SMALL,
		LARGE,
	};

private:
	bool auto_show;
	BannerAdLocation location;
	BannerAdSize size;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	BannerAdController();

	bool get_auto_show() const;
	void set_auto_show(bool p_auto_show);

	BannerAdLocation get_location() const;
	void set_location(BannerAdLocation p_location);

	BannerAdSize get_size() const;
	void set_size(BannerAdSize p_size);
};

VARIANT_ENUM_CAST(BannerAdController::BannerAdLocation);
VARIANT_ENUM_CAST(BannerAdController::BannerAdSize);

#endif // _BANNER_AD_CONTROLLER_H
