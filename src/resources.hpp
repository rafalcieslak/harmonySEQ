 /*
    Copyright (C) 2020 Rafał Cieślak

    This file is part of harmonySEQ.

    HarmonySEQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HarmonySEQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <glibmm/ustring.h>
#include <gdkmm/pixbuf.h>


void LoadIcons(std::string data_path);

extern Glib::RefPtr< Gdk::Pixbuf > harmonySEQ_logo_48;
extern Glib::RefPtr< Gdk::Pixbuf > metronome_icon_24;
extern Glib::RefPtr< Gdk::Pixbuf > icon_add_note_seq;
extern Glib::RefPtr< Gdk::Pixbuf > icon_add_ctrl_seq;
extern Glib::RefPtr< Gdk::Pixbuf > icon_slope_linear;
extern Glib::RefPtr< Gdk::Pixbuf > icon_slope_flat;

#endif // RESOURCES_HPP
