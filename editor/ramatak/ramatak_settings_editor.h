#ifndef _RAMATAK_SETTINGS_EDITOR
#define _RAMATAK_SETTINGS_EDITOR

#include "scene/gui/control.h"

class HBoxContainer;
class VBoxContainer;
class GridContainer;
class OptionButton;
class LineEdit;
class Button;
class ItemList;
class Label;

class RamatakSettingsAdUnitEditor : public Control {
	GDCLASS(RamatakSettingsAdUnitEditor, Control);

	GridContainer *grid_container;

	OptionButton* ad_unit_type_option_button;
	List<String> plugins;
	List<Label *> labels;
	List<LineEdit *> edits;

	String ad_unit;

	void _ad_unit_edited(Variant p_arg);

protected:
	static void _bind_methods();

public:
	RamatakSettingsAdUnitEditor();

	void set_disabled(bool p_disabled);
	void clear();

	Dictionary get_ad_unit_config() const;
	void set_ad_unit(const String &p_ad_unit);
	String get_ad_unit() const;
};

class RamatakSettingsEditor : public Control {
	GDCLASS(RamatakSettingsEditor, Control);

	Dictionary ad_units_config;

	VBoxContainer *main_vbox = nullptr;
	HBoxContainer *add_hbox = nullptr;
	HBoxContainer *edit_hbox = nullptr;
	HBoxContainer *save_row_hbox = nullptr;

	Label *add_label = nullptr;
	LineEdit *add_edit = nullptr;
	Button *add_button = nullptr;

	ItemList *edit_items_list = nullptr;
	RamatakSettingsAdUnitEditor *ad_unit_editor = nullptr;

	Button *save_button = nullptr;
	Button *revert_button = nullptr;

	void _add_edit_text_changed(const String &p_text);
	void _edit_items_list_item_selected(int p_index);
	void _ad_unit_edited();
	void _add_ad_unit();
	void _revert();
	void _save();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	RamatakSettingsEditor();
	// virtual ~RamatakSettingsEditor();
};

#endif