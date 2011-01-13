/*
    Copyright (C) 2010 Rafał Cieślak

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

#include "SettingsWindow.h"

SettingsWindow::SettingsWindow(){
    set_title(_("Settings"));
    set_border_width(10);

    add(main_vbox);
    main_vbox.pack_end(lower_hbox,Gtk::PACK_SHRINK);

    lower_hbox.pack_end(cancel_button);
    lower_hbox.pack_end(ok_button);

    cancel_button.set_label(_("Cancel"));
    ok_button.set_label(_("OK"));

    show_all();
    hide();
}

SettingsWindow::~SettingsWindow(){
    
}