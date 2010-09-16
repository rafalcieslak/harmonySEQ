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

#ifndef EVENTSWINDOW_H
#define	EVENTSWINDOW_H
#include "global.h"


class EventsWindow : public Gtk::Window {
public:
    EventsWindow();
    virtual ~EventsWindow();
    void InitTreeData();
    
    Gtk::VBox main_Vbox;
    Gtk::HBox lower_button_Hbox;
    Gtk::Button add_button;

    void ColorizeRow(Gtk::TreeRowReference rowref);
    bool UncolorizeRow(Gtk::TreeRowReference rowref);

    class ModelColumns : public Gtk::TreeModel::ColumnRecord{
    public:
        ModelColumns(){add(col_ID);add(col_label);add(col_colour);}
        Gtk::TreeModelColumn<int> col_ID;
        Gtk::TreeModelColumn<Glib::ustring> col_label;
        Gtk::TreeModelColumn<Glib::ustring> col_colour;
    };

    ModelColumns m_columns;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

private:

};

#endif	/* EVENTSWINDOW_H */

