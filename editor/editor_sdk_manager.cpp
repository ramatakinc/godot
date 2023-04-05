#include "editor/editor_sdk_manager.h"

#include "core/dictionary.h"
#include "editor_node.h"
#include "editor_scale.h"
#include "scene/gui/box_container.h"
#include "scene/gui/separator.h"
#include "scene/gui/tree.h"

void EditorSDKManager::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_package_list();

			[[fallthrough]];
		}
		case NOTIFICATION_THEME_CHANGED: {
			search_box->set_right_icon(get_icon("Search", "EditorIcons"));
		} break;
		case NOTIFICATION_PROCESS: {
			if (_is_working) {
				return;
			}

			thread->wait_to_finish();

			set_process(false);

			refresh->set_disabled(false);
			bool has_changes = !to_install.empty() || !to_remove.empty();
			reset->set_disabled(!has_changes);
			apply->set_disabled(!has_changes);

			working->hide();
			package_list->show();

			_reorganize_packages();
		} break;
	}
}

int EditorSDKManager::_run_sdkmanager(const List<String> &p_arguments, String *r_output) {
	int result = OK;
	String sdkmanager = OS::get_singleton()->get_environment("RAMATAK_SDKMANAGER_HOME");
	String sdk_root = OS::get_singleton()->get_environment("RAMATAK_ANDROID_SDK_HOME");

	List<String> args = p_arguments;
	args.push_front("--sdk_root=" + sdk_root);

	r_output->clear();
	result = OS::get_singleton()->execute(sdkmanager, args, true, nullptr, r_output);
	if (result == OK && r_output->empty()) {
		result = FAILED;
	}

	if (result != OK) {
		EditorNode::get_singleton()->show_warning(TTR("Error calling sdkmanager binary. Path in JAVA_HOME, RAMATAK_SDKMANAGER_HOME, or RAMATAK_ANDROID_SDK_HOME environment variables either invalid or not set."));
	}

	return result;
}

void EditorSDKManager::_update_package_list() {
	_is_working = true;

	package_list->hide();
	working->show();

	set_process(true);

	thread->start(_update_packages_thread, this);
}

void EditorSDKManager::_update_packages_thread(void *s) {
	EditorSDKManager *self = (EditorSDKManager *)s;
	self->_update_packages();
}

void EditorSDKManager::_update_packages() {
	List<String> args;
	args.push_back("--list");

	String output;
	int result = _run_sdkmanager(args, &output);
	if (result != OK) {
		_is_working = false;
		return;
	}

	List<String> installed;

	int from = output.find("Installed packages:");
	int limit = output.find("Available Packages:");

	// Get the paths of the installed packages.
	if (from != -1) {
		from = output.find("\n", output.find("-------", from)) + 1;

		while (true) {
			int to = output.find("|", from) ;
			if (to > limit) {
				break;
			}

			installed.push_back(output.substr(from, to - from).strip_edges());

			from = output.find("\n", from) + 1;
			if (from >= limit) {
				break;
			}
		}
	}

	from = output.find("\n", output.find("-------", limit)) + 1;
	limit = output.length() - 1;

	// Extract the info of all packages available.
	packages_available.clear();
	while (true) {
		int newline = output.find("\n", from);
		if (newline == -1) {
			break;
		}

		String line = output.substr(from, newline - from).strip_edges();
		if (!line.empty()) {
			Vector<String> strings = output.substr(from, newline - from).split("|", false);
			ERR_BREAK(strings.size() < 2);

			Vector<String> data;
			data.push_back(strings[2].strip_edges());
			data.push_back(strings[1].strip_edges());
			packages_available[strings[0].strip_edges()] = data;
		}

		from = newline + 1;
	}

	if (!installed.empty()) {
		//Place the installed ones in their own container.
		for (List<String>::Element *E = installed.front(); E; E = E->next()) {
			if (packages_available.has(E->get())) {
				packages_installed[E->get()] = packages_available[E->get()];
				packages_available.erase(E->get());
			} else {
				Vector<String> data;
				data.push_back(E->get());
				data.resize(2);
				packages_installed[E->get()] = data;
			}
		}
	}

	_is_working = false;
}

