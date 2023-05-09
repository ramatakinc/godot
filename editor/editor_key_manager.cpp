#include "editor/editor_key_manager.h"

#include "editor_node.h"
#include "editor_scale.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/tree.h"

void EditorKeyManager::_notification(int p_what) {
	if (p_what == NOTIFICATION_ENTER_TREE) {
		// List all saved keys.
		if (EditorSettings::get_singleton()->has_setting("key_manager/keys")) {
			Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");
			Array sha256_list = keys.keys();

			DirAccessRef da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
			for (int i = 0; i < sha256_list.size(); i++) {
				String sha256 = sha256_list[i];

				TreeItem *item = key_list->create_item(key_list->get_root());
				item->set_text(0, keys[sha256].get("alias"));
				item->add_button(0, get_icon("Remove", "EditorIcons"), -1, false, TTR("Remove"));
				item->set_metadata(0, sha256);

				if (!da->file_exists(keys[sha256].get("path"))) {
					item->set_custom_color(0, get_color("error_color", "Editor"));
					item->set_meta("missing", true);
				}
			}
		}

		add_keys->set_icon(get_icon("Load", "EditorIcons"));
		create_key_existing->set_icon(get_icon("Add", "EditorIcons"));
		create_key_new->set_icon(get_icon("New", "EditorIcons"));
		refresh->set_icon(get_icon("Reload", "EditorIcons"));
	}
}

int EditorKeyManager::_run_keytool(const List<String> &p_arguments, String *r_output, bool p_expects_return) {
	int result = OK;
	String keytool = OS::get_singleton()->get_environment("RAMATAK_KEYTOOL_HOME");

	r_output->clear();
	result = OS::get_singleton()->execute(keytool, p_arguments, true, nullptr, r_output);

	if (result == OK && p_expects_return && r_output->empty()) {
		result = FAILED;
	}

	if (result != OK) {
		EditorNode::get_singleton()->show_warning(TTR("Error calling keytool binary. Path in JAVA_HOME or RAMATAK_KEYTOOL_HOME environment variables either invalid or not set."));
	}

	return result;
}

Dictionary EditorKeyManager::_extract_key_info(const String &p_output) {
	int from = 0;
	Dictionary keys;
	while (true) {
		from = p_output.find("Alias name: ", from);
		if (from == -1) {
			break;
		}

		from += String("Alias name: ").size() - 1;

		Dictionary values;
		values["alias"] = p_output.substr(from, p_output.find("\n", from) - from);

		int limit = p_output.find("***", from);
		if (limit == -1) {
			limit = p_output.length() - 1;
		}

		from = p_output.find("Owner: ", from);

		if (from == -1 || from >= limit) {
			from = limit;
			continue;
		}

		from += String("Owner: ").size() - 1;

		// Fill up all tags first, so they don't return null.
		for (Map<String, String>::Element *E=dname_tags.front(); E; E = E->next()) {
			values[E->get()] = "";
		}

		String dname = p_output.substr(from, p_output.find("\n", from) - from);
		Vector<String> dname_list = dname.split(", ", false);
		for (int i = 0; i < dname_list.size(); i++) {
			String item = dname_list[i];
			String tag = item.substr(0, item.find("="));

			values[dname_tags[tag]] = item.substr(tag.size());
		}

		from = p_output.find(" until:", from);
		if (from == -1 || from >= limit) {
			from = limit;
			continue;
		}

		from -= 4;

		int year_start = p_output.substr(from, 4).to_int();
		from = p_output.find("\n", from);
		int year_end = p_output.substr(from, 4).to_int();
		values["validity"] = year_end - year_start;

		from = p_output.find("SHA256: ", from);
		if (from == -1 || from >= limit) {
			from = limit;
			continue;
		}

		from += String("SHA256: ").size() - 1;

		String sha256 = p_output.substr(from, p_output.find("\n", from) - from);
		keys[sha256] = values;

		from = limit;
	}

	return keys;
}

void EditorKeyManager::_add_keys_pressed() {
	_add_key_mode = ADD_EXISTING;

	file_dialog->set_mode(EditorFileDialog::MODE_OPEN_FILE);
	file_dialog->popup_centered_ratio();
}

void EditorKeyManager::_create_key_existing_pressed() {
	if (new_key) {
		return;
	}

	_add_key_mode = ADD_NEW;

	file_dialog->set_mode(EditorFileDialog::MODE_OPEN_FILE);
	file_dialog->popup_centered_ratio();
}

void EditorKeyManager::_create_key_new_pressed() {
	if (new_key) {
		return;
	}

	_add_key_mode = ADD_CREATE_FILE;

	file_dialog->set_mode(EditorFileDialog::MODE_SAVE_FILE);
	file_dialog->set_current_file("new_keystore.keystore");
	file_dialog->popup_centered_ratio();
}

