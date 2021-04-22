/*
    Copyright (C) 2010, 2020 Rafał Cieślak

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

#include "TreeModels.hpp"

#include <map>
#include <string>

#include "Action.hpp"
#include "Event.hpp"
#include "shared.hpp"


ModelColumns_SEQUENCERS m_columns_sequencers;
Glib::RefPtr<Gtk::ListStore> TreeModel_sequencers;
ModelColumns_EventTypes m_columns_event_types;
Glib::RefPtr<Gtk::ListStore> TreeModel_EventTypes;
ModelColumns_ActionTypes m_columns_action_types;
Glib::RefPtr<Gtk::ListStore> TreeModel_ActionTypes;
ModelColumns_KeyCodes m_columns_key_codes;
Glib::RefPtr<Gtk::ListStore> TreeModel_KeyCodes;
ModelColumns_Channels m_columns_channels;
Glib::RefPtr<Gtk::ListStore> TreeModel_Channels;
ModelColumns_Notes m_columns_notes;
Glib::RefPtr<Gtk::ListStore> TreeModel_Notes;
ModelColumns_Universal_IDandNAME m_columns_IdAndName;
Glib::RefPtr<Gtk::ListStore> TreeModel_TriadModes;
Glib::RefPtr<Gtk::ListStore> TreeModel_ChordGuitarModes;
Glib::RefPtr<Gtk::ListStore> TreeModel_ChordTypes;

void InitEventTypesTreeModel(){

    TreeModel_EventTypes = Gtk::ListStore::create(m_columns_event_types);
    Gtk::TreeModel::Row row = *(TreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::NONE;
    row[m_columns_event_types.label] = _("Empty");
    row = *(TreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::KEYBOARD;
    row[m_columns_event_types.label] = _("Keyboard");
    row = *(TreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::NOTE;
    row[m_columns_event_types.label] = _("MIDI Note");
    row = *(TreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::CONTROLLER;
    row[m_columns_event_types.label] = _("MIDI Controller");
    row = *(TreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::OSC;
    row[m_columns_event_types.label] = _("OSC");
}

void InitActionTypesTreeModel(){

    TreeModel_ActionTypes = Gtk::ListStore::create(m_columns_action_types);
    Gtk::TreeModel::Row row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::NONE;
    row[m_columns_action_types.label] = _("Empty");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_ON_OFF_TOGGLE;
    row[m_columns_action_types.label] = _("Sequencer/Switch On-Off");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_PLAY_ONCE;
    row[m_columns_action_types.label] = _("Sequencer/Play once");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_CHANGE_ONE_NOTE;
    row[m_columns_action_types.label] = _("Sequencer/Set one note");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_CHANGE_CHORD;
    row[m_columns_action_types.label] = _("Sequencer/Set chord");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_TRANSPOSE_OCTAVE;
    row[m_columns_action_types.label] = _("Sequencer/Change chord's octave");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SEQ_CHANGE_PATTERN;
    row[m_columns_action_types.label] = _("Sequencer/Set active pattern");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::TEMPO_SET;
    row[m_columns_action_types.label] = _("Set tempo");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::PLAY_PAUSE;
    row[m_columns_action_types.label] = _("Play/pause");
    row = *(TreeModel_ActionTypes->append());
    row[m_columns_action_types.type] = Action::SYNC;
    row[m_columns_action_types.label] = _("Synchronize");

}

void InitKeyTypesTreeModel(){
    TreeModel_KeyCodes = Gtk::ListStore::create(m_columns_key_codes);
    Gtk::TreeModel::Row row;
    std::map<int,std::string>::iterator iter = keymap_itos.begin();
    for (;iter != keymap_itos.end();iter++){
        row = *(TreeModel_KeyCodes->append());
        row[m_columns_key_codes.keycode] = iter->first;
        row[m_columns_key_codes.label] = iter->second;
    }

}

void InitChannelsTreeModel(){
    TreeModel_Channels = Gtk::ListStore::create(m_columns_channels);
    Gtk::TreeModel::Row row = *(TreeModel_Channels->append());
    row[m_columns_channels.ch] = 0;
    row[m_columns_channels.label] = _("All");
    char temp[3];
    for (int x = 1; x < 17; x++){
        row = *(TreeModel_Channels->append());
        sprintf(temp,"%d",x);
        row[m_columns_channels.ch] = x;
        row[m_columns_channels.label] = temp;
    }
}

void InitNotesTreeModel(){
    //To consideration: are the note names untranslatable?
    TreeModel_Notes = Gtk::ListStore::create(m_columns_notes);
    Gtk::TreeModel::Row row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 0;
    row[m_columns_notes.name] = "C";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 1;
    row[m_columns_notes.name] = "C#";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 2;
    row[m_columns_notes.name] = "D";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 3;
    row[m_columns_notes.name] = "D#";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 4;
    row[m_columns_notes.name] = "E";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 5;
    row[m_columns_notes.name] = "F";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 6;
    row[m_columns_notes.name] = "F#";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 7;
    row[m_columns_notes.name] = "G";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 8;
    row[m_columns_notes.name] = "G#";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 9;
    row[m_columns_notes.name] = "A";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 10;
    row[m_columns_notes.name] = "A#";
    row = *(TreeModel_Notes->append());
    row[m_columns_notes.note] = 11;
    row[m_columns_notes.name] = "H";
   /* Gtk::TreeModel::Row row = *(m_refTreeModel_Notes->append());
    row[m_columns_notes.note] = 12;
    row[m_columns_notes.name] = "C";*/

}
void InitTriadModesTreeModel(){
    TreeModel_TriadModes = Gtk::ListStore::create(m_columns_IdAndName);
    Gtk::TreeModel::Row row = *(TreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TRIAD_MODE_MAJOR;
    row[m_columns_IdAndName.name] = _("Major");
    row = *(TreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TRIAD_MODE_MINOR;
    row[m_columns_IdAndName.name] = _("Minor");
    row = *(TreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TRIAD_MODE_AUGMENTED;
    row[m_columns_IdAndName.name] = _("Augmented");
    row = *(TreeModel_TriadModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TRIAD_MODE_DIMINISCHED;
    row[m_columns_IdAndName.name] = _("Diminished");

}

void InitChordGuitarModesTreeModel(){
    TreeModel_ChordGuitarModes = Gtk::ListStore::create(m_columns_IdAndName);
    Gtk::TreeModel::Row row = *(TreeModel_ChordGuitarModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_GUITAR_MODE_MAJOR;
    row[m_columns_IdAndName.name] = _("maj");
    row = *(TreeModel_ChordGuitarModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_GUITAR_MODE_MINOR;
    row[m_columns_IdAndName.name] = _("m");
    row = *(TreeModel_ChordGuitarModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_GUITAR_MODE_MAJ7;
    row[m_columns_IdAndName.name] = _("maj7");
    row = *(TreeModel_ChordGuitarModes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_GUITAR_MODE_M7;
    row[m_columns_IdAndName.name] = _("m7");

}

void InitChordTypesTreeModel(){
    TreeModel_ChordTypes = Gtk::ListStore::create(m_columns_IdAndName);
    Gtk::TreeModel::Row row = *(TreeModel_ChordTypes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TYPE_CUSTOM;
    row[m_columns_IdAndName.name] = _("Custom");
    row = *(TreeModel_ChordTypes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TYPE_TRIAD;
    row[m_columns_IdAndName.name] = _("Triad");
    row = *(TreeModel_ChordTypes->append());
    row[m_columns_IdAndName.id] = Chord::CHORD_TYPE_GUITAR;
    row[m_columns_IdAndName.name] = _("Guitar");


}

void InitAllTreeModels(){
    InitEventTypesTreeModel();
    InitActionTypesTreeModel();
    InitKeyTypesTreeModel();
    InitChannelsTreeModel();
    InitTriadModesTreeModel();
    InitChordGuitarModesTreeModel();
    InitChordTypesTreeModel();
    InitNotesTreeModel();
}
