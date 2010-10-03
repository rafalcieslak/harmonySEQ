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

#include "ChordWidget.h"
#include "TreeModels.h"


ChordWidget::ChordWidget(Chord* associated_chord){

    chord = associated_chord;
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;
    pack_start(frame);
    
    frame.set_label(_("Chord"));
    frame.add(MainBox);

    MainBox.pack_start(line_guitar);
    MainBox.pack_start(line_triad);
    MainBox.pack_start(line_custom);
    MainBox.pack_start(line_custom2);


    Gtk::RadioButtonGroup group = radio_triad.get_group();
    radio_guitar.set_group(group);
    radio_custom.set_group(group);

    radio_triad.set_label(_("Triad"));
    radio_guitar.set_label(_("Guitar mode"));
    radio_custom.set_label(_("Custom"));
    line_guitar.pack_start(radio_guitar,Gtk::PACK_SHRINK);
    line_triad.pack_start(radio_triad,Gtk::PACK_SHRINK);
    line_custom.pack_start(radio_custom,Gtk::PACK_SHRINK);
    if(chord->GetMode() == Chord::CUSTOM){radio_custom.set_active(1);}
    if(chord->GetMode() == Chord::GUITAR){radio_guitar.set_active(1);}
    if(chord->GetMode() == Chord::TRIAD){radio_triad.set_active(1);}
    radio_triad.signal_toggled().connect(mem_fun(*this,&ChordWidget::OnRadioTriadToggled));
    radio_guitar.signal_toggled().connect(mem_fun(*this,&ChordWidget::OnRadioGuitarToggled));
    radio_custom.signal_toggled().connect(mem_fun(*this,&ChordWidget::OnRadioCustomToggled));

    for (int x = 0; x < 6; x++){
        note_buttons[x] = new Gtk::SpinButton;
        note_buttons[x]->set_range(-60.0,60.0);
        note_buttons[x]->set_increments(1.0,12.0);
        note_buttons[x]->set_width_chars(3);
        note_buttons[x]->signal_value_changed().connect(sigc::bind<int>(mem_fun(*this,&ChordWidget::OnNoteChanged),x));
        line_custom2.pack_start(*note_buttons[x],Gtk::PACK_SHRINK);
    }

    combo_guitar_note.set_model(m_refTreeModel_Notes);
    combo_triad_note.set_model(m_refTreeModel_Notes);
    combo_guitar_note.pack_start(m_columns_notes.name);
    combo_triad_note.pack_start(m_columns_notes.name);
    combo_guitar_note.signal_changed().connect(mem_fun(*this,&ChordWidget::OnGuitarRootChanged));
    combo_triad_note.signal_changed().connect(mem_fun(*this,&ChordWidget::OnTriadRootChanged));
    combo_triad_note.set_active(chord->GetTriadRoot()); //tricky
    combo_guitar_note.set_active(chord->GetGuitarRoot());
    line_guitar.pack_start(combo_guitar_note,Gtk::PACK_SHRINK);
    line_triad.pack_start(combo_triad_note,Gtk::PACK_SHRINK);

    UpdateNotes();
}


ChordWidget::~ChordWidget(){
    for (int x = 0; x < 6; x++)
        delete note_buttons[x];
}

void ChordWidget::OnNoteChanged(int n){
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;

    radio_custom.set_active(1);
    
    chord->SetNote(n,note_buttons[n]->get_value());

    //combo_triad_note.set_active(-1);
    //combo_guitar_note.set_active(-1);
    
}

void ChordWidget::OnRadioTriadToggled(){
    if(radio_triad.get_active()){
        chord->SetMode(Chord::TRIAD);
        UpdateNotes();
    }
}

void ChordWidget::OnRadioCustomToggled(){
    if(radio_custom.get_active()){
        chord->SetMode(Chord::CUSTOM);
        UpdateNotes();
    }
}

void ChordWidget::OnRadioGuitarToggled(){
    if(radio_guitar.get_active()){
        chord->SetMode(Chord::GUITAR);
        UpdateNotes();
    }
}

void ChordWidget::UpdateNotes(){
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    for (int x =0;x < 6; x++){
        note_buttons[x]->set_value(chord->GetNote(x));
    }
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;

}

void ChordWidget::OnGuitarRootChanged(){
    Gtk::TreeModel::Row row = *(combo_guitar_note.get_active());
    chord->SetGuitarRoot(row[m_columns_notes.note]);
    UpdateNotes();
}

void ChordWidget::OnTriadRootChanged(){
    Gtk::TreeModel::Row row = *(combo_triad_note.get_active());
    chord->SetTriadRoot(row[m_columns_notes.note]);
    UpdateNotes();
}