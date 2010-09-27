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
    lower_button_Hbox.pack_end(add_action_button);
    lower_button_Hbox.pack_end(remove_button);
    add_button.set_label(_("Add Event"));
    add_button.signal_clicked().connect(mem_fun(*this,&EventsWindow::OnAddEventClicked));
    remove_button.set_label(_("Remove"));
    remove_button.set_sensitive(0);
    remove_button.signal_clicked().connect(mem_fun(*this,&EventsWindow::OnRemoveClicked));
    add_action_button.set_sensitive(0);
    add_action_button.set_label(_("Add Action"));
    add_action_button.signal_clicked().connect(mem_fun(*this,&EventsWindow::OnAddActionClicked));

    m_refTreeModel = Gtk::TreeStore::create(m_columns);
    m_TreeView.set_model(m_refTreeModel);
     
    if (debugging) m_TreeView.append_column(_("ID"),m_columns.col_ID);
    //m_TreeView.append_column("prt",m_columns.col_prt);
    //m_TreeView.append_column("TYPE",m_columns.col_type);
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());
    int colcount = m_TreeView.append_column(_("Event"),*pRenderer);
    Gtk::TreeViewColumn * column = m_TreeView.get_column(colcount-1);
    column->add_attribute(pRenderer->property_cell_background(),m_columns.col_colour);
    column->add_attribute(pRenderer->property_text(),m_columns.col_label);
    column->add_attribute(pRenderer->property_cell_background(),m_columns.col_colour);

    m_TreeView.set_headers_visible(0);//hiding the upper headers

    m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this, &EventsWindow::OnRowChosen));
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_TreeView.get_selection();
    refTreeSelection->signal_changed().connect(mem_fun(*this, &EventsWindow::OnSelectionChanged));
    m_TreeView.signal_row_collapsed().connect(mem_fun(*this, &EventsWindow::OnRowCollapsed));
    m_TreeView.signal_key_press_event().connect(&FindAndProcessEventsKeyPress);
    signal_key_press_event().connect(&FindAndProcessEventsKeyPress);
    m_TreeView.set_enable_search(0);
    show_all_children(1);
    hide();//hidden at beggining;
}


EventsWindow::~EventsWindow(){
}

void EventsWindow::InitTreeData(){

    m_refTreeModel->clear();
    Gtk::TreeModel::Row row;
    for (unsigned int x = 0; x < events.size(); x++) {
        if (!events[x]) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = m_refTreeModel->append();
        row = *(iter);
        row[m_columns.col_ID] = x;
        row[m_columns.col_label] = events[x]->GetLabel();
        row[m_columns.col_colour] = "white";
        row[m_columns.col_type] = EVENT;
        row[m_columns.col_prt] = -1;
        Gtk::TreeRowReference rowref(m_refTreeModel, m_refTreeModel->get_path(iter));
        events[x]->row_in_event_window = rowref;
        //actions
        for (unsigned int c = 0; c < events[x]->actions.size();c++){
            if(!events[x]->actions[c]) continue;
            Gtk::TreeModel::iterator iter_child = m_refTreeModel->append(row.children());
            Gtk::TreeModel::Row row_child = *iter_child;
            row_child[m_columns.col_ID] = c;
            row_child[m_columns.col_label] = events[x]->actions[c]->GetLabel();
            row_child[m_columns.col_colour] = "beige";
            row_child[m_columns.col_type] = ACTION;
            row_child[m_columns.col_prt] = x;
            Gtk::TreeRowReference rowref_child(m_refTreeModel, m_refTreeModel->get_path(iter_child));
            events[x]->actions[c]->row_in_event_window = rowref_child;
        }
         

    }

    resize(2,2);
}

void EventsWindow::RefreshAll(){
    InitTreeData();
}

void EventsWindow::ColorizeEvent(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    if (!row) {
        *err << _("Error - cannot colorize triggered event: row is empty.\n");
        return;
    }

    row[m_columns.col_colour] = "royal blue";
    Glib::signal_timeout().connect(sigc::bind(mem_fun(*this,&EventsWindow::UncolorizeEvent),rowref),EVENTS_FLASH_TIMEOUT);
}

bool EventsWindow::UncolorizeEvent(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    row[m_columns.col_colour] = "white";
    return 0;
}
void EventsWindow::ColorizeAction(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    if (!row) {
        *err << _("Error - cannot colorize triggered action: row is empty.\n");
        return;
    }

    row[m_columns.col_colour] = "light blue";
    Glib::signal_timeout().connect(sigc::bind(mem_fun(*this,&EventsWindow::UncolorizeAction),rowref),EVENTS_FLASH_TIMEOUT);
}

