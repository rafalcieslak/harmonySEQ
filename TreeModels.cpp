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

#define I_DO_NOT_WANT_EXTERNS_FROM_TREE_MODELS
#include "TreeModels.h"
#undef I_DO_NOT_WANT_EXTERNS_FROM_TREE_MODELS
#include "Event.h"

ModelColumns_EventTypes m_columns_event_types;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_EventTypes;
ModelColumns_KeyCodes m_columns_key_codes;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_KeyCodes;
ModelColumns_Channels m_columns_channels;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Channels;

void InitEventTypesTreeModel(){

    m_refTreeModel_EventTypes = Gtk::ListStore::create(m_columns_event_types);
    Gtk::TreeModel::Row row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::NONE;
    row[m_columns_event_types.label] = _("Empty");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::KEYBOARD;
    row[m_columns_event_types.label] = _("Keyboard");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::NOTE;
    row[m_columns_event_types.label] = _("Note");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::CONTROLLER;
    row[m_columns_event_types.label] = _("Controller");
}

void InitKeyTypesTreeModel(){
    m_refTreeModel_KeyCodes = Gtk::ListStore::create(m_columns_key_codes);
    Gtk::TreeModel::Row row;
    std::map<int,string>::iterator iter = keymap_itos.begin();
    for (;iter != keymap_itos.end();iter++){
        row = *(m_refTreeModel_KeyCodes->append());
        row[m_columns_key_codes.keycode] = iter->first;
        row[m_columns_key_codes.label] = iter->second;
    }

}

void InitChannelsTreeModel(){
    m_refTreeModel_Channels = Gtk::ListStore::create(m_columns_channels);
    Gtk::TreeModel::Row row = *(m_refTreeModel_Channels->append());
    row[m_columns_channels.ch] = 0;
    row[m_columns_channels.label] = _("All");
    char temp[3];
    for (int x = 1; x < 17; x++){
        row = *(m_refTreeModel_Channels->append());
        *dbg << "-----" << x << ENDL;
        sprintf(temp,"%d",x);
        row[m_columns_channels.ch] = x;
        row[m_columns_channels.label] = temp;
    }
}

void InitAllTreeModels(){
    InitEventTypesTreeModel();
    InitKeyTypesTreeModel();
    InitChannelsTreeModel();
}

