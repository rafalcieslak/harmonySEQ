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

#include "EventsWindow.h"
#include "Event.h"


EventsWindow::EventsWindow(){
    char temp[30];
    sprintf(temp, "harmonySEQ %s - Events", VERSION);
    set_title(temp);
    set_border_width(5);

    add(main_Vbox);
    main_Vbox.pack_start(m_TreeView);
    main_Vbox.pack_start(lower_button_Hbox);
    lower_button_Hbox.pack_end(add_button);
    add_button.set_label(_("Add"));


    m_refTreeModel = Gtk::ListStore::create(m_columns);
    m_TreeView.set_model(m_refTreeModel);
    
    m_TreeView.append_column(_("ID"),m_columns.col_ID);
    m_TreeView.append_column(_("Event"),m_columns.col_label);

    m_TreeView.set_headers_visible(0);//hiding the upper headers

    show_all_children(1);
    hide();//hidden at beggining;
}


EventsWindow::~EventsWindow(){
}

void EventsWindow::InitTreeData(){

    m_refTreeModel->clear();
    Gtk::TreeModel::Row row;
    int rowcount = 0;
    for (unsigned int x = 0; x < events.size(); x++) {
        if (!events[x]) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = m_refTreeModel->append();
        row = *(iter);
        row[m_columns.col_ID] = x;
        row[m_columns.col_label] = events[x]->GetLabel();
        Gtk::TreeRowReference rowref(m_refTreeModel, m_refTreeModel->get_path(iter));
        events[x]->row_in_event_window = rowref;
        rowcount++;
    }


}