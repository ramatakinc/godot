#include "ramatak_settings_editor.h"

#include "core/project_settings.h"
#include "core/ustring.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/item_list.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "servers/ramatak/ad_plugin.h"
#include "servers/ramatak/ad_server.h"

const String _AD_UNIT_TYPE_KEY = "ad_unit_type";

void RamatakSettingsAdUnitEditor::_bind_methods() {
	ADD_SIGNAL(MethodInfo("edited"));
	ClassDB::bind_method("_ad_unit_edited", &RamatakSettingsAdUnitEditor::_ad_unit_edited);
}

Dictionary RamatakSettingsAdUnitEditor::get_ad_unit_config() const {
	Dictionary ad_unit_config;
	ERR_FAIL_COND_V_MSG(edits.size() != labels.size(), ad_unit_config, "Edits and labels size mismatch.");
	ad_unit_config[_AD_UNIT_TYPE_KEY] = ad_unit_type_option_button->get_selected_metadata();
	for (int i = 0; i < edits.size(); i++) {
		ad_unit_config[plugins[i]] = edits[i]->get_text();
	}
	return ad_unit_config;
}

void RamatakSettingsAdUnitEditor::set_ad_unit(const String &p_ad_unit) {
	clear();
	ad_unit = p_ad_unit;
	Dictionary ad_units = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	Dictionary ad_unit_config = ad_units[ad_unit];
	// Set up the option button to select ad unit type.
	{
		Label *ad_unit_type_label = memnew(Label);
		ad_unit_type_label->set_text("Ad unit type:");
		grid_container->add_child(ad_unit_type_label);

		ad_unit_type_option_button = memnew(OptionButton);
		ad_unit_type_option_button->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		ad_unit_type_option_button->add_item("Banner");
		ad_unit_type_option_button->set_item_metadata(0, AdServer::AD_TYPE_BANNER);
		ad_unit_type_option_button->add_item("Interstitial");
		ad_unit_type_option_button->set_item_metadata(1, AdServer::AD_TYPE_INTERSTITIAL);
		ad_unit_type_option_button->add_item("Rewarded");
		ad_unit_type_option_button->set_item_metadata(2, AdServer::AD_TYPE_REWARDED);
		{
			Variant ad_unit_type = ad_unit_config.get(_AD_UNIT_TYPE_KEY, Variant());
			if (ad_unit_type == (Variant)AdServer::AD_TYPE_BANNER) {
				ad_unit_type_option_button->select(0);
			} else if (ad_unit_type == (Variant)AdServer::AD_TYPE_INTERSTITIAL) {
				ad_unit_type_option_button->select(1);
			} else if (ad_unit_type == (Variant)AdServer::AD_TYPE_REWARDED) {
				ad_unit_type_option_button->select(2);
			} else {
				// Set to banner by default.
				ad_unit_config[_AD_UNIT_TYPE_KEY] = (Variant)AdServer::AD_TYPE_BANNER;
				ad_unit_type_option_button->select(0);
			}
		}
		ad_unit_type_option_button->connect("item_selected", this, "_ad_unit_edited");
		grid_container->add_child(ad_unit_type_option_button);
	}

	Array all_plugins = AdServer::get_singleton()->get_available_plugins();
	for (int i = 0; i < all_plugins.size(); i++) {
		plugins.push_back(all_plugins[i]);

		Label *label = memnew(Label);
		label->set_text(vformat(TTR("%s ad unit ID:"), plugins[i]));
		label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		grid_container->add_child(label);
		labels.push_back(label);

		LineEdit *edit = memnew(LineEdit);
		edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		edit->connect("text_changed", this, "_ad_unit_edited");
		grid_container->add_child(edit);
		edits.push_back(edit);
		if (ad_unit_config.has(plugins[i])) {
			edit->set_text(ad_unit_config[plugins[i]]);
		}
	}
}

void RamatakSettingsAdUnitEditor::_ad_unit_edited(Variant p_arg) {
	emit_signal("edited");
}

