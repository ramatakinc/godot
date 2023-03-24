#ifndef EDITOR_KEY_MANAGER_H
#define EDITOR_KEY_MANAGER_H

#include "core/dictionary.h"
#include "editor/editor_file_dialog.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/link_button.h"
#include "scene/gui/option_button.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tree.h"

class EditorKeyManager : public HSplitContainer {
	GDCLASS(EditorKeyManager, HSplitContainer);

public:
	enum AddKeyMode {
		ADD_EXISTING,
		ADD_NEW,
		ADD_CREATE_FILE,
		ADD_MOVED
	};

	enum DefaultType {
		DEFAULT_NOTHING,
		DEFAULT_RELEASE,
		DEFAULT_DEBUG,
		DEFAULT_DEBUG_RELEASE
	};

	const int KEY_PASSWORD_MINSIZE = 6;

private:
	Map<String, String> dname_tags;

	AddKeyMode _add_key_mode = ADD_EXISTING;

	Dictionary current_key_data;

	Button *add_keys;
	Button *create_key_existing;
	Button *create_key_new;

	EditorFileDialog *file_dialog;

	ConfirmationDialog *add_keys_dialog;
	LineEdit *add_pass;
	Button *unlock_keystore;
	Tree *add_keys_list;

	Tree *key_list;
	TreeItem *new_key = nullptr;

	LinkButton *from_keystore;
	OptionButton *default_for;
	Button *refresh;

	LineEdit *alias;
	LineEdit *pass_store;
	LineEdit *pass_store_confirm;
	SpinBox *validity;
	LineEdit *owner;
	LineEdit *org_unit;
	LineEdit *org;
	LineEdit *local;
	LineEdit *state;
	LineEdit *country_code;

	VBoxContainer *missing_file;

	Button *reset;
	Button *save;
	Button *cancel;
	Button *create;

	void _notification(int p_what);

	int _run_keytool(const List<String> &p_arguments, String *r_output, bool p_expects_return = true);
	Dictionary _extract_key_info(const String &p_output);

	void _add_keys_pressed();
	void _create_key_existing_pressed();
	void _create_key_new_pressed();

	void _open_keystore(const String &p_path);

	void _new_key(const String &p_path);

	void _add_keys_list_edited();
	void _add_keys_dialog_confirmed();

	void _key_list_cell_selected();
	void _key_list_button_pressed(Object *p_item, int p_column, int p_idx);

	void _unselect();

	void _from_keystore_pressed();
	void _refresh_pressed();
	void _default_for_selected(int p_index);
	void _keystore_text_changed(const String &p_text, const String &p_name);

	void _find_file_pressed();

	void _reset_pressed();
	void _save_pressed();
	void _cancel_pressed();
	void _create_pressed();

	void _unlock_keystore_pressed();
	void _pass_box_entered(const String &p_text);

protected:
	static void _bind_methods();

public:
	EditorKeyManager();
};

#endif // EDITOR_KEY_MANAGER_H