void EditorSDKManager::_apply_changes_thread(void *s) {
	EditorSDKManager *self = (EditorSDKManager *)s;
	self->_apply_changes();
}

void EditorSDKManager::_apply_changes() {
	if (!to_remove.empty()) {
		List<String> args;
		args.push_back("--uninstall");

		for (List<String>::Element *E = to_remove.front(); E; E = E->next()) {
			args.push_back(E->get());
		}

		String output;
		int result = _run_sdkmanager(args, &output);
		if (result != OK) {
			_is_working = false;
			return;
		}

		for (List<String>::Element *E = to_remove.front(); E; E = E->next()) {
			packages_available[E->get()] = packages_installed[E->get()];
			packages_installed.erase(E->get());
		}
		to_remove.clear();
	}

	if (!to_install.empty()) {
		List<String> args;
		for (List<String>::Element *E = to_install.front(); E; E = E->next()) {
			args.push_back(E->get());
		}

		String output;
		int result = _run_sdkmanager(args, &output);
		if (result != OK) {
			_is_working = false;
			return;
		}

		for (List<String>::Element *E = to_install.front(); E; E = E->next()) {
			packages_installed[E->get()] = packages_available[E->get()];
			packages_available.erase(E->get());
		}
		to_install.clear();
	}

	_is_working = false;
}

void EditorSDKManager::_reorganize_packages() {
	package_list->clear();

	TreeItem *root = package_list->create_item();
	Color section_color = get_color("prop_subsection", "Editor");

	String search = search_box->get_text().strip_edges().to_lower();

	if (!packages_installed.keys().empty()) {
		section_installed = package_list->create_item(root);
		section_installed->set_text(1, TTR("Installed"));
		section_installed->set_custom_bg_color(0, section_color);
		section_installed->set_custom_bg_color(1, section_color);
		section_installed->set_custom_bg_color(2, section_color);
		section_installed->set_custom_bg_color(3, section_color);

		List<Variant> ids;
		packages_installed.get_key_list(&ids);
		for (List<Variant>::Element *E = ids.front(); E; E = E->next()) {
			String id = E->get();
			Array data = packages_installed[id];
			String name = data[0];
			if (!search.empty() && name.to_lower().find(search) == -1 && id.find(search) == -1) {
				continue;
			}

			TreeItem *item = package_list->create_item(section_installed);
			item->set_cell_mode(0, TreeItem::CELL_MODE_CHECK);
			item->set_checked(0, !to_remove.find(id));
			item->set_metadata(0, id);
			item->set_text(1, name);
			item->set_text(2, data[1]);
		}
	}

	if (!packages_available.keys().empty()) {
		section_available = package_list->create_item(root);
		section_available->set_text(1, TTR("Available"));
		section_available->set_custom_bg_color(0, section_color);
		section_available->set_custom_bg_color(1, section_color);
		section_available->set_custom_bg_color(2, section_color);
		section_available->set_custom_bg_color(3, section_color);

		List<Variant> ids;
		packages_available.get_key_list(&ids);
		for (List<Variant>::Element *E = ids.front(); E; E = E->next()) {
			String id = E->get();
			Array data = packages_available[id];
			String name = data[0];
			if (!search.empty() && name.to_lower().find(search) == -1 && id.find(search) == -1) {
				continue;
			}

			TreeItem *item = package_list->create_item(section_available);
			item->set_cell_mode(0, TreeItem::CELL_MODE_CHECK);
			item->set_checked(0, to_install.find(id));
			item->set_metadata(0, id);
			item->set_text(1, name);
			item->set_text(3, data[1]);
		}
	}
}

void EditorSDKManager::_search_box_text_changed(const String &p_new_text) {
	_reorganize_packages();
}

void EditorSDKManager::_refresh_pressed() {
	refresh->set_disabled(true);
	_update_package_list();
}

void EditorSDKManager::_reset_pressed() {
	to_install.clear();
	to_remove.clear();

	_reorganize_packages();

	reset->set_disabled(true);
	apply->set_disabled(true);
}

