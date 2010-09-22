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
#include "ActionGUI.h"


ActionGUI::ActionGUI(Action *prt){
    parent = prt;
    set_title(_("Action"));

    set_border_width(5);
    set_transient_for(*eventswindow);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
}


ActionGUI::ActionGUI(const ActionGUI& orig){
}


ActionGUI::~ActionGUI(){
}

