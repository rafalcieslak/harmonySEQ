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

class ChordWidget : public Gtk::Box {
public:
    ChordWidget(Chord* associated_chord);
    virtual ~ChordWidget();

    Chord* chord;
    
    Gtk::VBox MainBox;

    Gtk::HBox line_guitar;
    Gtk::HBox line_triad;
    Gtk::HBox line_custom;
    Gtk::HBox line_custom2;

    Gtk::Label octave_label;


    Gtk::RadioButton radio_guitar;
    Gtk::RadioButton radio_triad;
    Gtk::RadioButton radio_custom;
    Gtk::SpinButton* note_buttons[6];

    Gtk::ComboBox combo_guitar_note;
    Gtk::ComboBox combo_triad_note;
    Gtk::ComboBox combo_guitar_mode;
    Gtk::ComboBox combo_triad_mode;
    Gtk::SpinButton octave;

    Gtk::Frame frame;
    
    void UpdateNotes();
private:
    void OnNoteChanged(int n);
    void OnRadioCustomToggled();
    void OnRadioGuitarToggled();
    void OnRadioTriadToggled();
    void OnGuitarRootChanged();
    void OnTriadRootChanged();
    void OnTriadModeChanged();
    void OnGuitarModeChanged();
    void OnOctaveChanged();
    bool we_are_copying_note_values_from_chord_so_do_not_handle_the_signals;
};

#endif	/* CHORDWIDGET_H */

