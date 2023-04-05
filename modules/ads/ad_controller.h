
#ifndef _AD_CONTROLLER_H
#define _AD_CONTROLLER_H

#include "scene/main/node.h"

class AdController : public Node {
	GDCLASS(AdController, Node);

	String network_alias;

	enum Status {
		NOT_SHOWN,
		SHOWING,
		SHOWN,
	};

	Status status;

	StringName ad_unit;

protected:
	static void
	_bind_methods();
	void _notification(int p_what);

public:
	void set_network(String p_network_alias);
	String get_network() const;

	void set_ad_unit(StringName p_ad_unit);
	StringName get_ad_unit() const;

	void show();
	void hide();

	AdController();
};

#endif // _AD_CONTROLLER_H