void EditorKeyManager::_open_keystore(const String &p_path) {
	switch (_add_key_mode) {
		case ADD_EXISTING: {
			add_pass->clear();
			add_pass->set_editable(true);
			unlock_keystore->set_disabled(false);
			add_keys_list->clear();
			add_keys_dialog->get_ok()->set_disabled(true);
			add_keys_dialog->popup_centered();
			add_pass->grab_focus();
		} break;

		case ADD_CREATE_FILE:
		case ADD_NEW: {
			_new_key(p_path);
		} break;

		case ADD_MOVED: {
			current_key_data["path"] = p_path;
			_refresh_pressed();
		} break;
	}
}

void EditorKeyManager::_new_key(const String &p_path) {
	current_key_data = Dictionary();
	current_key_data["path"] = p_path;

	new_key = key_list->create_item(key_list->get_root());
	new_key->set_text(0, TTR("[New]"));
	new_key->select(0);

	from_keystore->set_text(p_path.get_file());
	from_keystore->set_tooltip(String());
	from_keystore->set_underline_mode(LinkButton::UNDERLINE_MODE_NEVER);
	from_keystore->set_mouse_filter(MOUSE_FILTER_IGNORE);
	refresh->set_disabled(true);
	default_for->set_disabled(true);
	default_for->select(0);

	alias->set_editable(true);
	pass_store->set_editable(true);
	pass_store->add_color_override("font_color", get_color("font_color", "Label"));
	pass_store_confirm->set_editable(true);
	pass_store_confirm->add_color_override("font_color", get_color("font_color", "Label"));
	validity->set_editable(true);
	owner->set_editable(true);
	org_unit->set_editable(true);
	org->set_editable(true);
	local->set_editable(true);
	state->set_editable(true);
	country_code->set_editable(true);

	alias->set_text("");
	pass_store->set_text("");
	pass_store_confirm->set_text("");
	validity->set_min(1);
	validity->set_value(1);
	owner->set_text("");
	org_unit->set_text("");
	org->set_text("");
	local->set_text("");
	state->set_text("");
	country_code->set_text("");

	reset->hide();
	save->hide();
	cancel->show();
	create->show();
	create->set_disabled(true);
}

void EditorKeyManager::_add_keys_list_edited() {
	bool has_keys_checked = false;

	TreeItem *item = add_keys_list->get_root()->get_children();
	while (item) {
		if (item->is_checked(0)) {
			has_keys_checked = true;
			break;
		}

		item = item->get_next();
	}

	add_keys_dialog->get_ok()->set_disabled(!has_keys_checked);
}

void EditorKeyManager::_add_keys_dialog_confirmed() {
	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");

	TreeItem *item = add_keys_list->get_root()->get_children();
	TreeItem *added_item;
	while (item) {
		if (!item->is_checked(0)) {
			item = item->get_next();
			continue;
		}

		Dictionary data = item->get_metadata(0);
		keys.merge(data);
		String sha256 = data.keys()[0];

		added_item = key_list->create_item(key_list->get_root());
		added_item->set_text(0, keys[sha256].get("alias"));
		added_item->add_button(0, get_icon("Remove", "EditorIcons"), -1, false, TTR("Remove"));
		added_item->set_metadata(0, sha256);

		item = item->get_next();
	}

	added_item->select(0);

	EditorSettings::get_singleton()->set_manually("key_manager/keys", keys);
	EditorSettings::get_singleton()->save();

	add_keys_dialog->hide();
}

void EditorKeyManager::_key_list_cell_selected() {
	TreeItem *item = key_list->get_selected();
	ERR_FAIL_COND(!item);

	if (new_key) {
		if (item != new_key) {
			key_list->get_root()->remove_child(new_key);
			new_key = nullptr;

			validity->set_editable(false);
			owner->set_editable(false);
			org_unit->set_editable(false);
			org->set_editable(false);
			local->set_editable(false);
			state->set_editable(false);
			country_code->set_editable(false);

			reset->show();
			save->show();
			cancel->hide();
			create->hide();
		} else {
			return;
		}
	}

	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");
	String sha256 = item->get_metadata(0);
	current_key_data = keys[sha256];

	bool is_missing = item->has_meta("missing");

	from_keystore->set_text(String(current_key_data["path"]).get_file());
	from_keystore->set_underline_mode(is_missing ? LinkButton::UNDERLINE_MODE_NEVER : LinkButton::UNDERLINE_MODE_ON_HOVER);
	from_keystore->set_tooltip(current_key_data["path"]);
	from_keystore->set_mouse_filter(is_missing ? MOUSE_FILTER_IGNORE : MOUSE_FILTER_PASS);
	from_keystore->add_color_override("font_color", is_missing ? get_color("error_color", "Editor") : get_color("font_color", "LinkButton"));
	from_keystore->set_meta("folder", String(current_key_data["path"]).get_base_dir());
	refresh->set_disabled(false);

	bool is_debug = EditorSettings::get_singleton()->get("key_manager/default_debug") == sha256;
	bool is_release = EditorSettings::get_singleton()->get("key_manager/default_release") == sha256;
	if (is_debug && is_release) {
		default_for->select(DEFAULT_DEBUG_RELEASE);
	} else if (is_debug) {
		default_for->select(DEFAULT_DEBUG);
	} else if (is_release) {
		default_for->select(DEFAULT_RELEASE);
	} else {
		default_for->select(DEFAULT_NOTHING);
	}
	default_for->set_disabled(is_missing);

	alias->set_editable(!is_missing);
	pass_store->set_editable(!is_missing);
	pass_store_confirm->set_editable(!is_missing);
	pass_store_confirm->add_color_override("font_color", get_color("font_color", "Label"));
	validity->set_min(0);

	alias->set_text(current_key_data["alias"]);
	pass_store->set_text(current_key_data["pass_store"]);
	pass_store_confirm->set_text(current_key_data["pass_store"]);
	validity->set_value(current_key_data["validity"]);
	owner->set_text(current_key_data["owner"]);
	org_unit->set_text(current_key_data["org_unit"]);
	org->set_text(current_key_data["org"]);
	local->set_text(current_key_data["local"]);
	state->set_text(current_key_data["state"]);
	country_code->set_text(current_key_data["country_code"]);

	missing_file->set_visible(is_missing);

	reset->set_disabled(true);
	save->set_disabled(true);
}

