
#ifndef _AD_CONTROLLER_H
#define _AD_CONTROLLER_H

#include "scene/main/node.h"

class AdController : public Node {
	GDCLASS(AdController, Node);

	String network_alias;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_network(String p_network_alias);
	String get_network() const;

	AdController();
};

#endif // _AD_CONTROLLER_H
