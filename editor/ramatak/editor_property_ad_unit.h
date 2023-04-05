#ifndef _EDITOR_PROPERTY_AD_UNIT
#define _EDITOR_PROPERTY_AD_UNIT

#include "editor/editor_inspector.h"

class EditorPropertyAdUnit : public EditorProperty {
	GDCLASS(EditorPropertyAdUnit, EditorProperty);
	OptionButton *options = nullptr;

	void _option_selected(int p_which);
	void _refresh_options();

protected:
	virtual void _set_read_only(bool p_read_only) override;
	static void _bind_methods();

public:
	void setup();
	virtual void update_property() override;
	void set_option_button_clip(bool p_enable);
	EditorPropertyAdUnit();
};

#endif // _EDITOR_PROPERTY_AD_UNIT