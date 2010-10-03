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
    
    pack_start(line_guitar);
    pack_start(line_chord);
    pack_start(line_custom);


    Gtk::RadioButtonGroup group = radio_chord.get_group();
    radio_guitar.set_group(group);
    radio_custom.set_group(group);

    radio_chord.set_label(_("Chord"));
    radio_guitar.set_label(_("Guitar mode"));
    radio_custom.set_label(_("Custom"));
    line_guitar.pack_start(radio_guitar,Gtk::PACK_SHRINK);
    line_chord.pack_start(radio_chord,Gtk::PACK_SHRINK);
    line_custom.pack_start(radio_custom,Gtk::PACK_SHRINK);

    for (int x = 0; x < 6; x++){
        note_buttons[x] = new Gtk::SpinButton;
        note_buttons[x]->set_range(-60.0,60.0);
        note_buttons[x]->set_increments(1.0,12.0);
        note_buttons[x]->set_width_chars(2);
        note_buttons[x]->signal_value_changed().connect(sigc::bind<int>(mem_fun(*this,&ChordWidget::OnNoteChanged),x));
        line_custom.pack_end(*note_buttons[x],Gtk::PACK_SHRINK);
    }

    treeview_guitar_note.set_model(m_refTreeModel_Notes);
    treewiev_chord_note.set_model(m_refTreeModel_Notes);
    treeview_guitar_note.pack_start(m_columns_notes.name);
    treewiev_chord_note.pack_start(m_columns_notes.name);

    line_guitar.pack_start(treeview_guitar_note,Gtk::PACK_SHRINK);
    line_chord.pack_start(treewiev_chord_note,Gtk::PACK_SHRINK);
}


ChordWidget::~ChordWidget(){
    for (int x = 0; x < 6; x++)
        delete note_buttons[x];
}

void ChordWidget::OnNoteChanged(int n){

    radio_custom.set_active(1);
    
    chord->SetNote(n,note_buttons[n]->get_value());
    
}
