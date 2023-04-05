#ifndef _MONETIZATION_SETTINGS_H
#define _MONETIZATION_SETTINGS_H

#include "core/object/class_db.h"
#include "core/object/object.h"
#include "servers/ramatak/ad_server.h"

class MonetizationSettings : public Object {
	GDCLASS(MonetizationSettings, Object);

	TypedArray<StringName> ad_units;

	static MonetizationSettings *singleton;

protected:
	static void _bind_methods();

public:
	static MonetizationSettings *get_singleton();

	void delete_ad_unit(StringName p_ad_unit);

	void set_ad_unit_network_id(StringName p_ad_unit, String p_network, String p_id);
	String get_ad_unit_network_id(StringName p_ad_unit, String p_network);

	TypedArray<StringName> get_ad_units() const;

	MonetizationSettings();
	virtual ~MonetizationSettings();
};

#endif // _MONETIZATION_SETTINGS