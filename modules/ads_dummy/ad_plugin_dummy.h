#ifndef _AD_PLUGIN_DUMMY_H
#define _AD_PLUGIN_DUMMY_H

#include "servers/ramatak/ad_plugin.h"

class AdPluginDummy : public AdPlugin {
	GDCLASS(AdPluginDummy, AdPlugin)
public:
	AdPluginDummy();
	virtual ~AdPluginDummy();
};

#endif // _AD_PLUGIN_DUMMY_H