void EditorKeyManager::_key_list_button_pressed(Object *p_item, int p_column, int p_idx) {
	TreeItem *item = Object::cast_to<TreeItem>(p_item);
	ERR_FAIL_COND(!item);

	String sha256 = item->get_metadata(0);
	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");
	keys.erase(sha256);
	EditorSettings::get_singleton()->set_manually("key_manager/keys", keys);

	if (EditorSettings::get_singleton()->get("key_manager/default_debug") == sha256) {
		EditorSettings::get_singleton()->set_manually("key_manager/default_debug", String());
	}
	if (EditorSettings::get_singleton()->get("key_manager/default_release") == sha256) {
		EditorSettings::get_singleton()->set_manually("key_manager/default_release", String());
	}

	EditorSettings::get_singleton()->save();

	if (key_list->get_selected() == item) {
		current_key_data.clear();
		_unselect();
	}

	key_list->get_root()->remove_child(item);
}

void EditorKeyManager::_unselect() {
	from_keystore->set_text(TTR("[None]"));
	from_keystore->set_tooltip(String());
	from_keystore->set_underline_mode(LinkButton::UNDERLINE_MODE_NEVER);
	from_keystore->set_mouse_filter(MOUSE_FILTER_IGNORE);
	refresh->set_disabled(true);
	default_for->set_disabled(true);
	default_for->select(0);

	alias->set_editable(false);
	pass_store->set_editable(false);
	pass_store_confirm->set_editable(false);
	pass_store_confirm->add_color_override("font_color", get_color("font_color", "Label"));
	validity->set_editable(false);
	owner->set_editable(false);
	org_unit->set_editable(false);
	org->set_editable(false);
	local->set_editable(false);
	state->set_editable(false);
	country_code->set_editable(false);

	alias->set_text("");
	pass_store->set_text("");
	pass_store_confirm->set_text("");
	validity->set_value(0);
	owner->set_text("");
	org_unit->set_text("");
	org->set_text("");
	local->set_text("");
	state->set_text("");
	country_code->set_text("");
}

void EditorKeyManager::_from_keystore_pressed() {
	OS::get_singleton()->shell_open(from_keystore->get_meta("folder"));
}

void EditorKeyManager::_default_for_selected(int p_index) {
	TreeItem *item = key_list->get_selected();
	ERR_FAIL_COND(!item);

	String sha256 = item->get_metadata(0);
	switch (p_index) {
		case DEFAULT_NOTHING: {
			if (EditorSettings::get_singleton()->get("key_manager/default_debug") == sha256) {
				EditorSettings::get_singleton()->set_manually("key_manager/default_debug", String());
			}
			if (EditorSettings::get_singleton()->get("key_manager/default_release") == sha256) {
				EditorSettings::get_singleton()->set_manually("key_manager/default_release", String());
			}
		} break;
		case DEFAULT_DEBUG: {
			EditorSettings::get_singleton()->set_manually("key_manager/default_debug", sha256);
		} break;
		case DEFAULT_RELEASE: {
			EditorSettings::get_singleton()->set_manually("key_manager/default_release", sha256);
		} break;
		case DEFAULT_DEBUG_RELEASE: {
			EditorSettings::get_singleton()->set_manually("key_manager/default_debug", sha256);
			EditorSettings::get_singleton()->set_manually("key_manager/default_release", sha256);
		} break;
	}

	EditorSettings::get_singleton()->save();
}

