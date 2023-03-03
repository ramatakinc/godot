/*************************************************************************/
/*  editor_about_ramatak.cpp                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "editor_about_ramatak.h"

#include "core/authors.gen.h"
#include "core/color.h"
#include "core/license.gen.h"
#include "core/version.h"
#include "editor_node.h"
#include "scene/gui/nine_patch_rect.h"
#include "scene/gui/scroll_container.h"
#include "scene/resources/gradient.h"
#include "scene/resources/style_box.h"
#include "scene/resources/texture.h"

// The metadata key used to store and retrieve the version text to copy to the clipboard.
static const String META_TEXT_TO_COPY = "text_to_copy";

void EditorAboutRamatak::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			int dialog_margin = get_constant("margin", "Dialogs");
			_panel_style->set_expand_margin_size_individual(dialog_margin, 0, dialog_margin, 0);

			_logo_ramatak->set_texture(get_icon("FaceAbout", "EditorIcons"));
			Ref<Font> font = get_font("source", "EditorFonts");
			_tpl_text->add_font_override("normal_font", font);
			_version_text->add_font_override("font", get_font("about", "EditorFonts"));
			_about_text->add_font_override("font", get_font("about", "EditorFonts"));
			_logo_godot->set_texture(get_icon("LogoGodot", "EditorIcons"));
			[[fallthrough]];
		}
		case NOTIFICATION_THEME_CHANGED: {
			const bool dark_theme = EditorSettings::get_singleton()->is_dark_theme();
			Color font_color = dark_theme ? Color(0.94, 0.94, 0.94) : Color(0.02, 0.02, 0.02);
			Color font_outline_color = dark_theme ? Color(0.32, 0.32, 0.32, 0.5) : Color(0.94, 0.94, 0.94, 0.8);

			_gradient->set_color(0, dark_theme ? Color(0.07, 0.82, 0.49) : Color(0.05, 0.03, 0.22));
			_logo_ramatak->set_modulate(dark_theme ? Color(0.05, 0.03, 0.22) : Color(0.07, 0.82, 0.49));
			_title->set_texture(get_icon(dark_theme ? "TitleDark" : "TitleLight", "EditorIcons"));
			_version_text->add_color_override("font_color", font_color);
			_version_text->add_color_override("font_outline_modulate", font_outline_color);
			_about_text->add_color_override("font_color", font_color);
			_about_text->add_color_override("font_outline_modulate", font_outline_color);
		} break;
	}
}

void EditorAboutRamatak::_license_tree_selected() {
	TreeItem *selected = _tpl_tree->get_selected();
	_tpl_text->scroll_to_line(0);
	_tpl_text->set_text(selected->get_metadata(0));
}

void EditorAboutRamatak::_link_clicked(const String &p_link) {
	OS::get_singleton()->shell_open(p_link);
}

void EditorAboutRamatak::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_license_tree_selected"), &EditorAboutRamatak::_license_tree_selected);
	ClassDB::bind_method(D_METHOD("_link_clicked"), &EditorAboutRamatak::_link_clicked);
}

ScrollContainer *EditorAboutRamatak::_populate_list(const String &p_name, const List<String> &p_sections, const char *const *const p_src[], const int p_flag_single_column) {
	ScrollContainer *sc = memnew(ScrollContainer);
	sc->set_name(p_name);
	sc->set_v_size_flags(Control::SIZE_EXPAND);

	VBoxContainer *vbc = memnew(VBoxContainer);
	vbc->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	sc->add_child(vbc);

	for (int i = 0; i < p_sections.size(); i++) {
		bool single_column = p_flag_single_column & 1 << i;
		const char *const *names_ptr = p_src[i];
		if (*names_ptr) {
			Label *lbl = memnew(Label);
			lbl->set_text(p_sections[i]);
			vbc->add_child(lbl);

			ItemList *il = memnew(ItemList);
			il->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			il->set_same_column_width(true);
			il->set_auto_height(true);
			il->set_mouse_filter(Control::MOUSE_FILTER_IGNORE);
			il->add_constant_override("hseparation", 16 * EDSCALE);
			while (*names_ptr) {
				il->add_item(String::utf8(*names_ptr++), nullptr, false);
			}
			il->set_max_columns(il->get_item_count() < 4 || single_column ? 1 : 16);
			vbc->add_child(il);

			HSeparator *hs = memnew(HSeparator);
			hs->set_modulate(Color(0, 0, 0, 0));
			vbc->add_child(hs);
		}
	}

	return sc;
}

EditorAboutRamatak::EditorAboutRamatak() {
	set_title("Ramatak Mobile Studio");
	set_hide_on_ok(true);
	set_custom_minimum_size(Size2(800, 0) * EDSCALE);

	_panel = memnew(PanelContainer);
	_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(_panel);

	_panel_style = memnew(StyleBoxTexture);
	_panel_style->set_default_margin(MARGIN_TOP, 8 * EDSCALE);
	_panel_style->set_default_margin(MARGIN_BOTTOM, 8 * EDSCALE);
	_panel->add_style_override("panel", _panel_style);

	GradientTexture *grad_tex = memnew(GradientTexture);
	_gradient = memnew(Gradient);
	_gradient->set_color(1, Color(0, 0, 0, 0));
	_gradient->add_point(0.8, Color(0, 0, 0, 0));
	grad_tex->set_gradient(_gradient);
	_panel_style->set_texture(grad_tex);

	VBoxContainer *vbc = memnew(VBoxContainer);
	_panel->add_child(vbc);

	HBoxContainer *hbc = memnew(HBoxContainer);
	hbc->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	hbc->set_alignment(BoxContainer::ALIGN_CENTER);
	hbc->add_constant_override("separation", 30 * EDSCALE);
	vbc->add_child(hbc);

	_logo_ramatak = memnew(TextureRect);
	_logo_ramatak->set_expand(true);
	_logo_ramatak->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	_logo_ramatak->set_custom_minimum_size(Size2(200, 200) * EDSCALE);
	hbc->add_child(_logo_ramatak);

	VBoxContainer *info_vbc = memnew(VBoxContainer);
	info_vbc->set_alignment(VBoxContainer::ALIGN_CENTER);
	hbc->add_child(info_vbc);

	HBoxContainer *title_hbc = memnew(HBoxContainer);
	info_vbc->add_child(title_hbc);

	_title = memnew(TextureRect);
	title_hbc->add_child(_title);

	_version_text = memnew(Label);
	_version_text->set_text("v" + itos(VERSION_MAJOR) + "." + itos(VERSION_MINOR) + "." + itos(VERSION_PATCH));
	_version_text->set_v_size_flags(Control::SIZE_SHRINK_END);
	title_hbc->add_child(_version_text);

	_about_text = memnew(Label);
	_about_text->set_text(String::utf8("\xc2\xa9 Ramatak Inc.\n\n") + TTR("Powered by Godot Engine") + "\n" +
			TTR(String::utf8("\xc2\xa9 Juan Linietsky, Ariel Manzur, and Godot Engine contributors.")) + "\n");
	info_vbc->add_child(_about_text);

	HBoxContainer *hbc_buttons = memnew(HBoxContainer);
	hbc_buttons->set_h_size_flags(Control::SIZE_SHRINK_CENTER);
	hbc_buttons->add_constant_override("separation", 16);
	vbc->add_child(hbc_buttons);

	Button *show_godot = memnew(Button);
	show_godot->set_text("About Godot");
	hbc_buttons->add_child(show_godot);

	Button *show_licenses = memnew(Button);
	show_licenses->set_text("Third-party Licenses");
	hbc_buttons->add_child(show_licenses);

	// About Godot

	AcceptDialog *about_godot = memnew(AcceptDialog);
	about_godot->set_title("About Godot");
	about_godot->set_hide_on_ok(true);
	about_godot->set_custom_minimum_size(Size2(760, 200) * EDSCALE);
	add_child(about_godot);
	show_godot->connect("pressed", about_godot, "popup_centered");

	HBoxContainer *hbc_godot = memnew(HBoxContainer);
	about_godot->add_child(hbc_godot);

	_logo_godot = memnew(TextureRect);
	_logo_godot->set_expand(true);
	_logo_godot->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	_logo_godot->set_custom_minimum_size(Size2(300, 0) * EDSCALE);
	_logo_godot->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	hbc_godot->add_child(_logo_godot);

	RichTextLabel *desc_text = memnew(RichTextLabel);
	desc_text->set_use_bbcode(true);
	desc_text->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	StyleBoxEmpty *rtl_style = memnew(StyleBoxEmpty);
	rtl_style->set_default_margin(MARGIN_LEFT, 3 * EDSCALE);
	rtl_style->set_default_margin(MARGIN_RIGHT, 3 * EDSCALE);
	rtl_style->set_default_margin(MARGIN_TOP, 3 * EDSCALE);
	rtl_style->set_default_margin(MARGIN_BOTTOM, 3 * EDSCALE);
	desc_text->add_style_override("normal", rtl_style);
	desc_text->set_bbcode(vformat(TTR("Ramatak Mobile Studio is a modified version of the [url=%s]Godot Engine[/url], a free, libre, and open source cross-platform game engine for creation of 2D and 3D games.\n\nYou can help the original project by contributing with [url=%s]code[/url], or via a [url=%s]donation[/url]."),
			"https://godotengine.org", "https://github.com/godotengine/godot", "https://godotengine.org/donate/"));
	desc_text->connect("meta_clicked", this, "_link_clicked");
	hbc_godot->add_child(desc_text);

	// Third-party Licenses

	AcceptDialog *thirdparty_licenses = memnew(AcceptDialog);
	thirdparty_licenses->set_title("Third-party Licenses");
	thirdparty_licenses->set_resizable(true);
	thirdparty_licenses->set_hide_on_ok(true);
	thirdparty_licenses->set_custom_minimum_size(Size2(600, 300) * EDSCALE);
	add_child(thirdparty_licenses);
	show_licenses->connect("pressed", thirdparty_licenses, "popup_centered", varray(Size2(700, 600 * EDSCALE)));

	ScrollContainer *scroll = memnew(ScrollContainer);
	thirdparty_licenses->add_child(scroll);

	VBoxContainer *license_thirdparty = memnew(VBoxContainer);
	license_thirdparty->set_name(TTR("Third-party Licenses"));
	license_thirdparty->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	license_thirdparty->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->add_child(license_thirdparty);

	Label *tpl_label = memnew(Label);
	tpl_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tpl_label->set_autowrap(true);
	tpl_label->set_text(TTR("The following is an exhaustive list of all such third-party components used in this software with their respective copyright statements and license terms."));
	tpl_label->set_size(Size2(630, 1) * EDSCALE);
	license_thirdparty->add_child(tpl_label);

	HSplitContainer *tpl_hbc = memnew(HSplitContainer);
	tpl_hbc->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tpl_hbc->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tpl_hbc->set_split_offset(240 * EDSCALE);
	license_thirdparty->add_child(tpl_hbc);

	_tpl_tree = memnew(Tree);
	_tpl_tree->set_hide_root(true);
	TreeItem *root = _tpl_tree->create_item();
	TreeItem *tpl_ti_all = _tpl_tree->create_item(root);
	tpl_ti_all->set_text(0, TTR("All Components"));
	TreeItem *tpl_ti_tp = _tpl_tree->create_item(root);
	tpl_ti_tp->set_text(0, TTR("Components"));
	tpl_ti_tp->set_selectable(0, false);
	TreeItem *tpl_ti_lc = _tpl_tree->create_item(root);
	tpl_ti_lc->set_text(0, TTR("Licenses"));
	tpl_ti_lc->set_selectable(0, false);
	String long_text = "";

	for (int component_index = 0; component_index < COPYRIGHT_INFO_COUNT; component_index++) {
		const ComponentCopyright &component = COPYRIGHT_INFO[component_index];
		TreeItem *ti = _tpl_tree->create_item(tpl_ti_tp);
		String component_name = component.name;
		ti->set_text(0, component_name);
		String text = component_name + "\n";
		long_text += "- " + component_name + "\n";
		for (int part_index = 0; part_index < component.part_count; part_index++) {
			const ComponentCopyrightPart &part = component.parts[part_index];
			text += "\n    Files:";
			for (int file_num = 0; file_num < part.file_count; file_num++) {
				text += "\n        " + String(part.files[file_num]);
			}
			String copyright;
			for (int copyright_index = 0; copyright_index < part.copyright_count; copyright_index++) {
				copyright += String::utf8("\n    \xc2\xa9 ") + String::utf8(part.copyright_statements[copyright_index]);
			}
			text += copyright;
			long_text += copyright;
			String license = "\n    License: " + String(part.license) + "\n";
			text += license;
			long_text += license + "\n";
		}
		ti->set_metadata(0, text);
	}

	for (int i = 0; i < LICENSE_COUNT; i++) {
		TreeItem *ti = _tpl_tree->create_item(tpl_ti_lc);
		String licensename = String(LICENSE_NAMES[i]);
		ti->set_text(0, licensename);
		long_text += "- " + licensename + "\n\n";
		String licensebody = String(LICENSE_BODIES[i]);
		ti->set_metadata(0, licensebody);
		long_text += "    " + licensebody.replace("\n", "\n    ") + "\n\n";
	}

	tpl_ti_all->set_metadata(0, long_text);
	tpl_hbc->add_child(_tpl_tree);

	_tpl_text = memnew(RichTextLabel);
	_tpl_text->add_constant_override("line_separation", 6 * EDSCALE);
	_tpl_text->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	_tpl_text->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tpl_hbc->add_child(_tpl_text);

	_tpl_tree->connect("item_selected", this, "_license_tree_selected");
	tpl_ti_all->select(0);
	_tpl_text->set_text(tpl_ti_all->get_metadata(0));
}