void EditorSDKManager::_apply_pressed() {
	_is_working = true;

	refresh->set_disabled(true);
	reset->set_disabled(true);
	apply->set_disabled(true);

	package_list->hide();
	working->show();

	set_process(true);

	thread->start(_apply_changes_thread, this);
}

void EditorSDKManager::_package_list_item_selected() {
	if (_is_working) {
		return;
	}

	TreeItem *item = package_list->get_selected();
	item->set_checked(0, !item->is_checked(0));

	String id = item->get_metadata(0);
	TreeItem *parent = item->get_parent();

	if (parent == section_installed) {
		if (!item->is_checked(0)) {
			to_remove.push_back(id);
		} else {
			to_remove.erase(id);
		}
	} else if (parent == section_available) {
		if (item->is_checked(0)) {
			to_install.push_back(id);
		} else {
			to_install.erase(id);
		}
	}

	bool has_changes = !to_install.empty() || !to_remove.empty();
	reset->set_disabled(!has_changes);
	apply->set_disabled(!has_changes);
}

LineEdit *EditorSDKManager::get_search_box() {
	return search_box;
}

void EditorSDKManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_search_box_text_changed"), &EditorSDKManager::_search_box_text_changed);
	ClassDB::bind_method(D_METHOD("_refresh_pressed"), &EditorSDKManager::_refresh_pressed);
	ClassDB::bind_method(D_METHOD("_reset_pressed"), &EditorSDKManager::_reset_pressed);
	ClassDB::bind_method(D_METHOD("_apply_pressed"), &EditorSDKManager::_apply_pressed);
	ClassDB::bind_method(D_METHOD("_package_list_item_selected"), &EditorSDKManager::_package_list_item_selected);
}

EditorSDKManager::EditorSDKManager() {
	thread = memnew(Thread);

	set_process(false);

	set_name(TTR("SDK Manager"));

	HBoxContainer *hbc = memnew(HBoxContainer);
	add_child(hbc);

	search_box = memnew(LineEdit);
	search_box->set_clear_button_enabled(true);
	search_box->set_placeholder(TTR("Search"));
	search_box->set_h_size_flags(SIZE_EXPAND_FILL);
	hbc->add_child(search_box);
	search_box->connect("text_changed", this, "_search_box_text_changed");

	refresh = memnew(Button);
	refresh->set_text(TTR("Refresh"));
	refresh->set_disabled(true);
	hbc->add_child(refresh);
	refresh->connect("pressed", this, "_refresh_pressed");

	hbc->add_child(memnew(VSeparator));

	reset = memnew(Button);
	reset->set_text(TTR("Reset"));
	reset->set_disabled(true);
	hbc->add_child(reset);
	reset->connect("pressed", this, "_reset_pressed");

	apply = memnew(Button);
	apply->set_text(TTR("Apply"));
	apply->set_disabled(true);
	hbc->add_child(apply);
	apply->connect("pressed", this, "_apply_pressed");

	package_list = memnew(Tree);
	package_list->set_select_mode(Tree::SELECT_ROW);
	package_list->set_hide_root(true);
	package_list->set_allow_reselect(true);
	package_list->set_column_titles_visible(true);
	package_list->set_columns(4);
	package_list->set_column_title(1, TTR("Package"));
	package_list->set_column_title(2, TTR("Installed Version"));
	package_list->set_column_title(3, TTR("Available Version"));
	package_list->set_column_min_width(0, 42 * EDSCALE);
	package_list->set_column_min_width(2, 150 * EDSCALE);
	package_list->set_column_min_width(3, 150 * EDSCALE);
	package_list->set_column_expand(0, false);
	package_list->set_column_expand(2, false);
	package_list->set_column_expand(3, false);
	package_list->set_custom_minimum_size(Size2(400, 300) * EDSCALE);
	package_list->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(package_list);
	package_list->connect("item_selected", this, "_package_list_item_selected");

	working = memnew(Label(TTR("Working, please wait.")));
	working->set_align(Label::ALIGN_CENTER);
	working->set_valign(Label::VALIGN_CENTER);
	working->set_h_size_flags(SIZE_EXPAND_FILL);
	working->set_v_size_flags(SIZE_EXPAND_FILL);
	add_child(working);
	working->hide();
}
