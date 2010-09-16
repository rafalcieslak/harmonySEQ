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

class ModelColumns_EventTypes : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_EventTypes() {
        add(type);
        add(label);
    }
    Gtk::TreeModelColumn<int> type;
    Gtk::TreeModelColumn<Glib::ustring> label;
};

class ModelColumns_KeyCodes : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_KeyCodes() {
        add(keycode);
        add(label);
    }
    Gtk::TreeModelColumn<int> keycode;
    Gtk::TreeModelColumn<Glib::ustring> label;
};
#ifndef I_DO_NOT_WANT_EXTERNS_FROM_EVENT_GUI
extern ModelColumns_EventTypes m_columns_event_types;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_EventTypes;
extern ModelColumns_KeyCodes m_columns_key_codes;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_KeyCodes;
#endif

void InitEventTypesTreeModel();
void InitKeyTypesTreeModel();
void InitAllTreeModels();

class EventGUI :public Gtk::Window{
public:
    EventGUI(Event *prt);
    virtual ~EventGUI();

    void TypeChanged();
    Event *parent;

    Gtk::VBox main_box;
    Gtk::HBox line_type;
    Gtk::Label label_type;
    Gtk::HBox line_key;
    Gtk::Label label_key;
    Gtk::HBox line_note;
    Gtk::Label label_note;
    Gtk::HBox line_channel;
    Gtk::Label label_channel;
    Gtk::HBox line_controller;
    Gtk::Label label_controller;

    
    Gtk::ComboBox Types_combo;
    Gtk::ComboBox Keys_combo;





private:

};

#endif	/* EVENTGUI_H */

