#include "editor_property_ad_unit.h"

#include "core/config/project_settings.h"
#include "editor/editor_node.h"
#include "scene/gui/option_button.h"

void EditorPropertyAdUnit::_set_read_only(bool p_read_only) {
	options->set_disabled(p_read_only);
}

void EditorPropertyAdUnit::_option_selected(int p_which) {
	int64_t val = options->get_item_metadata(p_which);
	emit_changed(get_edited_property(), val);
}

void EditorPropertyAdUnit::_refresh_options() {
	options->clear();
	Dictionary ad_units = ProjectSettings::get_singleton()->get("ramatak/monetization/ad_units");
	Array new_options = ad_units.keys();
	int64_t current_val = 0;
	for (int i = 0; i < new_options.size(); i++) {
		String ad_unit_name = new_options[i];
		options->add_item(ad_unit_name);
		options->set_item_metadata(i, current_val);
		current_val += 1;
	}
}

void EditorPropertyAdUnit::update_property() {
	Variant current = get_edited_object()->get(get_edited_property());
	if (current.get_type() == Variant::NIL) {
		options->select(-1);
		return;
	}

	int64_t which = current;
	for (int i = 0; i < options->get_item_count(); i++) {
		if (which == (int64_t)options->get_item_metadata(i)) {
			options->select(i);
			return;
		}
	}
}

void EditorPropertyAdUnit::setup() {
	_refresh_options();
}

void EditorPropertyAdUnit::set_option_button_clip(bool p_enable) {
	options->set_clip_text(p_enable);
}

void EditorPropertyAdUnit::_bind_methods() {
}

EditorPropertyAdUnit::EditorPropertyAdUnit() {
	options = memnew(OptionButton);
	options->set_clip_text(true);
	options->set_flat(true);
	add_child(options);
	add_focusable(options);
	options->connect("item_selected", callable_mp(this, &EditorPropertyAdUnit::_option_selected));
	EditorNode::get_singleton()->connect("project_settings_changed", callable_mp(this, &EditorPropertyAdUnit::_refresh_options));
}
