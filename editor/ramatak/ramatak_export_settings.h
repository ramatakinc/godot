#ifndef _RAMATAK_SETTINGS_EDITOR
#define _RAMATAK_SETTINGS_EDITOR

#include "scene/gui/control.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tree.h"

class HBoxContainer;
class VBoxContainer;
class GridContainer;
class OptionButton;
class LineEdit;
class Button;
class Control;
class CheckBox;
class ItemList;
class Label;
class EditorExportPreset;

class RamatakExportSettingsEditor : public Control {
	GDCLASS(RamatakExportSettingsEditor, Control);

	VBoxContainer *main_vbox = nullptr;
	HBoxContainer *priorities_hbox = nullptr;

	CheckBox *debug_checkbox = nullptr;
	CheckBox *child_directed_checkbox = nullptr;

	Label *status_label = nullptr;

	ItemList *disabled_plugins_list = nullptr;
	ItemList *enabled_plugins_list = nullptr;

	VBoxContainer *button_row = nullptr;
	Button *increase_prioity = nullptr;
	Button *decrease_prioity = nullptr;
	Button *enable_plugin = nullptr;
	Button *disable_plugin = nullptr;

	Ref<EditorExportPreset> preset;

	int get_selected_index(ItemList *p_list);

	void persist_settings();

protected:
	static void _bind_methods();

	void update_status_label();

	void _notification(int p_what);

	void _increase_selected_priority();
	void _decrease_selected_priority();

	void _enable_selected_plugin();
	void _disable_selected_plugin();

	void _enabled_plugins_list_item_selected(int p_index);
	void _disabled_plugins_list_item_selected(int p_index);

	void _debug_checkbox_toggled(bool p_pressed);
	void _child_directed_checkbox_toggled(bool p_pressed);

public:
	void set_preset(Ref<EditorExportPreset> p_preset);

	Control *get_status_label();

	RamatakExportSettingsEditor();
};

#endif