void EditorKeyManager::_refresh_pressed() {
	String path = current_key_data["path"];
	TreeItem *item = key_list->get_selected();

	DirAccessRef da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	if (!da->file_exists(path)) {
		item->set_meta("missing", true);
		item->set_custom_color(0, get_color("error_color", "Editor"));
		item->deselect(0);
		item->select(0);

		return;
	}

	List<String> args;
	args.push_back("-list");
	args.push_back("-v");
	args.push_back("-alias");
	args.push_back(current_key_data["alias"]);
	args.push_back("-keystore");
	args.push_back(path);
	args.push_back("-storepass");
	args.push_back(current_key_data["pass_store"]);

	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");
	String sha256 = item->get_metadata(0);

	String output;
	int result = _run_keytool(args, &output);
	if (result != OK || output.begins_with("keytool error")) {
		if (result == OK) {
			EditorNode::get_singleton()->show_warning(TTR("Could not unlock keystore. The entry may have been deleted or the password may be incorrect."));
		}

		// Save if the path now points to a file that exists at least.
		if (keys[sha256].get("path") != path) {
			keys[sha256].set("path", current_key_data);
			EditorSettings::get_singleton()->set_manually("key_manager/keys", keys);
		}
	} else {
		Dictionary key = _extract_key_info(output);
		if (!key.has(sha256)) {
			EditorNode::get_singleton()->show_warning(TTR("The entry's SHA256 differs from the one stored. May be an incorrect file."));

			item->set_meta("missing", true);
			item->deselect(0);
			item->select(0);

			return;
		}

		current_key_data.merge(key[sha256], true);
		keys[sha256] = current_key_data;
		EditorSettings::get_singleton()->set_manually("key_manager/keys", keys);
	}

	if (item->has_meta("missing")) {
		item->remove_meta("missing");
		item->set_custom_color(0, get_color("font_color", "Tree"));
	}

	item->deselect(0);
	item->select(0);
}

void EditorKeyManager::_keystore_text_changed(const String &p_text, const String &p_name) {
	bool has_changed = false;
	if (!new_key) {
		has_changed = current_key_data["alias"] != alias->get_text() || current_key_data["pass_store"] != pass_store->get_text();
	}

	bool password_valid = true;
	if (p_name == "pass_store") {
		if (pass_store->get_text().strip_edges().empty() || pass_store->get_text().strip_edges().length() < KEY_PASSWORD_MINSIZE) {
			password_valid = false;
			pass_store->add_color_override("font_color", get_color("error_color", "Editor"));
		} else {
			pass_store->add_color_override("font_color", get_color("font_color", "Label"));
		}
	}
	if (pass_store->get_text() != pass_store_confirm->get_text()) {
		password_valid = false;
		pass_store_confirm->add_color_override("font_color", get_color("error_color", "Editor"));
	} else {
		pass_store_confirm->add_color_override("font_color", get_color("font_color", "Label"));
	}

	if (!new_key) {
		reset->set_disabled(!has_changed);
		save->set_disabled(!has_changed || !password_valid || alias->get_text().empty());
	} else {
		create->set_disabled(!password_valid || alias->get_text().empty());
	}
}

void EditorKeyManager::_find_file_pressed() {
	_add_key_mode = ADD_MOVED;

	file_dialog->set_mode(EditorFileDialog::MODE_OPEN_FILE);
	file_dialog->popup_centered_ratio();
}

void EditorKeyManager::_reset_pressed() {
	alias->set_text(current_key_data["alias"]);
	pass_store->set_text(current_key_data["pass_store"]);
	pass_store_confirm->set_text(current_key_data["pass_store"]);
	pass_store_confirm->add_color_override("font_color", get_color("font_color", "Label"));
	validity->set_value(current_key_data["validity"]);
	owner->set_text(current_key_data["owner"]);
	org_unit->set_text(current_key_data["org_unit"]);
	org->set_text(current_key_data["org"]);
	local->set_text(current_key_data["local"]);
	state->set_text(current_key_data["state"]);
	country_code->set_text(current_key_data["country_code"]);

	reset->set_disabled(true);
	save->set_disabled(true);
}

