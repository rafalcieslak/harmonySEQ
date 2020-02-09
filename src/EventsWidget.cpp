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
#include "Action.h"
#include "global.h"
#include "messages.h"
#include "MainWindow.h"
#include "Files.h"

EventsWidget::EventsWidget(){
    set_size_request(200,-1);

    pack_start(main_Vbox);
    main_Vbox.pack_start(scrolledwindow);

    scrolledwindow.add(m_TreeView);
    scrolledwindow.set_policy(Gtk::POLICY_NEVER,Gtk::POLICY_ALWAYS);
    scrolledwindow.set_overlay_scrolling(false);

    main_Vbox.pack_start(lower_button_Hbox,Gtk::PACK_SHRINK);
    lower_button_Hbox.pack_end(add_button);
    lower_button_Hbox.pack_end(add_action_button);
    lower_button_Hbox.pack_end(remove_button);
    add_button.set_label(_("Add Event"));
    add_button.set_tooltip_markup(_("Adds a new <b>event</b>.\n\nEvents define on what should harmonySEQ react - examples include keypresses on computer's keyboard or MIDI input from an external controller."));
    add_button.signal_clicked().connect(std::bind(&EventsWidget::OnAddEventClicked, this));
    remove_button.set_label(_("Remove"));
    remove_button.set_tooltip_markup(_("Removes selected action or event."));
    remove_button.set_sensitive(0);
    remove_button.signal_clicked().connect(std::bind(&EventsWidget::OnRemoveClicked, this));
    add_action_button.set_sensitive(0);
    add_action_button.set_label(_("Add Action"));
    add_action_button.set_tooltip_markup(_("Adds a new <b>action</b> assigned to selected event.\n\nActions define what harmonySEQ should do on corresponding event - for example it can toggle one of the sequencers, or change it's chord."));
    add_action_button.signal_clicked().connect(std::bind(&EventsWidget::OnAddActionClicked, this));

    m_refTreeModel = Gtk::TreeStore::create(m_columns);
    m_TreeView.set_model(m_refTreeModel);
    m_TreeView.set_name("events");

    if (debugging) m_TreeView.append_column(_("ID"),m_columns.col_ID);
    //m_TreeView.append_column("prt",m_columns.col_prt);
    //m_TreeView.append_column("TYPE",m_columns.col_type);
    Gtk::CellRendererText* pRenderer = Gtk::manage(new Gtk::CellRendererText());
    int colcount = m_TreeView.append_column(_("Events"),*pRenderer);
    Gtk::TreeViewColumn * column = m_TreeView.get_column(colcount-1);
    column->add_attribute(pRenderer->property_text(),m_columns.col_label);
    column->add_attribute(pRenderer->property_cell_background_rgba(), m_columns.col_colour);

    m_TreeView.set_headers_visible(1);//showing the upper headers

    m_TreeView.signal_row_activated().connect(std::bind(&EventsWidget::OnRowChosen, this, std::placeholders::_1, std::placeholders::_2));
    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_TreeView.get_selection();
    refTreeSelection->signal_changed().connect(std::bind(&EventsWidget::OnSelectionChanged, this));
    m_TreeView.signal_row_collapsed().connect(std::bind(&EventsWidget::OnRowCollapsed, this, std::placeholders::_1, std::placeholders::_2));


    m_TreeView.set_enable_search(0);
    show_all_children(1);
    hide();//hidden at beggining;

    signal_style_updated().connect(std::bind(&EventsWidget::UpdateColors, this));
    UpdateColors();
}


EventsWidget::~EventsWidget(){
}

void EventsWidget::UpdateColors(){
    background_color = m_TreeView.get_style_context()->get_background_color();
    highlight_color = m_TreeView.get_style_context()->get_background_color(
        Gtk::STATE_FLAG_SELECTED | Gtk::STATE_FLAG_FOCUSED | Gtk::STATE_FLAG_ACTIVE);

    /* Update existing rows to use new color. */
    Gtk::TreeModel::Children children = m_refTreeModel->children();
    for(Gtk::TreeModel::Children::iterator iter = children.begin(); iter != children.end(); ++iter){
        Gtk::TreeModel::Row row = *iter;
        /* Event */
        row[m_columns.col_colour] = background_color;

        Gtk::TreeModel::Children children2 = row->children();
        for(Gtk::TreeModel::Children::iterator iter = children2.begin(); iter != children2.end(); ++iter){
            Gtk::TreeModel::Row row = *iter;
            /* Action */
            row[m_columns.col_colour] = background_color;
        }
    }
}

void EventsWidget::InitTreeData(){
    m_refTreeModel->clear();
    Gtk::TreeModel::Row row;

    for (unsigned int x = 0; x < Events.size(); x++) {
        if (!Events[x]) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = m_refTreeModel->append();
        row = *(iter);
        Event* event = Events[x];
        row[m_columns.col_ID] = x;
        row[m_columns.col_label] = event->GetLabel();
        row[m_columns.col_colour] = background_color;
        row[m_columns.col_type] = EVENT;
        row[m_columns.col_prt] = -1;

        event->on_trigger.connect(
            [=](){ DeferWorkToUIThread(
                    [=](){ ColorizeEvent(row); });});

        event->on_changed.connect(
            [=](){ DeferWorkToUIThread(
                    [=](){ UpdateRow(row); });});

        //actions
        for (unsigned int c = 0; c < event->actions.size();c++){
            if(!event->actions[c]) continue;
            Gtk::TreeModel::iterator iter_child = m_refTreeModel->append(row.children());
            Gtk::TreeModel::Row row_child = *iter_child;
            Action* action = event->actions[c];
            row_child[m_columns.col_ID] = c;
            row_child[m_columns.col_label] = action->GetLabel();
            row_child[m_columns.col_colour] = background_color;
            row_child[m_columns.col_type] = ACTION;
            row_child[m_columns.col_prt] = x;

            action->on_trigger.connect(
                [=](){ DeferWorkToUIThread(
                        [=](){ ColorizeAction(row_child); });});

            action->on_changed.connect(
                [=](){ DeferWorkToUIThread(
                        [=](){ UpdateRow(row_child); });});
        }
    }
}

