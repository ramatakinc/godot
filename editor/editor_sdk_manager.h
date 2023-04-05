#ifndef EDITOR_SDK_MANAGER_H
#define EDITOR_SDK_MANAGER_H

#include "core/dictionary.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/tree.h"

class EditorSDKManager : public VBoxContainer {
	GDCLASS(EditorSDKManager, VBoxContainer);

	Thread *thread;
	bool _is_working = false;

	List<String> to_install;
	List<String> to_remove;

	Dictionary packages_installed;
	Dictionary packages_available;

	LineEdit *search_box;

	Button *refresh;
	Button *reset;
	Button *apply;

	Tree *package_list;
	TreeItem *section_installed;
	TreeItem *section_available;

	Label *working;

	void _notification(int p_what);

	int _run_sdkmanager(const List<String> &p_arguments, String *r_output);

	static void _update_packages_thread(void *s);
	void _update_packages();

	void _update_package_list();
	void _reorganize_packages();

	static void _apply_changes_thread(void *s);
	void _apply_changes();

	void _search_box_text_changed(const String &p_new_text);
	void _refresh_pressed();
	void _reset_pressed();
	void _apply_pressed();

	void _package_list_item_selected();

protected:
	static void _bind_methods();

public:
	LineEdit *get_search_box();

	EditorSDKManager();
};

#endif // EDITOR_SDK_MANAGER_H
