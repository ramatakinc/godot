
#ifndef _AD_CONTROLLER_H
#define _AD_CONTROLLER_H

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

	String ad_unit;

	static void	_bind_methods();
	void _notification(int p_what);

public:
	void set_ad_unit(String p_ad_unit);
	String get_ad_unit() const;

	void show();
	void hide();

	AdController();
};

#endif // _AD_CONTROLLER_H