void EditorKeyManager::_save_pressed() {
	String path = current_key_data["path"];
	TreeItem *item = key_list->get_selected();

	DirAccessRef da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	if (!da->file_exists(path)) {
		item->set_meta("missing", true);
		item->deselect(0);
		item->select(0);

		return;
	}

	bool has_alias_changed = false;

	if (alias->get_text() != current_key_data["alias"]) {
		List<String> args;
		args.push_back("-changealias");
		args.push_back("-alias");
		args.push_back(current_key_data["alias"]);
		args.push_back("-destalias");
		args.push_back(alias->get_text());
		args.push_back("-keystore");
		args.push_back(path);
		args.push_back("-storepass");
		args.push_back(current_key_data["pass_store"]);

		String output;
		int result = _run_keytool(args, &output, false);
		if (result != OK) {
			return;
		}

		if (output.begins_with("keytool error")) {
			EditorNode::get_singleton()->show_warning(TTR("Could not change alias in keystore file. The alias may be invalid or the password may be incorrect."));
			return;
		}

		has_alias_changed = true;

		current_key_data["alias"] = alias->get_text();
		key_list->get_selected()->set_text(0, current_key_data["alias"]);
	}

	String password = pass_store->get_text();
	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");

	if (password != current_key_data["pass_store"]) {
		List<String> args;
		args.push_back("-storepasswd");
		args.push_back("-new");
		args.push_back(password);
		args.push_back("-keystore");
		args.push_back(path);
		args.push_back("-storepass");
		args.push_back(current_key_data["pass_store"]);

		bool password_valid = true;

		String output;
		int result = _run_keytool(args, &output, false);
		if (result != OK) {
			return;
		}

		if (output.begins_with("keytool error")) {
			if (!has_alias_changed) {
				// Attempt to use the new password to unlock it. In case the user is trying to update a keystore that was modified externally.
				args.clear();
				args.push_back("-list");
				args.push_back("-alias");
				args.push_back(current_key_data["alias"]);
				args.push_back("-keystore");
				args.push_back(path);
				args.push_back("-storepass");
				args.push_back(password);

				output.clear();
				result = _run_keytool(args, &output);
				if (result != OK) {
					return;
				}

				if (output.begins_with("keytool error")) {
					EditorNode::get_singleton()->show_warning(TTR("Could not change password in keystore file. Original password might have been changed externally."));
					return;
				}
			} else {
				password_valid = false;
				EditorNode::get_singleton()->show_warning(TTR("Could not change password in keystore file."));
			}
		}

		if (password_valid) {
			current_key_data["pass_store"] = password;

			// Change the stored password for all keys that also share this keystore.
			Array sha256_list = keys.keys();
			for (int i = 0; i < sha256_list.size(); i++) {
				Dictionary key = keys[sha256_list[i]];
				if (key["path"] != path) {
					continue;
				}

				key["pass_store"] = password;
				keys[sha256_list[i]] = key;
			}
		}
	}

	keys[key_list->get_selected()->get_metadata(0)] = current_key_data;
	EditorSettings::get_singleton()->set_manually("key_manager/keys", keys);
	EditorSettings::get_singleton()->save();

	reset->set_disabled(true);
	save->set_disabled(true);
}

void EditorKeyManager::_cancel_pressed() {
	key_list->get_root()->remove_child(new_key);
	new_key = nullptr;

	_unselect();
}

void EditorKeyManager::_create_pressed() {
	current_key_data["alias"] = alias->get_text();
	current_key_data["pass_store"] = pass_store->get_text();
	current_key_data["validity"] = (int)validity->get_value();
	current_key_data["owner"] = owner->get_text();
	current_key_data["org_unit"] = org_unit->get_text();
	current_key_data["org"] = org->get_text();
	current_key_data["local"] = local->get_text();
	current_key_data["state"] = state->get_text();
	current_key_data["country_code"] = country_code->get_text();

	List<String> args;
	args.push_back("-genkey");
	args.push_back("-keystore");
	args.push_back(current_key_data["path"]);
	args.push_back("-alias");
	args.push_back(current_key_data["alias"]);
	args.push_back("-dname");

	String dname_arg;
	for (Map<String, String>::Element *E=dname_tags.front(); E; E = E->next()) {
		dname_arg += E->key() + "=" + current_key_data[E->get()];
		if (E->next()) {
			dname_arg += ",";
		}
	}
	args.push_back(dname_arg);

	args.push_back("-keyalg");
	args.push_back("RSA");
	args.push_back("-validity");
	args.push_back(itos(validity->get_value() * 365));
	args.push_back("-storepass");
	args.push_back(pass_store->get_text());

	String output;
	int result = _run_keytool(args, &output, false);
	if (result != OK ) {
		return;
	}

	if (output.begins_with("keytool error")) {
		EditorNode::get_singleton()->show_warning(TTR("Could not create key or keystore file."));
		return;
	}

	args.clear();
	args.push_back("-list");
	args.push_back("-alias");
	args.push_back(current_key_data["alias"]);
	args.push_back("-keystore");
	args.push_back(current_key_data["path"]);
	args.push_back("-storepass");
	args.push_back(pass_store->get_text());

	output.clear();
	result = _run_keytool(args, &output);
	if (result != OK || output.begins_with("keytool error")) {
		if (result == OK ) {
			EditorNode::get_singleton()->show_warning(TTR("Could not read keystore file."));
		}

		return;
	}

	int from = output.find("(SHA-256): ");
	String sha256 = output.substr(from + String("(SHA-256): ").length());
	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");
	keys[sha256] = current_key_data;
	EditorSettings::get_singleton()->set_manually("key_manager/keys", keys);
	EditorSettings::get_singleton()->save();

	new_key->set_text(0, current_key_data["alias"]);
	new_key->add_button(0, get_icon("Remove", "EditorIcons"), -1, false, TTR("Remove"));
	new_key->set_metadata(0, sha256);

	validity->set_editable(false);
	owner->set_editable(false);
	org_unit->set_editable(false);
	org->set_editable(false);
	local->set_editable(false);
	state->set_editable(false);
	country_code->set_editable(false);

	TreeItem *item = new_key;
	new_key = nullptr;
	item->deselect(0);
	item->select(0);

	reset->show();
	save->show();
	cancel->hide();
	create->hide();
}

