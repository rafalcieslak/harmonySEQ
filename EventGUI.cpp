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

#include "EventGUI.h"
#include "Event.h"

EventGUI::EventGUI(Event *prt){
    parent = prt;

    set_title(parent->GetLabel());
    set_border_width(0);
    add(main_box);
    main_box.pack_start(line_one);
    line_one.pack_start(label_one,Gtk::PACK_SHRINK);
    label_one.set_text(_("Type:"));
    line_one.pack_start(Types_combo,Gtk::PACK_SHRINK);

    show_all_children(1);
    hide();
}


EventGUI::~EventGUI(){
}

