
#ifndef _BANNER_AD_CONTROLLER_H
#define _BANNER_AD_CONTROLLER_H

#include "ad_controller.h"
#include "servers/ramatak/ad_server.h"

class BannerAdController : public AdController {
	GDCLASS(BannerAdController, AdController);

	bool auto_show;
	AdServer::BannerAdLocation location;
	AdServer::BannerAdSize size;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	BannerAdController();

	bool get_auto_show() const;
	void set_auto_show(bool p_auto_show);

	void show();

	AdServer::BannerAdLocation get_location() const;
	void set_location(AdServer::BannerAdLocation p_location);

	AdServer::BannerAdSize get_size() const;
	void set_size(AdServer::BannerAdSize p_size);
};

#endif // _BANNER_AD_CONTROLLER_H