void EditorKeyManager::_unlock_keystore_pressed() {
	String path = ProjectSettings::get_singleton()->globalize_path(file_dialog->get_current_file());
	String password = add_pass->get_text();

	List<String> args;
	args.push_back("-list");
	args.push_back("-v");
	args.push_back("-keystore");
	args.push_back(path);
	args.push_back("-storepass");
	args.push_back(password);

	String output;
	int result = _run_keytool(args, &output);
	if (result != OK || output.begins_with("keytool error")) {
		add_pass->grab_focus();

		if (result == OK) {
			EditorNode::get_singleton()->show_warning(TTR("Could not unlock keystore. The password may be incorrect."));
		}

		add_pass->select_all();

		return;
	}

	add_pass->set_editable(false);
	unlock_keystore->set_disabled(true);
	add_keys_list->grab_focus();

	TreeItem *root = add_keys_list->create_item();
	Dictionary keys = EditorSettings::get_singleton()->get("key_manager/keys");
	Dictionary keys_extracted = _extract_key_info(output);
	Array sha256_list = keys_extracted.keys();
	for (int i = 0; i < sha256_list.size(); i++) {
		String sha256 = sha256_list[i];

		Dictionary values = keys_extracted[sha256];
		values["path"] = file_dialog->get_current_path();
		values["pass_store"] = password;

		TreeItem *item = add_keys_list->create_item(root);
		item->set_cell_mode(0, TreeItem::CELL_MODE_CHECK);
		item->set_text(0, keys_extracted[sha256].get("alias"));

		// Check if the key has already been added before.
		if (keys.has(sha256)) {
			// Fade out added keys for easier visual grepping.
			item->set_custom_color(0, get_color("font_color", "Label") * Color(1, 1, 1, 0.5));
		} else {
			Dictionary key;
			key[sha256] = values;
			item->set_metadata(0, key);
			item->set_editable(0, true);
		}
	}
}

void EditorKeyManager::_pass_box_entered(const String &p_text) {
	_unlock_keystore_pressed();
}

void EditorKeyManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_add_keys_pressed"), &EditorKeyManager::_add_keys_pressed);
	ClassDB::bind_method(D_METHOD("_create_key_existing_pressed"), &EditorKeyManager::_create_key_existing_pressed);
	ClassDB::bind_method(D_METHOD("_create_key_new_pressed"), &EditorKeyManager::_create_key_new_pressed);
	ClassDB::bind_method(D_METHOD("_open_keystore"), &EditorKeyManager::_open_keystore);
	ClassDB::bind_method(D_METHOD("_add_keys_list_edited"), &EditorKeyManager::_add_keys_list_edited);
	ClassDB::bind_method(D_METHOD("_add_keys_dialog_confirmed"), &EditorKeyManager::_add_keys_dialog_confirmed);
	ClassDB::bind_method(D_METHOD("_key_list_cell_selected"), &EditorKeyManager::_key_list_cell_selected);
	ClassDB::bind_method(D_METHOD("_key_list_button_pressed"), &EditorKeyManager::_key_list_button_pressed);
	ClassDB::bind_method(D_METHOD("_from_keystore_pressed"), &EditorKeyManager::_from_keystore_pressed);
	ClassDB::bind_method(D_METHOD("_refresh_pressed"), &EditorKeyManager::_refresh_pressed);
	ClassDB::bind_method(D_METHOD("_default_for_selected"), &EditorKeyManager::_default_for_selected);
	ClassDB::bind_method(D_METHOD("_keystore_text_changed"), &EditorKeyManager::_keystore_text_changed);
	ClassDB::bind_method(D_METHOD("_find_file_pressed"), &EditorKeyManager::_find_file_pressed);
	ClassDB::bind_method(D_METHOD("_reset_pressed"), &EditorKeyManager::_reset_pressed);
	ClassDB::bind_method(D_METHOD("_save_pressed"), &EditorKeyManager::_save_pressed);
	ClassDB::bind_method(D_METHOD("_cancel_pressed"), &EditorKeyManager::_cancel_pressed);
	ClassDB::bind_method(D_METHOD("_create_pressed"), &EditorKeyManager::_create_pressed);
	ClassDB::bind_method(D_METHOD("_unlock_keystore_pressed"), &EditorKeyManager::_unlock_keystore_pressed);
	ClassDB::bind_method(D_METHOD("_pass_box_entered"), &EditorKeyManager::_pass_box_entered);;
}

