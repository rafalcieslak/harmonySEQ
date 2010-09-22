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
#include "Action.h"


ActionGUI::ActionGUI(Action *prt){
    parent = prt;
    set_title(_("Action"));

    set_border_width(5);
    set_transient_for(*eventswindow);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

    add(main_box);

    main_box.pack_start(line_type);
    main_box.pack_start(line_note);
    main_box.pack_start(separator);
    main_box.pack_start(label_preview);
    main_box.pack_start(ok_button);

    line_type.pack_start(label_type);

    //

    label_type.set_text(_("Type:"));
    ok_button.set_label(_("OK"));
    
    ok_button.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnOKClicked));


    signal_show().connect(mem_fun(*this,&ActionGUI::OnShow));

    label_preview.set_text(parent->GetLabel());
    show_all_children(1);
    TypeChanged(); // to hide some of widgets according to the type
    hide();

}


ActionGUI::ActionGUI(const ActionGUI& orig){
}


ActionGUI::~ActionGUI(){
}

void ActionGUI::OnOKClicked(){
    hide();

}

void ActionGUI::OnShow(){
    UpdateValues();

}

void ActionGUI::UpdateValues(){


}

void ActionGUI::TypeChanged(){

    
}