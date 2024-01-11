#include "ramatak_export_settings.h"

#include "core/project_settings.h"
#include "core/ustring.h"
#include "editor/editor_export.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/flow_container.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "servers/ramatak/ad_plugin.h"
#include "servers/ramatak/ad_server.h"

RamatakExportSettingsEditor::RamatakExportSettingsEditor() {
	main_vbox = memnew(VBoxContainer);
	main_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	main_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(main_vbox);

	HFlowContainer *checkbox_row = memnew(HFlowContainer);
	checkbox_row->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	debug_checkbox = memnew(CheckBox);
	debug_checkbox->set_text("Advertisement Debug/Developer Mode");
	debug_checkbox->connect("toggled", this, "_debug_checkbox_toggled");
	checkbox_row->add_child(debug_checkbox);

	child_directed_checkbox = memnew(CheckBox);
	child_directed_checkbox->set_text("Project is child-directed");
	child_directed_checkbox->connect("toggled", this, "_child_directed_checkbox_toggled");
	checkbox_row->add_child(child_directed_checkbox);

	main_vbox->add_child(checkbox_row);

	status_label = memnew(Label);
	status_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	status_label->set_autowrap(true);

	priorities_hbox = memnew(HBoxContainer);
	priorities_hbox->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	main_vbox->add_child(priorities_hbox);

	HSplitContainer *priorities_hsplit = memnew(HSplitContainer);
	priorities_hsplit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	priorities_hbox->add_child(priorities_hsplit);

	VBoxContainer *disabled_plugins_vbox = memnew(VBoxContainer);
	disabled_plugins_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list = memnew(ItemList);
	disabled_plugins_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list->connect("item_selected", this, "_disabled_plugins_list_item_selected");

	Label *disabled_plugins_label = memnew(Label);
	disabled_plugins_label->set_text("Disabled Plugins:");

	disabled_plugins_vbox->add_child(disabled_plugins_label);
	disabled_plugins_vbox->add_child(disabled_plugins_list);
	priorities_hsplit->add_child(disabled_plugins_vbox);

	VBoxContainer *enabled_plugins_vbox = memnew(VBoxContainer);
	enabled_plugins_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list = memnew(ItemList);
	enabled_plugins_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list->connect("item_selected", this, "_enabled_plugins_list_item_selected");

	Label *enabled_plugins_label = memnew(Label);
	enabled_plugins_label->set_text("Enabled Plugins:");

	enabled_plugins_vbox->add_child(enabled_plugins_label);
	enabled_plugins_vbox->add_child(enabled_plugins_list);
	priorities_hsplit->add_child(enabled_plugins_vbox);

	button_row = memnew(VBoxContainer);
	priorities_hbox->add_child(button_row);

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
		Array priority_order = preset->get(AD_PLUGIN_PRIORITIES_KEY);
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
		debug_checkbox->set_pressed(preset->get(AD_DEBUG_MODE_KEY));
		child_directed_checkbox->set_pressed(preset->get(AD_CHILD_DIRECTED_KEY));
		update_status_label();
	}
}

void RamatakExportSettingsEditor::update_status_label() {
	if (preset == nullptr) {
		status_label->set_text("");
		return;
	}
	String status = "";

	Array revenue_generating_plugins;
	Array enabled_plugins = preset->get(AD_PLUGIN_PRIORITIES_KEY);
	for (int i = 0; i < enabled_plugins.size(); i++) {
		if (enabled_plugins[i] == "DUMMY") {
			continue;
		}
		revenue_generating_plugins.push_back(enabled_plugins[i]);
	}

	if (revenue_generating_plugins.size() > 0) {
		for (int i = 0; i < revenue_generating_plugins.size(); i++) {
			String plugin_id = revenue_generating_plugins[i];
			status += AdServer::get_singleton()->get_plugin_raw(plugin_id)->get_friendly_name();
			if (i != revenue_generating_plugins.size() - 1) {
				if (i == revenue_generating_plugins.size() - 2) {
					status += " and ";
				} else {
					status += ", ";
				}
			}
		}
		if (revenue_generating_plugins.size() > 1) {
			status += " are enabled.";
		} else {
			status += " is enabled.";
		}

		if (preset->get(AD_DEBUG_MODE_KEY)) {
			status += " Debug mode is enabled, ads will NOT generate revenue.";
		} else {
			status += " Debug mode is disabled, ads will generate revenue but testing with live ads is against the terms of service for some ad networks.";
		}

		if (preset->get(AD_CHILD_DIRECTED_KEY)) {
			status += " This project is marked as child-directed. Advertisement delivery will be suitable for children.";
		} else {
			status += " This project is marked as not child-directed. Advertisements may not be suitable for children.";
		}
	} else {
		status += "No revenue-generating advertisement plugins are enabled.";
	}

	status_label->set_text(status);
}

void RamatakExportSettingsEditor::_bind_methods() {
	ClassDB::bind_method("_increase_selected_priority", &RamatakExportSettingsEditor::_increase_selected_priority);
	ClassDB::bind_method("_decrease_selected_priority", &RamatakExportSettingsEditor::_decrease_selected_priority);
	ClassDB::bind_method("_enable_selected_plugin", &RamatakExportSettingsEditor::_enable_selected_plugin);
	ClassDB::bind_method("_disable_selected_plugin", &RamatakExportSettingsEditor::_disable_selected_plugin);
	ClassDB::bind_method("_enabled_plugins_list_item_selected", &RamatakExportSettingsEditor::_enabled_plugins_list_item_selected);
	ClassDB::bind_method("_disabled_plugins_list_item_selected", &RamatakExportSettingsEditor::_disabled_plugins_list_item_selected);
	ClassDB::bind_method("_debug_checkbox_toggled", &RamatakExportSettingsEditor::_debug_checkbox_toggled);
	ClassDB::bind_method("_child_directed_checkbox_toggled", &RamatakExportSettingsEditor::_child_directed_checkbox_toggled);
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
	preset->set(AD_PLUGIN_PRIORITIES_KEY, priorities);
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
	update_status_label();
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
	update_status_label();
}

void RamatakExportSettingsEditor::_enabled_plugins_list_item_selected(int p_index) {
	disabled_plugins_list->unselect_all();
}

void RamatakExportSettingsEditor::_disabled_plugins_list_item_selected(int p_index) {
	enabled_plugins_list->unselect_all();
}

void RamatakExportSettingsEditor::_debug_checkbox_toggled(bool p_pressed) {
	preset->set(AD_DEBUG_MODE_KEY, p_pressed);
	update_status_label();
}

void RamatakExportSettingsEditor::_child_directed_checkbox_toggled(bool p_pressed) {
	preset->set(AD_CHILD_DIRECTED_KEY, p_pressed);
	update_status_label();
}

Control *RamatakExportSettingsEditor::get_status_label() {
	return status_label;
}

void RamatakExportSettingsEditor::set_preset(Ref<EditorExportPreset> p_preset) {
	preset = p_preset;
	update_status_label();
}