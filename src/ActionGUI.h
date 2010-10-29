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

#ifndef ACTIONGUI_H
#define	ACTIONGUI_H
#include "global.h"
#include <gtkmm.h>
#include "ChordWidget.h"
class Action;

class ActionGUI : public Gtk::Window{
public:
    ActionGUI(Action *prt);
    virtual ~ActionGUI();
    
    /**Called mostly from outside this class, it refreshes the GUI according to parent Action*/
    void UpdateValues();

    void UpdateChordwidget();

    /**An action this GUI is associated with, it is used for two-way communication between Action and ActionGUI*/
    Action *parent;

private:
    /**Flag disabling reaction on signals, used to set data in widgets without reacting (react only if it was the user that changes the data)*/
    bool we_are_copying_data_from_parent_action_so_do_not_handle_signals;

    void OnShow();

    /**Hides and shows lines appropieate to the parent action type.*/
    void ChangeVisibleLines();

    /**Sets all default data for given type, used to avoid having actions with strange arguments*/
    void InitType();

    void OnOKClicked();
    void OnTypeChanged();
    void OnSeqChanged();
    void OnTempoChanged();
    void OnNoteChanged();
    void OnVolumeChanged();
    void SetTypeCombo(int type);
    void SetSeqCombo(int seq);
    void OnNoteNrChanged();
    void OnNoteSeqChanged();
    void OnOnOffToggleChanged();
    void OnPlayOnOffToggleClicked();
    void OnChordWidgetChanged();


    Gtk::VBox main_box;

    Gtk::HBox line_type;
    Gtk::Label label_type;
    Gtk::HBox line_note;
    Gtk::Label label_note;
    Gtk::HBox line_seq;
    Gtk::Label label_seq;
    Gtk::HBox line_tempo;
    Gtk::Label label_tempo;
    Gtk::HBox line_volume;
    Gtk::Label label_volume;
    Gtk::HBox line_set_one_note;
    Gtk::Label label_note_nr;
    Gtk::Label label_note_seq;
    Gtk::HBox line_chord;

    Gtk::HBox line_on_off_toggle;
    Gtk::RadioButton on_off_toggle_ON;
    Gtk::RadioButton on_off_toggle_OFF;
    Gtk::RadioButton on_off_toggle_TOGGLE;

    Gtk::HBox line_play;
    Gtk::RadioButton play_OFF;
    Gtk::RadioButton play_ON;
    Gtk::RadioButton play_TOGGLE;

    Gtk::ComboBox Types_combo;
    Gtk::ComboBox Seqs_combo;
    Gtk::SpinButton note_button;
    Gtk::SpinButton tempo_button;
    Gtk::SpinButton vol_button;
    Gtk::SpinButton notenr_button;
    Gtk::SpinButton chordseq_button;

    Gtk::Button ok_button;

    Gtk::Label label_preview;
    Gtk::HSeparator separator;

    ChordWidget chordwidget;

};

#endif	/* ACTIONGUI_H */

