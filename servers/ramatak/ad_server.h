#ifndef _AD_SERVER_H
#define _AD_SERVER_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "core/variant/typed_array.h"
#include "servers/ramatak/ad_plugin.h"

class AdServer : public Object {
	GDCLASS(AdServer, Object);

	static AdServer *singleton;

	HashMap<StringName, Ref<AdPlugin>> ad_plugins;
	TypedArray<StringName> ad_plugin_list;
	TypedArray<StringName> ad_plugin_priorities;

protected:
	static void _bind_methods();

public:
	static AdServer *get_singleton();

	void register_ad_plugin(StringName p_name, Ref<AdPlugin> p_plugin);

	TypedArray<StringName> get_available_plugins() const;

	TypedArray<StringName> get_plugin_priority_order() const;
	void set_plugin_priority_order(TypedArray<StringName> p_priorites);

	AdServer();
	virtual ~AdServer();
};

#endif // _AD_SERVER_H