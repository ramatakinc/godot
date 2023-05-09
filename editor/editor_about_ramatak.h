#ifndef EDITOR_ABOUT_RAMATAK_H
#define EDITOR_ABOUT_RAMATAK_H

#include "scene/gui/control.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/item_list.h"
#include "scene/gui/link_button.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/rich_text_label.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/separator.h"
#include "scene/gui/split_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/tree.h"

#include "editor_scale.h"
#include "scene/resources/dynamic_font.h"
#include "scene/resources/style_box.h"

/**
 * NOTE: Do not assume the EditorAboutRamatak singleton to be available in this class' methods.
 * EditorAboutRamatak is also used from the project manager where EditorAboutRamatak isn't initialized.
 */
class EditorAboutRamatak : public AcceptDialog {
	GDCLASS(EditorAboutRamatak, AcceptDialog);

private:
	void _license_tree_selected();
	void _link_clicked(const String &p_link);
	ScrollContainer *_populate_list(const String &p_name, const List<String> &p_sections, const char *const *const p_src[], const int p_flag_single_column = 0);

	PanelContainer *_panel;
	Tree *_tpl_tree;
	RichTextLabel *_tpl_text;
	TextureRect *_logo_ramatak;
	TextureRect *_logo_godot;
	TextureRect *_title;
	Label *_version_text;
	Label *_about_text;
	StyleBoxTexture *_panel_style;
	Gradient *_gradient;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	EditorAboutRamatak();
};

#endif // EDITOR_ABOUT_RAMATAK_H