void RamatakSettingsAdUnitEditor::clear() {
	ad_unit = "";
	ERR_FAIL_COND_MSG(edits.size() != labels.size(), "Edits and labels size mismatch.");
	for (int i = 0; i < edits.size(); i++) {
		LineEdit *edit = edits[i];
		Label *label = labels[i];
		edits[i] = nullptr;
		labels[i] = nullptr;
		memdelete(edit);
		memdelete(label);
	}
	edits.clear();
	labels.clear();
	plugins.clear();

	// Remove any stragglers.
	while (grid_container->get_child_count() != 0) {
		Node *child = grid_container->get_child(0);
		grid_container->remove_child(child);
		memdelete(child);
	}
}

void RamatakSettingsAdUnitEditor::set_disabled(bool p_disabled) {
	ERR_FAIL_COND_MSG(edits.size() != labels.size(), "Edits and labels size mismatch.");
	for (int i = 0; i < edits.size(); i++) {
		edits[i]->set_editable(!p_disabled);
	}
}

String RamatakSettingsAdUnitEditor::get_ad_unit() const {
	return ad_unit;
}

RamatakSettingsAdUnitEditor::RamatakSettingsAdUnitEditor() {
	grid_container = memnew(GridContainer);
	grid_container->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	grid_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	grid_container->set_columns(2);
	add_child(grid_container);
}

void RamatakSettingsAdUnitSetEditor::_bind_methods() {
	ClassDB::bind_method("_ad_unit_edited", &RamatakSettingsAdUnitSetEditor::_ad_unit_edited);
	ClassDB::bind_method("_add_edit_text_changed", &RamatakSettingsAdUnitSetEditor::_add_edit_text_changed);
	ClassDB::bind_method("_add_ad_unit", &RamatakSettingsAdUnitSetEditor::_add_ad_unit);
	ClassDB::bind_method("_remove_ad_unit", &RamatakSettingsAdUnitSetEditor::_remove_ad_unit);
	ClassDB::bind_method("_edit_items_list_item_selected", &RamatakSettingsAdUnitSetEditor::_edit_items_list_item_selected);
	ClassDB::bind_method("_save", &RamatakSettingsAdUnitSetEditor::_save);
}

void RamatakSettingsAdUnitSetEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		Dictionary ad_units_config = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
		edit_items_list->clear();
		Array ad_unit_names = ad_units_config.keys();
		for (int i = 0; i < ad_unit_names.size(); i++) {
			edit_items_list->add_item(ad_unit_names[i]);
		}
		if (ad_unit_names.size() > 0) {
			edit_items_list->select(0);
			ad_unit_editor->set_ad_unit(ad_unit_names[0]);
		}
		ad_unit_editor->set_ad_unit(ad_unit_editor->get_ad_unit());
	}
}

void RamatakSettingsAdUnitSetEditor::_add_edit_text_changed(const String &p_text) {
	add_button->set_disabled(p_text.empty());
}

void RamatakSettingsAdUnitSetEditor::_add_ad_unit() {
	String ad_unit = add_edit->get_text();
	Dictionary ad_units_config = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	if (ad_units_config.has(ad_unit) || ad_unit == "") {
		return;
	}
	add_edit->set_text("");
	add_button->set_disabled(true);
	ad_units_config[ad_unit] = Dictionary();
	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_units", ad_units_config);
	call_deferred("_save");
	edit_items_list->add_item(ad_unit);
}

void RamatakSettingsAdUnitSetEditor::_save() {
	if (Error::OK != ProjectSettings::get_singleton()->save()) {
		WARN_PRINT("Failed to save");
	}
}

void RamatakSettingsAdUnitSetEditor::_remove_ad_unit() {
	Dictionary ad_units_config = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	Vector<int> selected_items = edit_items_list->get_selected_items();
	if (selected_items.size() > 0) {
		// DEV_ASSERT(ad_unit_editor->get_ad_unit() != "");
		ad_units_config.erase(ad_unit_editor->get_ad_unit());
		edit_items_list->remove_item(selected_items.get(0));
		ad_unit_editor->clear();
		ad_unit_editor->set_disabled(true);
	}
	WARN_PRINT("REMOVING");
	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_units", ad_units_config);
	call_deferred("_save");
}

void RamatakSettingsAdUnitSetEditor::_edit_items_list_item_selected(int p_index) {
	Dictionary previous_ad_unit_config = ad_unit_editor->get_ad_unit_config();
	Dictionary ad_units_config = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	if (ad_unit_editor->get_ad_unit() != "") {
		ad_units_config[ad_unit_editor->get_ad_unit()] = previous_ad_unit_config;
	}

	String ad_unit = edit_items_list->get_item_text(p_index);
	ad_unit_editor->set_ad_unit(ad_unit);
	remove_button->set_disabled(false);
}

