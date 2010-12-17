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
#include "messages.h"

ModelColumns_SEQUENCERS m_columns_sequencers;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_sequencers;
ModelColumns_EventTypes m_columns_event_types;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_EventTypes;
ModelColumns_ActionTypes m_columns_action_types;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ActionTypes;
ModelColumns_KeyCodes m_columns_key_codes;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_KeyCodes;
ModelColumns_Channels m_columns_channels;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Channels;
ModelColumns_Notes m_columns_notes;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Notes;
ModelColumns_Universal_IDandNAME m_columns_IdAndName;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_TriadModes;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_ChordGuitarModes;

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

void InitActionTypesTreeModel(){

    m_refTreeModel_ActionTypes = Gtk::ListStore::create(m_columns_action_types);
    Gtk::TreeModel::Row row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::NONE;
    row[m_columns_action_types.label] = _("Empty");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_ON_OFF_TOGGLE;
    row[m_columns_action_types.label] = _("Sequencer/Switch On-Off");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_PLAY_ONCE;
    row[m_columns_action_types.label] = _("Sequencer/Play once");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_VOLUME_SET;
    row[m_columns_action_types.label] = _("Sequencer/Set volume");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_CHANGE_ONE_NOTE;
    row[m_columns_action_types.label] = _("Sequencer/Set one note");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_CHANGE_CHORD;
    row[m_columns_action_types.label] = _("Sequencer/Set chord");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_CHANGE_PATTERN;
    row[m_columns_action_types.label] = _("Sequencer/Set active pattern");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::MAINOTE_SET;
    row[m_columns_action_types.label] = _("Set main note");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::TEMPO_SET;
    row[m_columns_action_types.label] = _("Set tempo");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::PLAY_PAUSE;
    row[m_columns_action_types.label] = _("Play/pause");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SYNC;
    row[m_columns_action_types.label] = _("Synchronize");
    row = *(m_refTreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::TOGGLE_PASS_MIDI;
    row[m_columns_action_types.label] = _("Toggle passing MIDI events");

}

void InitKeyTypesTreeModel(){
    m_refTreeModel_KeyCodes = Gtk::ListStore::create(m_columns_key_codes);
    Gtk::TreeModel::Row row;
    std::map<int,Glib::ustring>::iterator iter = keymap_itos.begin();
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
        sprintf(temp,"%d",x);
        row[m_columns_channels.ch] = x;
        row[m_columns_channels.label] = temp;
    }
}

void InitNotesTreeModel(){
    //To consideration: are the note names untranslatable?
    m_refTreeModel_Notes = Gtk::ListStore::create(m_columns_notes);
    Gtk::TreeModel::Row row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 0;
    row[m_columns_notes.name] = "C";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 1;
    row[m_columns_notes.name] = "C#";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 2;
    row[m_columns_notes.name] = "D";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 3;
    row[m_columns_notes.name] = "D#";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 4;
    row[m_columns_notes.name] = "E";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 5;
    row[m_columns_notes.name] = "F";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 6;
    row[m_columns_notes.name] = "F#";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 7;
    row[m_columns_notes.name] = "G";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 8;
    row[m_columns_notes.name] = "G#";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 9;
    row[m_columns_notes.name] = "A";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 10;
    row[m_columns_notes.name] = "A#";
    row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 11;
    row[m_columns_notes.name] = "H";
   /* Gtk::TreeModel::Row row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 12;
    row[m_columns_notes.name] = "C";*/
    
}
void InitTriadModesTreeModel(){
    m_refTreeModel_TriadModes = Gtk::ListStore::create(m_columns_IdAndName);
    Gtk::TreeModel::Row row = *(m_refTreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = 0;
    row[m_columns_IdAndName.name] = _("Major");
    row = *(m_refTreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = 1;
    row[m_columns_IdAndName.name] = _("Minor");
    row = *(m_refTreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = 2;
    row[m_columns_IdAndName.name] = _("Augmented");
    row = *(m_refTreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = 3;
    row[m_columns_IdAndName.name] = _("Diminished");

}

void InitChordGuitarModesTreeModel(){
    m_refTreeModel_ChordGuitarModes = Gtk::ListStore::create(m_columns_IdAndName);
    Gtk::TreeModel::Row row = *(m_refTreeModel_ChordGuitarModes->append());
    row[m_columns_IdAndName.id] = 0;
    row[m_columns_IdAndName.name] = _("Major");
    row = *(m_refTreeModel_ChordGuitarModes->append());
    row[m_columns_IdAndName.id] = 1;
    row[m_columns_IdAndName.name] = _("Minor");
    
}

void InitAllTreeModels(){
    InitEventTypesTreeModel();
    InitActionTypesTreeModel();
    InitKeyTypesTreeModel();
    InitChannelsTreeModel();
    InitTriadModesTreeModel();
    InitChordGuitarModesTreeModel();
    InitNotesTreeModel();
}

