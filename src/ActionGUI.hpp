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

#ifndef ACTIONGUI_H
#define	ACTIONGUI_H
#include "global.hpp"
#include <gtkmm.h>
#include "ChordWidget.hpp"
class Action;

class ActionGUI : public Gtk::Window {
public:
    ActionGUI();
    virtual ~ActionGUI();

    void SwitchTarget(Action* t);

private:
    /**Flag disabling reaction on signals, used to set data in widgets without reacting (react only if it was the user that changes the data)*/
    bool we_are_copying_data_from_parent_action_so_do_not_handle_signals;

    /** An action this GUI currently displays. Note: It can be NULL at times. */
    Action *target;

    /** Refreshes the GUI according to parent Action */
    void UpdateEverything();

    void OnShow();
    void OnHide();
    bool shown;
    void SetupTreeModels();
    /**Hides and shows lines appropieate to the parent action type.*/
    void UpdateVisibleLines();

    /**Sets all default data for given type, used to avoid having actions with strange arguments*/
    void InitType(int action_type);

    void OnOKClicked();
    void OnTypeChanged();
    void OnAllSeqComboChanged();
    void OnNoteSeqComboChanged();
    void OnTempoChanged();
    void OnVelocityChanged();
    void SetTypeCombo(int type);
    void SetSeqCombos(std::shared_ptr<Sequencer>);
    void OnChordChanged();
    void OnNoteNrChanged();
    void OnNoteSeqChanged();
    void OnOnOffToggleChanged();
    void OnPlayOnOffToggleClicked();
    void OnPatternChanged();
    void OnOctaveChanged();
    void OnApplyOctaveToogled(bool apply);

    void UpdateSequencerList();

    Gtk::VBox main_box;

    Gtk::HBox line_type;
    Gtk::Label label_type;
    Gtk::HBox line_seq;
    Gtk::Label label_seq;
    Gtk::HBox line_tempo;
    Gtk::Label label_tempo;
    Gtk::HBox line_set_one_note;
    Gtk::Label label_note_nr;
    Gtk::Label label_note_seq;
    Gtk::HBox line_chord;
    Gtk::HBox line_pattern;
    Gtk::Label label_pattern;
    Gtk::HBox line_octave;
    Gtk::Label label_octave1;
    Gtk::Label label_octave2;


    Gtk::HBox line_on_off_toggle;
    Gtk::RadioButton on_off_toggle_ON;
    Gtk::RadioButton on_off_toggle_OFF;
    Gtk::RadioButton on_off_toggle_TOGGLE;

    Gtk::HBox line_play;
    Gtk::RadioButton play_OFF;
    Gtk::RadioButton play_ON;
    Gtk::RadioButton play_TOGGLE;

    Gtk::ComboBox Types_combo;
    Gtk::ComboBox AllSeqs_combo;
    Gtk::ComboBox NoteSeqs_combo;
    Gtk::SpinButton tempo_button;
    Gtk::SpinButton notenr_button;
    Gtk::SpinButton chordseq_button;
    Gtk::SpinButton pattern_button;
    Gtk::SpinButton octave_spinbutton;

    Gtk::Button ok_button;

    Gtk::Label label_preview;
    Gtk::HSeparator separator;

    Chord chord;
    ChordWidget chordwidget;

    /* Helper method for setting target->target_seq according to selection in ComboBox. */
    void SetTargetSeq();

    class ModelColumns_Seqs : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns_Seqs() {
            add(seq); add(name);
        }
        Gtk::TreeModelColumn<std::weak_ptr<Sequencer>> seq;
        Gtk::TreeModelColumn<Glib::ustring> name;
    };
    ModelColumns_Seqs m_col_seqs;

    Glib::RefPtr<Gtk::ListStore> m_ref_treemodel_allseqs;
    Glib::RefPtr<Gtk::ListStore> m_ref_treemodel_noteseqs;
};

#endif	/* ACTIONGUI_H */
