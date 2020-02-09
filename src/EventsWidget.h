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


#ifndef EVENTSWIDGET_H
#define	EVENTSWIDGET_H
#include <gtkmm.h>
#include "ActionGUI.h"
#include "EventGUI.h"

/**List of all items. Used once in the MainWindow*/
class EventsWidget : public Gtk::VBox{
public:
    EventsWidget();
    virtual ~EventsWidget();

    /**Fills the treeview with all events from events list*/
    void InitTreeData();
    /**Refreshes settings for all events in the treeview*/
    void UpdateAll();

    Gtk::VBox main_Vbox;
    Gtk::HBox lower_button_Hbox;
    Gtk::Button add_button;
    Gtk::Button add_action_button;
    Gtk::Button remove_button;
    Gtk::ScrolledWindow scrolledwindow;

    void OnAddEventClicked();
    void OnAddActionClicked();
    void OnRemoveClicked();
    void OnSelectionChanged();
    void OnRowChosen(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
    void OnRowCollapsed(const Gtk::TreeModel::iterator& iter, const Gtk::TreeModel::Path& path);


private:
    /** Refreshes one, particular row. */
    void UpdateRow(Gtk::TreeModel::Row rowref);

    /** Used to set colors for rows. */
    void ColorizeEvent(Gtk::TreeModel::Row row);
    void UncolorizeEvent(Gtk::TreeModel::Row row);
    void ColorizeAction(Gtk::TreeModel::Row row);
    void UncolorizeAction(Gtk::TreeModel::Row row);

    enum RowType{
        EVENT,
        ACTION
    };

    /**Used for treeview*/
    class ModelColumns : public Gtk::TreeModel::ColumnRecord{
    public:
        ModelColumns(){add(col_ID);add(col_label);add(col_colour);add(col_type);add(col_prt);}
        Gtk::TreeModelColumn<int> col_ID;
        Gtk::TreeModelColumn<Glib::ustring> col_label;
        Gtk::TreeModelColumn<Gdk::RGBA> col_colour;
        Gtk::TreeModelColumn<int> col_type;
        Gtk::TreeModelColumn<int> col_prt;
    };

    void ShowActionGUI(Action* target);
    ActionGUI action_gui;

    void ShowEventGUI(Event* target);
    EventGUI event_gui;

    void UpdateColors();

    ModelColumns m_columns;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

    Gdk::RGBA background_color;
    Gdk::RGBA highlight_color;
};

#endif	/* EVENTSWIDGET_H */
