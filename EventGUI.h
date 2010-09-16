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

#ifndef EVENTGUI_H
#define	EVENTGUI_H
#include "global.h"

class Event;

class EventGUI :public Gtk::Window{
public:
    EventGUI(Event *prt);
    virtual ~EventGUI();

    void TypeChanged();
    Event *parent;

    Gtk::VBox main_box;
    Gtk::HBox line_one;
    Gtk::Label label_one;
    Gtk::HBox line_two;
    Gtk::Label label_two;
    Gtk::HBox line_three;
    Gtk::Label label_three;
    Gtk::ComboBox Types_combo;


    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(type); add(label);
        }
        Gtk::TreeModelColumn<int> type;
        Gtk::TreeModelColumn<Glib::ustring> label;
    };
    ModelColumns m_columns_types;

    Glib::RefPtr<Gtk::ListStore> m_refTreeModel_types;

private:

};

#endif	/* EVENTGUI_H */

