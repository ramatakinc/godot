#ifndef _AD_PLUGIN_H
#define _AD_PLUGIN_H

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"

class AdPlugin : public RefCounted {
	GDCLASS(AdPlugin, RefCounted);

public:
	// Plugin API TBD.
};

#endif // _AD_PLUGIN_H