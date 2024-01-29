
#ifndef _AD_CONTROLLER_H
#define _AD_CONTROLLER_H

#include "core/variant.h"
#include "scene/main/node.h"

class AdController : public Node {
	GDCLASS(AdController, Node);

protected:
	enum Status {
		NOT_SHOWN,
		SHOWING,
		SHOWN,
	};

	Status status;
	Array request_tokens;
	String ad_placement;

	static void _bind_methods();
	void _notification(int p_what);

public:
	virtual void set_ad_placement(String p_ad_placement);
	virtual String get_ad_placement() const;

	virtual void show();
	virtual void hide();

	virtual void _ad_loaded(Variant p_request_token);
	virtual void _ad_shown(Variant p_request_token);
	virtual void _ad_clicked(Variant p_request_token);
	virtual void _ad_closed(Variant p_request_token);
	virtual void _ad_reward_earned(Variant p_request_token);
	virtual void _ad_error(Variant p_request_token, Variant p_message);

	AdController();
};

#endif // _AD_CONTROLLER_H
