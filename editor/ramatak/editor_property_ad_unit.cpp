#include "editor_property_ad_unit.h"

#include "core/project_settings.h"
#include "editor/editor_node.h"

void EditorPropertyAdUnit::_option_selected(int p_which) {
	if (p_which == 0) {
		emit_changed(get_edited_property(), "");
	} else {
		String val = options->get_item_text(p_which);
		emit_changed(get_edited_property(), val);
	}
}

void EditorPropertyAdUnit::_refresh_options() {
	options->clear();
	Dictionary ad_units = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	Array new_options = ad_units.keys();
	options->add_item("[none selected]");
	for (int i = 0; i < new_options.size(); i++) {
		String ad_unit_name = new_options[i];
		if (ad_unit_name.size() == 0) {
			continue;
		}
		options->add_item(ad_unit_name);
	}
}

void EditorPropertyAdUnit::update_property() {
	String ad_unit = get_edited_object()->get(get_edited_property());

	for (int i = 0; i < options->get_item_count(); i++) {
		if (ad_unit == options->get_item_text(i)) {
			options->select(i);
			return;
		}
	}
	options->select(0);
}

void EditorPropertyAdUnit::setup() {
	_refresh_options();
}

void EditorPropertyAdUnit::set_option_button_clip(bool p_enable) {
	options->set_clip_text(p_enable);
}

void EditorPropertyAdUnit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_option_selected"), &EditorPropertyAdUnit::_option_selected);
	ClassDB::bind_method(D_METHOD("_refresh_options"), &EditorPropertyAdUnit::_refresh_options);
}

EditorPropertyAdUnit::EditorPropertyAdUnit() {
	options = memnew(OptionButton);
	options->set_clip_text(true);
	options->set_flat(true);
	add_child(options);
	add_focusable(options);
	options->connect("item_selected", this, "_option_selected");
	ProjectSettings::get_singleton()->connect("project_settings_changed", this, "_refresh_options");
}
