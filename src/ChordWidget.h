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

#ifndef CHORDWIDGET_H
#define	CHORDWIDGET_H
#include <gtkmm.h>

#include "Chord.h"
#include "global.h"

class ChordWidget : public Gtk::VBox {
public:
    ChordWidget(Chord* associated_chord);
    virtual ~ChordWidget();

    Chord* chord;
    
    Gtk::VBox MainBox;

    Gtk::HBox line_guitar;
    Gtk::HBox line_chord;
    Gtk::HBox line_custom;

    Gtk::Label octave_label;


    Gtk::RadioButton radio_guitar;
    Gtk::RadioButton radio_chord;
    Gtk::RadioButton radio_custom;
    Gtk::SpinButton* note_buttons[6];

    Gtk::ComboBox treeview_guitar_note;
    Gtk::ComboBox treewiev_chord_note;
    Gtk::SpinButton octave;
    
    void OnNoteChanged(int n);
private:

};

#endif	/* CHORDWIDGET_H */