EditorKeyManager::EditorKeyManager() {
	EDITOR_DEF("key_manager/keys", Dictionary());
	EDITOR_DEF("key_manager/default_debug", String());
	EDITOR_DEF("key_manager/default_release", String());

	dname_tags["CN"] = "owner";
	dname_tags["OU"] = "org_unit";
	dname_tags["O"] = "org";
	dname_tags["L"] = "local";
	dname_tags["ST"] = "state";
	dname_tags["C"] = "country_code";

	set_name(TTR("Key Manager"));

	VBoxContainer *vbc = memnew(VBoxContainer);
	add_child(vbc);
	HBoxContainer *hbc = memnew(HBoxContainer);
	vbc->add_child(hbc);

	Label *l = memnew(Label(TTR("Keys:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	hbc->add_child(l);

	file_dialog = memnew(EditorFileDialog);
	file_dialog->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
	file_dialog->add_filter("*.keystore; " + TTR("Keystore File"));
	file_dialog->add_filter("*.jks; " + TTR("Java Keystore File"));
	add_child(file_dialog);
	file_dialog->connect("file_selected", this, "_open_keystore");

	add_keys = memnew(Button);
	add_keys->set_flat(true);
	add_keys->set_tooltip(TTR("Add one or more keys from a keystore file."));
	hbc->add_child(add_keys);
	add_keys->connect("pressed", this, "_add_keys_pressed");
	create_key_existing = memnew(Button);
	create_key_existing->set_flat(true);
	create_key_existing->set_tooltip(TTR("Create a new key in an existing keystore file."));
	hbc->add_child(create_key_existing);
	create_key_existing->connect("pressed", this, "_create_key_existing_pressed");
	create_key_new = memnew(Button);
	create_key_new->set_flat(true);
	create_key_new->set_tooltip(TTR("Create a new key together with a new keystore file."));
	hbc->add_child(create_key_new);
	create_key_new->connect("pressed", this, "_create_key_new_pressed");

	key_list = memnew(Tree);
	key_list->set_hide_root(true);
	key_list->set_custom_minimum_size(Size2(200 * EDSCALE, 0));
	key_list->set_v_size_flags(SIZE_EXPAND_FILL);
	vbc->add_child(key_list);
	key_list->create_item();
	key_list->connect("cell_selected", this, "_key_list_cell_selected");
	key_list->connect("button_pressed", this, "_key_list_button_pressed");

	add_keys_dialog = memnew(ConfirmationDialog);
	add_keys_dialog->set_title(TTR("Add Keys"));
	add_keys_dialog->get_ok()->set_text(TTR("Add"));
	add_keys_dialog->set_hide_on_ok(false);
	add_keys_dialog->set_custom_minimum_size(Size2(400, 500) * EDSCALE);
	add_child(add_keys_dialog);
	add_keys_dialog->connect("confirmed", this, "_add_keys_dialog_confirmed");
	vbc = memnew(VBoxContainer);
	add_keys_dialog->add_child(vbc);
	l = memnew(Label(TTR("Password:")));
	vbc->add_child(l);
	hbc = memnew(HBoxContainer);
	vbc->add_child(hbc);
	add_pass = memnew(LineEdit);
	add_pass->set_h_size_flags(SIZE_EXPAND_FILL);
	hbc->add_child(add_pass);
	add_pass->connect("text_entered", this, "_pass_box_entered");
	unlock_keystore = memnew(Button);
	unlock_keystore->set_text(TTR("Unlock"));
	hbc->add_child(unlock_keystore);
	unlock_keystore->connect("pressed", this, "_unlock_keystore_pressed");
	add_keys_list = memnew(Tree);
	add_keys_list->set_hide_root(true);
	add_keys_list->set_v_size_flags(SIZE_EXPAND_FILL);
	vbc->add_child(add_keys_list);
	add_keys_list->connect("item_edited", this, "_add_keys_list_edited");

	vbc = memnew(VBoxContainer);
	add_child(vbc);
	GridContainer *gc = memnew(GridContainer);
	gc->set_columns(2);
	vbc->add_child(gc);

	l = memnew(Label(TTR("From Keystore:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	hbc = memnew(HBoxContainer);
	gc->add_child(hbc);
	from_keystore = memnew(LinkButton());
	from_keystore->set_text(TTR("[None]"));
	from_keystore->set_underline_mode(LinkButton::UNDERLINE_MODE_NEVER);
	from_keystore->set_mouse_filter(MOUSE_FILTER_IGNORE);
	from_keystore->set_h_size_flags(SIZE_EXPAND_FILL);
	hbc->add_child(from_keystore);
	from_keystore->connect("pressed", this, "_from_keystore_pressed");

	refresh = memnew(Button);
	refresh->set_flat(true);
	refresh->set_disabled(true);
	refresh->set_tooltip(TTR("Refresh data from the keystore file."));
	hbc->add_child(refresh);
	refresh->connect("pressed", this, "_refresh_pressed");

	hbc = memnew(HBoxContainer);
	vbc->add_child(hbc);
	l = memnew(Label(TTR("Set as Default For:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	default_for = memnew(OptionButton);
	default_for->set_disabled(true);
	default_for->add_item(TTR("Nothing"), DEFAULT_NOTHING);
	default_for->add_item("Release", DEFAULT_RELEASE);
	default_for->add_item("Debug", DEFAULT_DEBUG);
	default_for->add_item("Release and Debug", DEFAULT_DEBUG_RELEASE);
	default_for->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(default_for);
	default_for->connect("item_selected", this, "_default_for_selected");

	HSeparator *sep = memnew(HSeparator);
	vbc->add_child(sep);

	gc = memnew(GridContainer);
	gc->set_columns(2);
	gc->set_custom_minimum_size(Size2(400 * EDSCALE, 0));
	vbc->add_child(gc);

	l = memnew(Label(TTR("Store Password:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	pass_store = memnew(LineEdit);
	pass_store->set_editable(false);
	pass_store->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(pass_store);
	pass_store->connect("text_changed", this, "_keystore_text_changed", varray("pass_store"));

	l = memnew(Label(TTR("Confirm Store Password:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	pass_store_confirm = memnew(LineEdit);
	pass_store_confirm->set_editable(false);
	pass_store_confirm->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(pass_store_confirm);
	pass_store_confirm->connect("text_changed", this, "_keystore_text_changed", varray("pass_store"));

	sep = memnew(HSeparator);
	vbc->add_child(sep);

	gc = memnew(GridContainer);
	gc->set_columns(2);
	gc->set_custom_minimum_size(Size2(400 * EDSCALE, 0));
	vbc->add_child(gc);

	l = memnew(Label(TTR("Alias:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	alias = memnew(LineEdit);
	alias->set_editable(false);
	alias->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(alias);
	alias->connect("text_changed", this, "_keystore_text_changed", varray("alias"));

	l = memnew(Label(TTR("Validity (Years):")));
	gc->add_child(l);
	validity = memnew(SpinBox);
	validity->set_editable(false);
	validity->set_max(99);
	gc->add_child(validity);

	l = memnew(Label(TTR("First and Last Name:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	owner = memnew(LineEdit);
	owner->set_editable(false);
	owner->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(owner);

	l = memnew(Label(TTR("Organizational Unit:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	org_unit = memnew(LineEdit);
	org_unit->set_editable(false);
	org_unit->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(org_unit);

	l = memnew(Label(TTR("Organization:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	org = memnew(LineEdit);
	org->set_editable(false);
	org->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(org);

	l = memnew(Label(TTR("City or Locality:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	local = memnew(LineEdit);
	local->set_editable(false);
	local->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(local);

	l = memnew(Label(TTR("State or Province:")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	state = memnew(LineEdit);
	state->set_editable(false);
	state->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(state);

	l = memnew(Label(TTR("Country Code (XX):")));
	l->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(l);
	country_code = memnew(LineEdit);
	country_code->set_editable(false);
	country_code->set_h_size_flags(SIZE_EXPAND_FILL);
	gc->add_child(country_code);

	missing_file = memnew(VBoxContainer);
	missing_file->hide();
	vbc->add_child(missing_file);

	sep = memnew(HSeparator);
	missing_file->add_child(sep);

	l = memnew(Label(TTR("The keystore file is missing. It could have been renamed, moved, or deleted.")));
	l->set_align(Label::ALIGN_CENTER);
	l->set_autowrap(true);
	missing_file->add_child(l);
	Button *find_file = memnew(Button);
	find_file->set_text(TTR("Find File"));
	missing_file->add_child(find_file);
	find_file->set_h_size_flags(SIZE_SHRINK_CENTER);
	find_file->connect("pressed", this, "_find_file_pressed");

	hbc = memnew(HBoxContainer);
	hbc->set_alignment(HBoxContainer::ALIGN_CENTER);
	hbc->set_v_size_flags(SIZE_EXPAND | SIZE_SHRINK_END);
	vbc->add_child(hbc);

	reset = memnew(Button);
	reset->set_text(TTR("Reset"));
	reset->set_disabled(true);
	hbc->add_child(reset);
	reset->connect("pressed", this, "_reset_pressed");
	save = memnew(Button);
	save->set_text(TTR("Save"));
	save->set_disabled(true);
	hbc->add_child(save);
	save->connect("pressed", this, "_save_pressed");

	cancel = memnew(Button);
	cancel->set_text(TTR("Cancel"));
	cancel->hide();
	hbc->add_child(cancel);
	cancel->connect("pressed", this, "_cancel_pressed");
	create = memnew(Button);
	create->set_text(TTR("Create"));
	create->hide();
	hbc->add_child(create);
	create->connect("pressed", this, "_create_pressed");
}
