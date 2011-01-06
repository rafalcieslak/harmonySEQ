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

    /**A chord this GUI is associated with, settings & changes done by user in this chordwidget will be applied to this chord*/
    Chord* chord;

    /**Refreshes the GUI using data from parent chord*/
    void Update();

    /**Called when any widget in chordwidget gets his data changed*/
    sigc::signal<void> on_changed;
private:
    void UpdateNotes();
    void UpdateWhatToShowAndWhatIsSensitive();
    void OnTypeChanged();
    void OnGuitarModeChanged();
    void OnTriadModeChanged();
    void OnRootChanged();
    void OnBaseNoteChanged();
    void OnBaseOctaveChanged();
    void OnInversionChanged();
    void OnBaseChanged();
    void OnNoteChanged(int n);

    /**Flag disabling reaction on signals, used to set data in widgets without reacting (react only if it was the user that changes the data)*/
    bool we_are_copying_note_values_from_chord_so_do_not_handle_the_signals;

    Gtk::VBox MainBox;

    Gtk::HBox line1;
    Gtk::HBox line2;
    Gtk::HBox line3;

    Gtk::ComboBox combo_type;
    Gtk::ComboBox combo_root;
    Gtk::ComboBox combo_guitar_mode;
    Gtk::ComboBox combo_triad_mode;
    Gtk::Label inversion_label;
    Gtk::SpinButton inversion;

    Gtk::CheckButton use_base;

    Gtk::Label octave_label;
    Gtk::SpinButton octave;
    Gtk::Label note_label;
    Gtk::ComboBox combo_note;
    Gtk::Label eq_label;
    Gtk::SpinButton base;

    Gtk::SpinButton* note_buttons[6];

    Gtk::Frame frame;
};

#endif	/* CHORDWIDGET_H */