void EventsWidget::OnAddEventClicked(){
    Gtk::TreeModel::iterator iter = m_refTreeModel->append();
    Gtk::TreeModel::Row row = *(iter);
    Event* event = new Event(Event::NONE,0,0);
    Events.push_back(event);
    // TODO: Store shared ptrs insted of IDs
    row[m_columns.col_ID] = Events.size()-1;
    row[m_columns.col_label] = event->GetLabel();
    row[m_columns.col_colour] = background_color;
    row[m_columns.col_type] = EVENT;
    row[m_columns.col_prt] = -1;
    event->ShowWindow();

    event->on_trigger.connect(
        [=](){ DeferWorkToUIThread(
                [=](){ ColorizeEvent(row); });});

    event->on_changed.connect(
        [=](){ DeferWorkToUIThread(
                [=](){ UpdateRow(row); });});

    m_TreeView.get_selection()->select(iter); //select the event that was just added
    Files::SetFileModified(1);
}

void EventsWidget::OnAddActionClicked(){
    int id = -1; //assert
    Gtk::TreeModel::iterator iter_selected = m_TreeView.get_selection()->get_selected();
    if(!iter_selected) return; //should not happen, button wolud not be sensitive

    Gtk::TreeModel::Row row = *iter_selected;
    Gtk::TreeModel::iterator iter_parent;
    switch (row[m_columns.col_type]){
        case EVENT:
            id = row[m_columns.col_ID];
            iter_parent = iter_selected;
            break;
        case ACTION:
            id = row[m_columns.col_prt];
            Gtk::TreePath path(iter_selected);
            path.up();
            iter_parent = m_refTreeModel->get_iter(path);
            break;
    }

    Action* action = new Action(Action::NONE);
    Events[id]->actions.push_back(action);
    int act = Events[id]->actions.size() - 1;
    row = *iter_parent;
    Gtk::TreeModel::iterator iter_new = m_refTreeModel->append(row.children());
    row = *iter_new;
    row[m_columns.col_type] = ACTION;
    row[m_columns.col_prt] = id;
    row[m_columns.col_ID] = act;
    row[m_columns.col_label] = action->GetLabel();
    row[m_columns.col_colour] = background_color;

    action->on_trigger.connect(
        [=](){ DeferWorkToUIThread(
                [=](){ ColorizeAction(row); });});

    action->on_changed.connect(
        [=](){ DeferWorkToUIThread(
                [=](){ UpdateRow(row); });});

    ShowActionGUI(action);

    row = *iter_selected;
    if (row[m_columns.col_type] == EVENT)
        m_TreeView.expand_row(m_refTreeModel->get_path(iter_selected),false); //auto expanding if possible to show the action;

    Files::SetFileModified(1);
}

void EventsWidget::ShowActionGUI(Action* target){
    action_gui.SwitchTarget(target);
    action_gui.set_transient_for(*dynamic_cast<Gtk::Window*>(get_toplevel()));
    action_gui.show();
    action_gui.raise();
}

void EventsWidget::UpdateAll(){
    InitTreeData();
}

void EventsWidget::ColorizeEvent(Gtk::TreeModel::Row row){
    row[m_columns.col_colour] = highlight_color;
    Glib::signal_timeout().connect([=](){UncolorizeEvent(row); return false;},EVENTS_FLASH_TIMEOUT,Glib::PRIORITY_DEFAULT_IDLE);
}

void EventsWidget::UncolorizeEvent(Gtk::TreeModel::Row row){
    row[m_columns.col_colour] = background_color;
}
void EventsWidget::ColorizeAction(Gtk::TreeModel::Row row){
    row[m_columns.col_colour] = highlight_color;
    Glib::signal_timeout().connect([=](){UncolorizeAction(row); return false;},EVENTS_FLASH_TIMEOUT,Glib::PRIORITY_DEFAULT_IDLE);
}

void EventsWidget::UncolorizeAction(Gtk::TreeModel::Row row){
    row[m_columns.col_colour] = background_color;
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
                // TODO: The tree model should store pointers / shared pointers to actions.
                ShowActionGUI(Events[row[m_columns.col_prt]]->actions[row[m_columns.col_ID]]);
                break;
        }
    }

}

void EventsWidget::UpdateRow(Gtk::TreeModel::Row row){
    switch (row[m_columns.col_type]){
    case EVENT:
        row[m_columns.col_label] = Events[row[m_columns.col_ID]]->GetLabel();
        break;
    case ACTION:
        row[m_columns.col_label] = Events[row[m_columns.col_prt]]->actions[row[m_columns.col_ID]]->GetLabel();
        break;
    }

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