void RamatakSettingsAdUnitSetEditor::_ad_unit_edited() {
	Dictionary current_ad_unit_config = ad_unit_editor->get_ad_unit_config();
	Dictionary ad_units_config = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	if (ad_unit_editor->get_ad_unit() != "") {
		ad_units_config[ad_unit_editor->get_ad_unit()] = current_ad_unit_config;
		ProjectSettings::get_singleton()->set("ramatak/monetization/ad_units", ad_units_config);
		call_deferred("_save");
	}
}

RamatakSettingsAdUnitSetEditor::RamatakSettingsAdUnitSetEditor() {
	set_h_size_flags(Control::SIZE_EXPAND_FILL);

	main_vbox = memnew(VBoxContainer);
	main_vbox->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	add_child(main_vbox);

	add_hbox = memnew(HBoxContainer);
	add_label = memnew(Label);
	add_label->set_text(TTR("Add Advertisement Unit"));
	add_hbox->add_child(add_label);

	add_edit = memnew(LineEdit);
	add_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	add_edit->set_placeholder(TTR("Name (e.g. level_completed_interstitial)"));
	add_edit->set_clear_button_enabled(true);
	add_edit->connect("text_changed", this, "_add_edit_text_changed");
	add_hbox->add_child(add_edit);

	add_button = memnew(Button);
	add_button->set_text(TTR("Add"));
	add_button->set_disabled(true);
	add_button->connect("pressed", this, "_add_ad_unit");
	add_hbox->add_child(add_button);

	remove_button = memnew(Button);
	remove_button->set_text(TTR("Remove"));
	remove_button->set_disabled(true);
	remove_button->connect("pressed", this, "_remove_ad_unit");
	add_hbox->add_child(remove_button);

	main_vbox->add_child(add_hbox);

	edit_hbox = memnew(HBoxContainer);
	edit_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	edit_hbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	edit_items_list = memnew(ItemList);
	edit_items_list->set_custom_minimum_size(Size2(200, 0));
	edit_items_list->connect("item_selected", this, "_edit_items_list_item_selected", Vector<Variant>(), CONNECT_DEFERRED);
	edit_hbox->add_child(edit_items_list);

	ad_unit_editor = memnew(RamatakSettingsAdUnitEditor);
	ad_unit_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	ad_unit_editor->connect("edited", this, "_ad_unit_edited");
	edit_hbox->add_child(ad_unit_editor);

	main_vbox->add_child(edit_hbox);
}

RamatakAdPluginSettingsEditor::RamatakAdPluginSettingsEditor() {
	grid_container = memnew(GridContainer);
	grid_container->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	grid_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	grid_container->set_columns(2);
	add_child(grid_container);
}

void RamatakAdPluginSettingsEditor::_bind_methods() {
	ClassDB::bind_method("_setting_edited", &RamatakAdPluginSettingsEditor::_setting_edited);
}

void RamatakAdPluginSettingsEditor::set_plugin(const String &p_plugin) {
	clear();
	plugin_id = p_plugin;

	Ref<AdPlugin> plugin = AdServer::get_singleton()->get_plugin_raw(plugin_id);
	Array key_tooltip_pairs = plugin->get_config_key_tooltip_pairs();
	Dictionary ad_plugin_configs = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_config");
	Dictionary this_plugin_config = ad_plugin_configs[plugin_id];
	for (int i = 0; i < key_tooltip_pairs.size(); i++) {
		Array pair = key_tooltip_pairs[i];
		String key = pair[0];
		plugin_keys.push_back(key);

		// TODO: Tooltips.

		Label *label = memnew(Label);
		label->set_text(key);
		label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		grid_container->add_child(label);
		labels.push_back(label);

		LineEdit *edit = memnew(LineEdit);
		edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		edit->connect("text_changed", this, "_setting_edited");
		grid_container->add_child(edit);
		edits.push_back(edit);

		if (this_plugin_config.has(key)) {
			edit->set_text(this_plugin_config[key]);
		}
	}
}

