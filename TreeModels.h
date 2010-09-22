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

#ifndef TREEMODELS_H
#define	TREEMODELS_H
#include "global.h"

class ModelColumns_EventTypes : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_EventTypes() {
        add(type);
        add(label);
    }
    Gtk::TreeModelColumn<int> type;
    Gtk::TreeModelColumn<Glib::ustring> label;
};

class ModelColumns_ActionTypes : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_ActionTypes() {
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

class ModelColumns_Channels : public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_Channels() {
        add(ch);
        add(label);
    }
    Gtk::TreeModelColumn<int> ch;
    Gtk::TreeModelColumn<Glib::ustring> label;
};
#ifndef I_DO_NOT_WANT_EXTERNS_FROM_TREE_MODELS
extern ModelColumns_EventTypes m_columns_event_types;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_EventTypes;
extern ModelColumns_ActionTypes m_columns_action_types;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ActionTypes;
extern ModelColumns_KeyCodes m_columns_key_codes;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_KeyCodes;
extern ModelColumns_Channels m_columns_channels;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Channels;
#endif

void InitEventTypesTreeModel();
void InitKeyTypesTreeModel();
void InitChannelsTreeModel();
void InitActionTypesTreeModel();
void InitAllTreeModels();


#endif	/* TREEMODELS_H */
