
#ifndef _AD_CONTROLLER_H
#define _AD_CONTROLLER_H

#include "scene/main/node.h"
#include "core/variant.h"

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
	String ad_unit;

	static void	_bind_methods();
	void _notification(int p_what);

public:
	void set_ad_unit(String p_ad_unit);
	String get_ad_unit() const;

	void show();
	void hide();

	void _ad_loaded(Variant p_request_token);
	void _ad_shown(Variant p_request_token);
	void _ad_clicked(Variant p_request_token);
	void _ad_closed(Variant p_request_token);
	void _ad_reward_earned(Variant p_request_token);
	void _ad_error(Variant p_request_token, Variant p_message);

	AdController();
};

#endif // _AD_CONTROLLER_H