void RamatakAdPluginSettingsEditor::_setting_edited(Variant p_arg) {
	Dictionary ad_plugin_configs = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_plugin_config");
	Dictionary this_plugin_config = ad_plugin_configs[plugin_id];

	Ref<AdPlugin> plugin = AdServer::get_singleton()->get_plugin_raw(plugin_id);
	Array key_tooltip_pairs = plugin->get_config_key_tooltip_pairs();
	for (int i = 0; i < key_tooltip_pairs.size(); i++) {
		Array pair = key_tooltip_pairs[i];
		String key = pair[0];

		this_plugin_config[key] = edits[i]->get_text();
		WARN_PRINT(key);
		WARN_PRINT(edits[i]->get_text());
	}
	ad_plugin_configs[plugin_id] = this_plugin_config;
	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_plugin_config", ad_plugin_configs);
	ProjectSettings::get_singleton()->save();
}

void RamatakAdPluginSettingsEditor::clear() {
	ERR_FAIL_COND_MSG(edits.size() != labels.size(), "Edits and labels size mismatch.");
	for (int i = 0; i < edits.size(); i++) {
		LineEdit *edit = edits[i];
		Label *label = labels[i];
		edits[i] = nullptr;
		labels[i] = nullptr;
		memdelete(edit);
		memdelete(label);
	}
	edits.clear();
	labels.clear();
	plugin_keys.clear();

	// Remove any stragglers.
	while (grid_container->get_child_count() != 0) {
		Node *child = grid_container->get_child(0);
		grid_container->remove_child(child);
		memdelete(child);
	}
}

RamatakAdPluginPriorityEditor::RamatakAdPluginPriorityEditor() {
	main_hbox = memnew(HBoxContainer);
	main_hbox->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	add_child(main_hbox);

	VBoxContainer *disabled_plugins_vbox = memnew(VBoxContainer);
	disabled_plugins_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list = memnew(ItemList);
	disabled_plugins_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	disabled_plugins_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	Label *disabled_plugins_label = memnew(Label);
	disabled_plugins_label->set_text("Disabled plugins");

	disabled_plugins_vbox->add_child(disabled_plugins_label);
	disabled_plugins_vbox->add_child(disabled_plugins_list);
	main_hbox->add_child(disabled_plugins_vbox);

	VBoxContainer *enabled_plugins_vbox = memnew(VBoxContainer);
	enabled_plugins_vbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list = memnew(ItemList);
	enabled_plugins_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	enabled_plugins_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	Label *enabled_plugins_label = memnew(Label);
	enabled_plugins_label->set_text("Enabled plugins");

	enabled_plugins_vbox->add_child(enabled_plugins_label);
	enabled_plugins_vbox->add_child(enabled_plugins_list);
	main_hbox->add_child(enabled_plugins_vbox);

	button_row = memnew(VBoxContainer);
	main_hbox->add_child(button_row);

	increase_prioity = memnew(Button);
	increase_prioity->set_text("Increase priority");
	increase_prioity->connect("pressed", this, "_increase_selected_priority");
	button_row->add_child(increase_prioity);

	decrease_prioity = memnew(Button);
	decrease_prioity->set_text("Decrease priority");
	decrease_prioity->connect("pressed", this, "_decrease_selected_priority");
	button_row->add_child(decrease_prioity);

	enable_plugin = memnew(Button);
	enable_plugin->set_text("Enable plugin");
	enable_plugin->connect("pressed", this, "_enable_selected_plugin");
	button_row->add_child(enable_plugin);

	disable_plugin = memnew(Button);
	disable_plugin->set_text("Disable plugin");
	disable_plugin->connect("pressed", this, "_disable_selected_plugin");
	button_row->add_child(disable_plugin);
}

void RamatakAdPluginPriorityEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		Array all_plugins = AdServer::get_singleton()->get_available_plugins();
		Array priority_order = AdServer::get_singleton()->get_plugin_priority_order();
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

void RamatakAdPluginPriorityEditor::_bind_methods() {
	ClassDB::bind_method("_increase_selected_priority", &RamatakAdPluginPriorityEditor::_increase_selected_priority);
	ClassDB::bind_method("_decrease_selected_priority", &RamatakAdPluginPriorityEditor::_decrease_selected_priority);
	ClassDB::bind_method("_enable_selected_plugin", &RamatakAdPluginPriorityEditor::_enable_selected_plugin);
	ClassDB::bind_method("_disable_selected_plugin", &RamatakAdPluginPriorityEditor::_disable_selected_plugin);
}

