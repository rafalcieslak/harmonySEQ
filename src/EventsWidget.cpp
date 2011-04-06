/*
    Copyright (C) 2010, 2011 Rafał Cieślak

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


#include "EventsWidget.h"
#include "Event.h"
#include "global.h"
#include "messages.h"
#include "Files.h"

EventsWidget::EventsWidget(){
    set_size_request(250,-1);

    pack_start(main_Vbox);
    main_Vbox.pack_start(scrolledwindow);
    scrolledwindow.add(m_TreeView);
    scrolledwindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC);
    main_Vbox.pack_start(lower_button_Hbox,Gtk::PACK_SHRINK);
    lower_button_Hbox.pack_end(add_button);
    lower_button_Hbox.pack_end(add_action_button);
    lower_button_Hbox.pack_end(remove_button);
    add_button.set_label(_("Add Event"));
    add_button.signal_clicked().connect(sigc::mem_fun(*this,&EventsWidget::OnAddEventClicked));
    remove_button.set_label(_("Remove"));
    remove_button.set_sensitive(0);
    remove_button.signal_clicked().connect(sigc::mem_fun(*this,&EventsWidget::OnRemoveClicked));
    add_action_button.set_sensitive(0);
    add_action_button.set_label(_("Add Action"));
    add_action_button.signal_clicked().connect(sigc::mem_fun(*this,&EventsWidget::OnAddActionClicked));

    m_refTreeModel = Gtk::TreeStore::create(m_columns);
    m_TreeView.set_model(m_refTreeModel);

    if (debugging) m_TreeView.append_column(_("ID"),m_columns.col_ID);
    //m_TreeView.append_column("prt",m_columns.col_prt);
    //m_TreeView.append_column("TYPE",m_columns.col_type);
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());
    int colcount = m_TreeView.append_column(_("Events"),*pRenderer);
    Gtk::TreeViewColumn * column = m_TreeView.get_column(colcount-1);
    column->add_attribute(pRenderer->property_cell_background(),m_columns.col_colour);
    column->add_attribute(pRenderer->property_text(),m_columns.col_label);
    column->add_attribute(pRenderer->property_cell_background(),m_columns.col_colour);

    m_TreeView.set_headers_visible(1);//showing the upper headers

    m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this, &EventsWidget::OnRowChosen));
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_TreeView.get_selection();
    refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &EventsWidget::OnSelectionChanged));
    m_TreeView.signal_row_collapsed().connect(sigc::mem_fun(*this, &EventsWidget::OnRowCollapsed));
    m_TreeView.signal_key_press_event().connect(&FindAndProcessEventsKeyPress);
    signal_key_press_event().connect(&FindAndProcessEventsKeyPress);
    m_TreeView.set_enable_search(0);
    show_all_children(1);
    hide();//hidden at beggining;
}


EventsWidget::~EventsWidget(){
}


void EventsWidget::InitTreeData(){

    m_refTreeModel->clear();
    Gtk::TreeModel::Row row;
    for (unsigned int x = 0; x < Events.size(); x++) {
        if (!Events[x]) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = m_refTreeModel->append();
        row = *(iter);
        row[m_columns.col_ID] = x;
        row[m_columns.col_label] = Events[x]->GetLabel();
        row[m_columns.col_colour] = "white";
        row[m_columns.col_type] = EVENT;
        row[m_columns.col_prt] = -1;
        Gtk::TreeRowReference rowref(m_refTreeModel, m_refTreeModel->get_path(iter));
        Events[x]->row_in_event_widget = rowref;
        //actions
        for (unsigned int c = 0; c < Events[x]->actions.size();c++){
            if(!Events[x]->actions[c]) continue;
            Gtk::TreeModel::iterator iter_child = m_refTreeModel->append(row.children());
            Gtk::TreeModel::Row row_child = *iter_child;
            row_child[m_columns.col_ID] = c;
            row_child[m_columns.col_label] = Events[x]->actions[c]->GetLabel();
            row_child[m_columns.col_colour] = "beige";
            row_child[m_columns.col_type] = ACTION;
            row_child[m_columns.col_prt] = x;
            Gtk::TreeRowReference rowref_child(m_refTreeModel, m_refTreeModel->get_path(iter_child));
            Events[x]->actions[c]->row_in_event_widget = rowref_child;
        }
    }
}

void EventsWidget::UpdateAll(){
    InitTreeData();
}

void EventsWidget::ColorizeEvent(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    if (!row) {
        *err << _("Error - cannot colorize triggered event: row is empty.\n");
        return;
    }

    row[m_columns.col_colour] = "royal blue";
    Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this,&EventsWidget::UncolorizeEvent),rowref),EVENTS_FLASH_TIMEOUT);
}

bool EventsWidget::UncolorizeEvent(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    row[m_columns.col_colour] = "white";
    return 0;
}
void EventsWidget::ColorizeAction(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    if (!row) {
        *err << _("Error - cannot colorize triggered action: row is empty.\n");
        return;
    }

    row[m_columns.col_colour] = "light blue";
    Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this,&EventsWidget::UncolorizeAction),rowref),EVENTS_FLASH_TIMEOUT);
}

bool EventsWidget::UncolorizeAction(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    row[m_columns.col_colour] = "beige";
    return 0;
}
void EventsWidget::OnRowChosen(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column){

    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter;
        switch (row[m_columns.col_type]){
            case EVENT:
                Events[row[m_columns.col_ID]]->ShowWindow();
                break;
            case ACTION:
                Events[row[m_columns.col_prt]]->actions[row[m_columns.col_ID]]->GUIShowWindow();
                break;
        }
    }

}

void EventsWidget::UpdateRow(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));
        switch (row[m_columns.col_type]){
            case EVENT:
                row[m_columns.col_label] = Events[row[m_columns.col_ID]]->GetLabel();
                break;
            case ACTION:
                row[m_columns.col_label] = Events[row[m_columns.col_prt]]->actions[row[m_columns.col_ID]]->GetLabel();
                break;
        }

}

void EventsWidget::OnAddEventClicked(){

    Gtk::TreeModel::iterator iter = m_refTreeModel->append();
    Gtk::TreeModel::Row row = *(iter);
    Events.push_back(new Event(Event::NONE,0,0));
    row[m_columns.col_ID] = Events.size()-1;
    row[m_columns.col_label] = Events[Events.size()-1]->GetLabel();
    row[m_columns.col_colour] = "white";
    row[m_columns.col_type] = EVENT;
    row[m_columns.col_prt] = -1;
    Gtk::TreeRowReference rowref(m_refTreeModel, m_refTreeModel->get_path(iter));
    Events[Events.size()-1]->row_in_event_widget = rowref;
    Events[Events.size()-1]->ShowWindow();

    Files::SetFileModified(1);
}

void EventsWidget::OnRemoveClicked(){

    Gtk::TreeModel::iterator iter = *(m_TreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id, prt;
    switch (row[m_columns.col_type]){
        case EVENT:

            id = row[m_columns.col_ID];
            m_refTreeModel->erase(iter);

            delete Events[id];
            Events[id] = NULL;
            break;
        case ACTION:
            id = row[m_columns.col_ID];
            prt = row[m_columns.col_prt];
            m_refTreeModel->erase(iter);

            //TODO: checck whether we cannot do the following by erasing an item from action's std::vecctor
            delete Events[prt]->actions[id];
            Events[prt]->actions[id] = NULL;
            break;
    }

    Files::SetFileModified(1);
}

void EventsWidget::OnSelectionChanged(){

    Gtk::TreeModel::iterator iter = m_TreeView.get_selection()->get_selected();
    if(iter){
        remove_button.set_sensitive(1);
        add_action_button.set_sensitive(1);
    }else{
        remove_button.set_sensitive(0);
        add_action_button.set_sensitive(0);

    }

}

void EventsWidget::OnRowCollapsed(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path){
    //void!
}

void EventsWidget::OnAddActionClicked(){
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

    Events[id]->actions.push_back(new Action(Action::NONE));
    int act = Events[id]->actions.size() - 1;
    Gtk::TreeModel::iterator iter_parent = m_refTreeModel->get_iter(Events[id]->row_in_event_widget.get_path());
    row = *iter_parent;
    Gtk::TreeModel::iterator iter_new = m_refTreeModel->append(row.children());
    row = *iter_new;
    row[m_columns.col_type] = ACTION;
    row[m_columns.col_prt] = id;
    row[m_columns.col_ID] = act;
    row[m_columns.col_label] = Events[id]->actions[act]->GetLabel();
    row[m_columns.col_colour] = "beige";
    Gtk::TreeRowReference rowref_child(m_refTreeModel, m_refTreeModel->get_path(iter_new));
    Events[id]->actions[act]->row_in_event_widget = rowref_child;

    Events[id]->actions[act]->GUIShowWindow();

    row = *iter_selected;
    if (row[m_columns.col_type] == EVENT)
        m_TreeView.expand_row(m_refTreeModel->get_path(iter_selected),false); //auto expanding if possible to show the action;

    Files::SetFileModified(1);
}