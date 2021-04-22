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

#include "resources.hpp"

#include <string>

Glib::RefPtr< Gdk::Pixbuf > harmonySEQ_logo_48;
Glib::RefPtr< Gdk::Pixbuf > metronome_icon_24;
Glib::RefPtr< Gdk::Pixbuf > icon_add_note_seq;
Glib::RefPtr< Gdk::Pixbuf > icon_add_ctrl_seq;
Glib::RefPtr< Gdk::Pixbuf > icon_slope_linear;
Glib::RefPtr< Gdk::Pixbuf > icon_slope_flat;


void LoadIcons(std::string data_path){
    harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/48x48/apps/harmonyseq.png");
    metronome_icon_24 = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/24x24/metronome.png");
    icon_add_ctrl_seq = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/24x24/add_ctrl_seq.png");
    icon_add_note_seq = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/24x24/add_note_seq.png");
    icon_slope_flat = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/16x16/slope_flat.png");
    icon_slope_linear = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/16x16/slope_linear.png");
}