int RamatakAdPluginPriorityEditor::get_selected_index(ItemList *p_list) {
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

void RamatakAdPluginPriorityEditor::persist_settings() {
	Array priorities;
	for (int i = 0; i < enabled_plugins_list->get_item_count(); i++) {
		String id = enabled_plugins_list->get_item_metadata(i);
		priorities.push_back(id);
	}
	AdServer::get_singleton()->set_plugin_priority_order(priorities);
}

void RamatakAdPluginPriorityEditor::_increase_selected_priority() {
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

void RamatakAdPluginPriorityEditor::_decrease_selected_priority() {
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

void RamatakAdPluginPriorityEditor::_enable_selected_plugin() {
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

void RamatakAdPluginPriorityEditor::_disable_selected_plugin() {
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

RamatakSettingsEditor::RamatakSettingsEditor() {
	ad_plugin_settings_editor = memnew(RamatakAdPluginSettingsEditor);
	ad_plugin_settings_editor->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	ad_plugin_settings_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	ad_unit_set_editor = memnew(RamatakSettingsAdUnitSetEditor);
	ad_unit_set_editor->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	ad_unit_set_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	ad_priority_editor = memnew(RamatakAdPluginPriorityEditor);
	ad_priority_editor->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	ad_priority_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	main_hbox = memnew(HBoxContainer);
	main_hbox->set_anchors_and_margins_preset(LayoutPreset::PRESET_WIDE, LayoutPresetMode::PRESET_MODE_KEEP_SIZE);
	add_child(main_hbox);

	edit_items_list = memnew(ItemList);
	edit_items_list->set_custom_minimum_size(Size2(200, 0));

	edit_items_list->add_item("Ad units");
	edit_items_list->set_item_metadata(edit_items_list->get_item_count() - 1, (Variant)AD_UNITS);

	edit_items_list->add_item("Plugin priorities");
	edit_items_list->set_item_metadata(edit_items_list->get_item_count() - 1, (Variant)PLUGIN_PRIORITIES);

	edit_items_list->connect("item_selected", this, "_edit_items_list_item_selected");
	main_hbox->add_child(edit_items_list);

	main_hbox->add_child(ad_unit_set_editor);
	current_pane = ad_unit_set_editor;
}

void RamatakSettingsEditor::_bind_methods() {
	ClassDB::bind_method("_edit_items_list_item_selected", &RamatakSettingsEditor::_edit_items_list_item_selected);
}

void RamatakSettingsEditor::_edit_items_list_item_selected(int p_index) {
	if (edit_items_list->get_item_metadata(p_index) == (Variant)AD_UNITS) {
		if (current_pane) {
			main_hbox->remove_child(current_pane);
		}

		current_pane = ad_unit_set_editor;
		main_hbox->add_child(current_pane);

	} else if (edit_items_list->get_item_metadata(p_index) == (Variant)PLUGIN_PRIORITIES) {
		if (current_pane) {
			main_hbox->remove_child(current_pane);
		}
		current_pane = this->ad_priority_editor;
		main_hbox->add_child(current_pane);
	} else {
		if (current_pane) {
			main_hbox->remove_child(current_pane);
		}

		String plugin = edit_items_list->get_item_metadata(p_index);
		this->ad_plugin_settings_editor->set_plugin(plugin);

		current_pane = this->ad_plugin_settings_editor;
		main_hbox->add_child(current_pane);
	}
}

void RamatakSettingsEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		plugins = AdServer::get_singleton()->get_available_plugins();
		for (int i = 0; i < plugins.size(); i++) {
			if (i == 0) {
				ad_unit_set_editor->remove_button->set_disabled(false);
			}
			Ref<AdPlugin> plugin = AdServer::get_singleton()->get_plugin_raw(plugins[i]);
			Array key_tooltip_pairs = plugin->get_config_key_tooltip_pairs();
			if (key_tooltip_pairs.empty()) {
				continue;
			}
			edit_items_list->add_item(vformat("%s options", plugin->get_friendly_name()));
			// Use the non-friendly name as metadata.
			edit_items_list->set_item_metadata(edit_items_list->get_item_count() - 1, plugins[i]);
		}
		edit_items_list->select(0);
	}
}