bool EventsWindow::UncolorizeAction(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    row[m_columns.col_colour] = "beige";
    return 0;
}
void EventsWindow::OnRowChosen(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column){

    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter;
        //*dbg << row[m_columns.col_label] << " - " << row[m_columns.col_ID] << " - p: " << row[m_columns.col_prt] << ENDL;
        switch (row[m_columns.col_type]){
            case EVENT:
                events[row[m_columns.col_ID]]->ShowWindow();
                break;
            case ACTION:
                events[row[m_columns.col_prt]]->actions[row[m_columns.col_ID]]->ShowWindow();
                break;
        }
    }

}

void EventsWindow::UpdateRow(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));
        switch (row[m_columns.col_type]){
            case EVENT:
                row[m_columns.col_label] = events[row[m_columns.col_ID]]->GetLabel();
                break;
            case ACTION:
                row[m_columns.col_label] = events[row[m_columns.col_prt]]->actions[row[m_columns.col_ID]]->GetLabel();
                break;
        }
    
}

void EventsWindow::OnAddEventClicked(){

    Gtk::TreeModel::iterator iter = m_refTreeModel->append();
    Gtk::TreeModel::Row row = *(iter);
    events.push_back(new Event(Event::NONE,0,0));
    row[m_columns.col_ID] = events.size()-1;
    row[m_columns.col_label] = events[events.size()-1]->GetLabel();
    row[m_columns.col_colour] = "white";
    row[m_columns.col_type] = EVENT;
    row[m_columns.col_prt] = -1;
    Gtk::TreeRowReference rowref(m_refTreeModel, m_refTreeModel->get_path(iter));
    events[events.size()-1]->row_in_event_window = rowref;
    events[events.size()-1]->ShowWindow();

}

void EventsWindow::OnRemoveClicked(){

    Gtk::TreeModel::iterator iter = *(m_TreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id, prt;
    switch (row[m_columns.col_type]){
        case EVENT:

            id = row[m_columns.col_ID];
            m_refTreeModel->erase(iter);

            delete events[id];
            events[id] = NULL;
            break;
        case ACTION:
            id = row[m_columns.col_ID];
            prt = row[m_columns.col_prt];
            m_refTreeModel->erase(iter);

            //TODO: checck whether we cannot do the following by erasing an item from action's std::vecctor
            delete events[prt]->actions[id];
            events[prt]->actions[id] = NULL;
            break;
    }
    resize(2,2);

}

void EventsWindow::OnSelectionChanged(){

    Gtk::TreeModel::iterator iter = m_TreeView.get_selection()->get_selected();
    if(iter){
        remove_button.set_sensitive(1);
        add_action_button.set_sensitive(1);
    }else{
        remove_button.set_sensitive(0);
        add_action_button.set_sensitive(0);

    }

}

void EventsWindow::OnRowCollapsed(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path){

    resize(2,2);
}

void EventsWindow::OnAddActionClicked(){
    int id;
    Gtk::TreeModel::iterator iter_selected = m_TreeView.get_selection()->get_selected();
    if(!iter_selected) return; //should not happen, button wolud not be sensitive

    Gtk::TreeModel::Row row = *iter_selected;
    switch (row[m_columns.col_type]){
        case EVENT:
            id = row[m_columns.col_ID];
            break;
        case ACTION:
            id = row[m_columns.col_prt];
            break;
    }

    events[id]->actions.push_back(new Action(Action::NONE));
    int act = events[id]->actions.size() - 1;
    Gtk::TreeModel::iterator iter_parent = m_refTreeModel->get_iter(events[id]->row_in_event_window.get_path());
    row = *iter_parent;
    Gtk::TreeModel::iterator iter_new = m_refTreeModel->append(row.children());
    row = *iter_new;
    row[m_columns.col_type] = ACTION;
    row[m_columns.col_prt] = id;
    row[m_columns.col_ID] = act;
    row[m_columns.col_label] = events[id]->actions[act]->GetLabel();
    row[m_columns.col_colour] = "beige";
    Gtk::TreeRowReference rowref_child(m_refTreeModel, m_refTreeModel->get_path(iter_new));
    events[id]->actions[act]->row_in_event_window = rowref_child;

    events[id]->actions[act]->ShowWindow();

    row = *iter_selected;
    if (row[m_columns.col_type] == EVENT)
        m_TreeView.expand_row(m_refTreeModel->get_path(iter_selected),false); //auto expanding if possible to show the action;
}