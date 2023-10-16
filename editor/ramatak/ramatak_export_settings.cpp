#include "ramatak_export_settings.h"

#include "core/project_settings.h"
#include "core/ustring.h"
#include "editor/editor_export.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "servers/ramatak/ad_plugin.h"
#include "servers/ramatak/ad_server.h"

RamatakExportSettingsEditor::RamatakExportSettingsEditor() {
	main_hbox = memnew(HBoxContainer);
	main_hbox->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	add_child(main_hbox);

	HSplitContainer *hsplit = memnew(HSplitContainer);
	hsplit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_hbox->add_child(hsplit);

	VBoxContainer *disabled_plugins_vbox = memnew(VBoxContainer);
	disabled_plugins_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list = memnew(ItemList);
	disabled_plugins_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	Label *disabled_plugins_label = memnew(Label);
	disabled_plugins_label->set_text("Disabled Plugins:");

	disabled_plugins_vbox->add_child(disabled_plugins_label);
	disabled_plugins_vbox->add_child(disabled_plugins_list);
	hsplit->add_child(disabled_plugins_vbox);

	VBoxContainer *enabled_plugins_vbox = memnew(VBoxContainer);
	enabled_plugins_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list = memnew(ItemList);
	enabled_plugins_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	Label *enabled_plugins_label = memnew(Label);
	enabled_plugins_label->set_text("Enabled Plugins:");

	enabled_plugins_vbox->add_child(enabled_plugins_label);
	enabled_plugins_vbox->add_child(enabled_plugins_list);
	hsplit->add_child(enabled_plugins_vbox);

	button_row = memnew(VBoxContainer);
	main_hbox->add_child(button_row);

	// Make buttons align with the plugin lists.
	button_row->add_child(memnew(Label(" ")));

	increase_prioity = memnew(Button);
	increase_prioity->set_text("Increase Priority");
	increase_prioity->connect("pressed", this, "_increase_selected_priority");
	button_row->add_child(increase_prioity);

	decrease_prioity = memnew(Button);
	decrease_prioity->set_text("Decrease Priority");
	decrease_prioity->connect("pressed", this, "_decrease_selected_priority");
	button_row->add_child(decrease_prioity);

	enable_plugin = memnew(Button);
	enable_plugin->set_text("Enable Plugin");
	enable_plugin->connect("pressed", this, "_enable_selected_plugin");
	button_row->add_child(enable_plugin);

	disable_plugin = memnew(Button);
	disable_plugin->set_text("Disable Plugin");
	disable_plugin->connect("pressed", this, "_disable_selected_plugin");
	button_row->add_child(disable_plugin);
}

void RamatakExportSettingsEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		Array all_plugins = AdServer::get_singleton()->get_available_plugins();
		Array priority_order = preset->get("ramatak/monetization/ad_plugin_priorities");
		for (int i = 0; i < priority_order.size(); i++) {
			enabled_plugins_list->add_item(AdServer::get_singleton()->get_plugin_raw(priority_order[i])->get_friendly_name());
			enabled_plugins_list->set_item_metadata(i, (String)priority_order[i]);
		}
		for (int i = 0; i < all_plugins.size(); i++) {
			if (priority_order.find(all_plugins[i]) == -1) {
				disabled_plugins_list->add_item(AdServer::get_singleton()->get_plugin_raw(all_plugins[i])->get_friendly_name());
				disabled_plugins_list->set_item_metadata(disabled_plugins_list->get_item_count() - 1, (String)all_plugins[i]);
			}
		}
	}
}

void RamatakExportSettingsEditor::_bind_methods() {
	ClassDB::bind_method("_increase_selected_priority", &RamatakExportSettingsEditor::_increase_selected_priority);
	ClassDB::bind_method("_decrease_selected_priority", &RamatakExportSettingsEditor::_decrease_selected_priority);
	ClassDB::bind_method("_enable_selected_plugin", &RamatakExportSettingsEditor::_enable_selected_plugin);
	ClassDB::bind_method("_disable_selected_plugin", &RamatakExportSettingsEditor::_disable_selected_plugin);
}

int RamatakExportSettingsEditor::get_selected_index(ItemList *p_list) {
	Vector<int> selected = p_list->get_selected_items();
	enabled_plugins_list->unselect_all();
	if (selected.size() > 1) {
		WARN_PRINT_ONCE("Multi-select is not enabled but multiple items are selected");
		return -1;
	} else if (selected.empty()) {
		return -1;
	}
	return selected[0];
}

void RamatakExportSettingsEditor::persist_settings() {
	Array priorities;
	for (int i = 0; i < enabled_plugins_list->get_item_count(); i++) {
		String id = enabled_plugins_list->get_item_metadata(i);
		priorities.push_back(id);
	}
	preset->set("ramatak/monetization/ad_plugin_priorities", priorities);
}

void RamatakExportSettingsEditor::_increase_selected_priority() {
	int selected_index = get_selected_index(enabled_plugins_list);
	if (selected_index == -1) {
		return;
	}
	if (selected_index == 0) {
		return; // Can't move it up any more.
	}
	enabled_plugins_list->move_item(selected_index, selected_index - 1);
	enabled_plugins_list->select(selected_index - 1);
	persist_settings();
}

void RamatakExportSettingsEditor::_decrease_selected_priority() {
	int selected_index = get_selected_index(enabled_plugins_list);
	if (selected_index == -1) {
		return;
	}
	if (selected_index == enabled_plugins_list->get_item_count() - 1) {
		return; // Can't move it down any more.
	}
	enabled_plugins_list->move_item(selected_index, selected_index + 1);
	enabled_plugins_list->select(selected_index + 1);
	persist_settings();
}

void RamatakExportSettingsEditor::_enable_selected_plugin() {
	int selected_index = get_selected_index(disabled_plugins_list);
	if (selected_index == -1) {
		return;
	}
	String plugin_id = disabled_plugins_list->get_item_metadata(selected_index);
	disabled_plugins_list->remove_item(selected_index);
	enabled_plugins_list->add_item(AdServer::get_singleton()->get_plugin_raw(plugin_id)->get_friendly_name());
	enabled_plugins_list->set_item_metadata(enabled_plugins_list->get_item_count() - 1, plugin_id);
	persist_settings();
}

void RamatakExportSettingsEditor::_disable_selected_plugin() {
	int selected_index = get_selected_index(enabled_plugins_list);
	if (selected_index == -1) {
		return;
	}
	String plugin_id = enabled_plugins_list->get_item_metadata(selected_index);
	enabled_plugins_list->remove_item(selected_index);
	disabled_plugins_list->add_item(AdServer::get_singleton()->get_plugin_raw(plugin_id)->get_friendly_name());
	disabled_plugins_list->set_item_metadata(disabled_plugins_list->get_item_count() - 1, plugin_id);
	persist_settings();
}

void RamatakExportSettingsEditor::set_preset(Ref<EditorExportPreset> p_preset) {
	preset = p_preset;
}