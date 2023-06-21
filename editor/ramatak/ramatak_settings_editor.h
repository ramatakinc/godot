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

	OptionButton *ad_unit_type_option_button;
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

class RamatakSettingsAdUnitSetEditor : public Control {
	GDCLASS(RamatakSettingsAdUnitSetEditor, Control);

	VBoxContainer *main_vbox = nullptr;
	HBoxContainer *add_hbox = nullptr;
	HBoxContainer *edit_hbox = nullptr;

	Label *add_label = nullptr;
	LineEdit *add_edit = nullptr;
	Button *add_button = nullptr;
	Button *remove_button = nullptr;

	ItemList *edit_items_list = nullptr;
	RamatakSettingsAdUnitEditor *ad_unit_editor = nullptr;

	void _add_edit_text_changed(const String &p_text);
	void _edit_items_list_item_selected(int p_index);
	void _ad_unit_edited();
	void _add_ad_unit();
	void _remove_ad_unit();
	void _save();

	friend class RamatakSettingsEditor;

protected:
	static void
	_bind_methods();
	void _notification(int p_what);

public:
	RamatakSettingsAdUnitSetEditor();
};

class RamatakAdPluginSettingsEditor : public Control {
	GDCLASS(RamatakAdPluginSettingsEditor, Control);

	String plugin_id;
	Array plugin_keys;

	VBoxContainer *main_vbox = nullptr;

	GridContainer *grid_container;

	OptionButton *ad_unit_type_option_button;
	List<Label *> labels;
	List<LineEdit *> edits;

	void _edit_items_list_item_selected(int p_index);
	void _setting_edited(Variant p_arg);

protected:
	static void _bind_methods();

	void clear();

public:
	void set_plugin(const String &p_plugin);

	RamatakAdPluginSettingsEditor();
};

class RamatakAdPluginPriorityEditor : public Control {
	GDCLASS(RamatakAdPluginPriorityEditor, Control);

	HBoxContainer *main_hbox = nullptr;

	ItemList *disabled_plugins_list = nullptr;
	ItemList *enabled_plugins_list = nullptr;

	VBoxContainer *button_row = nullptr;
	Button *increase_prioity = nullptr;
	Button *decrease_prioity = nullptr;
	Button *enable_plugin = nullptr;
	Button *disable_plugin = nullptr;

	int get_selected_index(ItemList *p_list);

	void persist_settings();

protected:
	static void _bind_methods();

	void _notification(int p_what);

	void _increase_selected_priority();
	void _decrease_selected_priority();

	void _enable_selected_plugin();
	void _disable_selected_plugin();

public:
	void set_platform(const String &p_platform);

	RamatakAdPluginPriorityEditor();
};

class RamatakSettingsEditor : public Control {
	GDCLASS(RamatakSettingsEditor, Control);

	Array plugins;

	HBoxContainer *main_hbox = nullptr;
	ItemList *edit_items_list = nullptr;
	RamatakSettingsAdUnitSetEditor *ad_unit_set_editor = nullptr;
	RamatakAdPluginPriorityEditor *ad_priority_editor = nullptr;
	RamatakAdPluginSettingsEditor *ad_plugin_settings_editor = nullptr;

	Control *current_pane = nullptr;

	void _edit_items_list_item_selected(int p_index);

protected:
	static void _bind_methods();
	void _notification(int p_what);

	enum ItemListMetadataKeys {
		AD_UNITS,
		PLUGIN_PRIORITIES,
	};

public:
	RamatakSettingsEditor();
};

#endif