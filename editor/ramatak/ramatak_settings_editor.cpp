#include "ramatak_settings_editor.h"

#include "core/project_settings.h"
#include "core/ustring.h"
#include "scene/gui/box_container.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/button.h"
#include "scene/gui/item_list.h"
#include "scene/gui/option_button.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
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
		Label* ad_unit_type_label = memnew(Label);
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
		Node* child = grid_container->get_child(0);
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

void RamatakSettingsEditor::_bind_methods() {
	ClassDB::bind_method("_ad_unit_edited", &RamatakSettingsEditor::_ad_unit_edited);
	ClassDB::bind_method("_add_edit_text_changed", &RamatakSettingsEditor::_add_edit_text_changed);
	ClassDB::bind_method("_add_ad_unit", &RamatakSettingsEditor::_add_ad_unit);
	ClassDB::bind_method("_edit_items_list_item_selected", &RamatakSettingsEditor::_edit_items_list_item_selected);
	ClassDB::bind_method("_save", &RamatakSettingsEditor::_save);
	ClassDB::bind_method("_revert", &RamatakSettingsEditor::_revert);

}

void RamatakSettingsEditor::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		_revert();
	}
}


void RamatakSettingsEditor::_add_edit_text_changed(const String &p_text) {
	add_button->set_disabled(p_text.empty());
}

void RamatakSettingsEditor::_add_ad_unit() {
	String ad_unit = add_edit->get_text();
	add_edit->set_text("");
	add_button->set_disabled(true);
	Dictionary ad_units = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	if (ad_units.has(ad_unit)) {
		return;
	}
	ad_units[ad_unit] = Dictionary();
	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_units", ad_units);
	edit_items_list->add_item(ad_unit);
}

void RamatakSettingsEditor::_revert() {
	ad_units_config = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	edit_items_list->clear();
	Array ad_unit_names = ad_units_config.keys();
	for (int i = 0; i < ad_unit_names.size(); i++) {
		edit_items_list->add_item(ad_unit_names[i]);
	}
	if (ad_unit_names.size() > 0) {
		edit_items_list->select(0);
		ad_unit_editor->set_ad_unit(ad_unit_names[0]);
	}
	save_button->set_disabled(true);
	revert_button->set_disabled(true);
	ad_unit_editor->set_ad_unit(ad_unit_editor->get_ad_unit());
}

void RamatakSettingsEditor::_save() {
	Dictionary current_ad_unit_config = ad_unit_editor->get_ad_unit_config();
	if (ad_unit_editor->get_ad_unit() != "") {
		ad_units_config[ad_unit_editor->get_ad_unit()] = current_ad_unit_config;
	}

	ProjectSettings::get_singleton()->set("ramatak/monetization/ad_units", ad_units_config);
	ProjectSettings::get_singleton()->save();
	save_button->set_disabled(true);
	revert_button->set_disabled(true);
}

void RamatakSettingsEditor::_edit_items_list_item_selected(int p_index) {
	Dictionary previous_ad_unit_config = ad_unit_editor->get_ad_unit_config();
	if (ad_unit_editor->get_ad_unit() != "") {
		ad_units_config[ad_unit_editor->get_ad_unit()] = previous_ad_unit_config;
	}

	String ad_unit = edit_items_list->get_item_text(p_index);
	ad_unit_editor->set_ad_unit(ad_unit);
}

void RamatakSettingsEditor::_ad_unit_edited() {
	save_button->set_disabled(false);
	revert_button->set_disabled(false);
}

RamatakSettingsEditor::RamatakSettingsEditor() {
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

	main_vbox->add_child(add_hbox);

	edit_hbox = memnew(HBoxContainer);
	edit_hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	edit_hbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);

	edit_items_list = memnew(ItemList);
	edit_items_list->set_custom_minimum_size(Size2(200, 0));
	edit_items_list->connect("item_selected", this, "_edit_items_list_item_selected");
	edit_hbox->add_child(edit_items_list);

	ad_unit_editor = memnew(RamatakSettingsAdUnitEditor);
	ad_unit_editor->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	ad_unit_editor->connect("edited", this, "_ad_unit_edited");
	edit_hbox->add_child(ad_unit_editor);

	main_vbox->add_child(edit_hbox);

	save_row_hbox = memnew(HBoxContainer);
	save_row_hbox->set_h_size_flags(Control::SIZE_SHRINK_END);
	main_vbox->add_child(save_row_hbox);

	save_button = memnew(Button);
	save_button->set_text(TTR("Save"));
	save_button->set_disabled(true);
	save_button->connect("pressed", this, "_save");
	save_row_hbox->add_child(save_button);

	revert_button = memnew(Button);
	revert_button->set_text(TTR("Revert"));
	revert_button->set_disabled(true);
	revert_button->connect("pressed", this, "_revert");
	save_row_hbox->add_child(revert_button);
}