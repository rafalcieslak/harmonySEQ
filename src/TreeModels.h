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

#include <gtkmm.h>

class ModelColumns_SEQUENCERS : public Gtk::TreeModel::ColumnRecord {
public:

    ModelColumns_SEQUENCERS() {
        add(col_ID);
        add(col_name);
        add(col_muted);
        add(col_channel);
        add(col_pattern);
        add(col_res);
        add(col_len);
        add(col_vol);
        add(col_colour);
    }
    Gtk::TreeModelColumn<int> col_ID;
    Gtk::TreeModelColumn<Glib::ustring> col_name;
    Gtk::TreeModelColumn<bool> col_muted;
    Gtk::TreeModelColumn<int> col_channel;
    Gtk::TreeModelColumn<int> col_pattern;
    Gtk::TreeModelColumn<int> col_res;
    Gtk::TreeModelColumn<double> col_len;
    Gtk::TreeModelColumn<int> col_vol;
    Gtk::TreeModelColumn<Glib::ustring> col_colour;
};


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

class ModelColumns_Notes: public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_Notes() {
        add(note);
        add(name);
    }
    Gtk::TreeModelColumn<int> note;
    Gtk::TreeModelColumn<Glib::ustring> name;
};
class ModelColumns_Universal_IDandNAME: public Gtk::TreeModel::ColumnRecord {
public:
    ModelColumns_Universal_IDandNAME() {
        add(id);
        add(name);
    }
    Gtk::TreeModelColumn<int> id;
    Gtk::TreeModelColumn<Glib::ustring> name;
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
extern ModelColumns_SEQUENCERS m_columns_sequencers;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_sequencers;
extern ModelColumns_Notes m_columns_notes;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Notes;
extern ModelColumns_Universal_IDandNAME m_columns_IdAndName;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_TriadModes;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ChordGuitarModes;
extern Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ChordTypes;
#endif

/**Init - tree models*/
void InitEventTypesTreeModel();
void InitKeyTypesTreeModel();
void InitChannelsTreeModel();
void InitActionTypesTreeModel();
void InitNotesTreeModel();
void InitTriadModesTreeModel();
void InitChordGuitarModesTreeModel();
void InitChordTypesTreeModel();
void InitAllTreeModels();


#endif	/* TREEMODELS_H */

