#ifndef _EDITOR_PROPERTY_AD_UNIT
#define _EDITOR_PROPERTY_AD_UNIT

#include "editor/editor_inspector.h"
#include "scene/gui/option_button.h"

class EditorPropertyAdUnit : public EditorProperty {
	GDCLASS(EditorPropertyAdUnit, EditorProperty);
	OptionButton *options = nullptr;

	void _option_selected(int p_which);
	void _refresh_options();

protected:
	static void _bind_methods();

public:
	void setup();
	virtual void update_property() override;
	void set_option_button_clip(bool p_enable);
	EditorPropertyAdUnit();
};

#endif // _EDITOR_PROPERTY_AD_UNIT