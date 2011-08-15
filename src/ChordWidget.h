/*
    Copyright (C) 2010, 2011 Rafał Cieślak

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
#include "global.h"

class Chord;

class ChordWidget : public Gtk::Box {
public:
    ChordWidget();
    virtual ~ChordWidget();

    /**A chord this GUI is associated with, settings & changes done by user in this chordwidget will be applied to this chord*/
    Chord* chord;

    /**Refreshes the GUI using data from parent chord*/
    void Update();
    
    void ShowApplyOctave(bool show);
    void UpdateApplyOctave(bool apply);

    /**Updates the visibility/sensitivity (f.e. hides chors root selector, it the type is CUSTOM)*/
    void UpdateWhatToShowAndWhatIsSensitive();

    /**Called when any widget in chordwidget gets his data changed*/
    sigc::signal<void> on_changed;
    /**Calleed when a note is MANUALLY changed. Prototype: ...(int note_number,int pitch)*/
    sigc::signal<void,int,int> on_note_changed;
    /**Called when the Apply Octave toggle is toggled*/
    sigc::signal<void,bool> on_apply_octave_toggled;

    void Select(Chord* ch);
    void UnSelect();

    void SetExpandDetails(bool e);
    
    bool AnythingSelected;
    Gtk::Label caption;
private:
    void UpdateNotes();
    void UpdateSummary();
    void OnTypeChanged();
    void OnGuitarModeChanged();
    void OnTriadModeChanged();
    void OnRootChanged();
    void OnBaseNoteChanged();
    void OnBaseOctaveChanged();
    void OnInversionChanged();
    void OnBaseChanged();
    void OnUseBaseToggled();
    void OnNoteChanged(int n);
    void OnShow();
    void OnApplyOctaveClicked();
    
    /**Flag disabling reaction on signals, used to set data in widgets without reacting (react only if it was the user that changes the data)*/
    bool we_are_copying_note_values_from_chord_so_do_not_handle_the_signals;

    bool show_dnao;
    
    Gtk::HBox MainBox;

    Gtk::VBox LeftHBox;
    Gtk::VSeparator vsep;
    Gtk::VBox NotesVBox;

    Gtk::HBox line1;
    Gtk::HBox line2;
    Gtk::HBox line3;
    Gtk::HBox line4;
    Gtk::HBox line4_5;
    Gtk::HBox line5;

    Gtk::Label type_label;
    Gtk::Label inv_label;

    Gtk::ComboBox combo_type;
    Gtk::ComboBox combo_root;
    Gtk::ComboBox combo_guitar_mode;
    Gtk::ComboBox combo_triad_mode;
    Gtk::SpinButton inversion;

    Gtk::CheckButton use_base;
    Gtk::CheckButton do_not_apply_octave;

    Gtk::Label octave_label;
    Gtk::SpinButton octave;
    Gtk::Label note_label;
    Gtk::ComboBox combo_note;
    Gtk::Label eq_label;
    Gtk::SpinButton base;

    Gtk::Label summary_label;
    
    Gtk::SpinButton* note_buttons[6];
};

#endif	/* CHORDWIDGET_H */

