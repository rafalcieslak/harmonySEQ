/*
    Copyright (C) 2010-2012, 2020 Rafał Cieślak

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

#include "ActionGUI.hpp"

#include "Action.hpp"
#include "Files.hpp"
#include "NoteSequencer.hpp"
#include "Sequencer.hpp"
#include "SequencerManager.hpp"
#include "TreeModels.hpp"
#include "messages.hpp"
#include "shared.hpp"


ActionGUI::ActionGUI()
{
    set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    set_modal(true);

    //by defalut default
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = false;

    //window's title
    set_title(_("Action"));

    set_border_width(5);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    set_resizable(0);

    /* Holy shit, are we really building the entire interface BY
     * HAND?! This needs to be rewritten with Glade ASAP. */
    add(main_box);
    main_box.set_spacing(5);

    main_box.pack_start(line_type);
    main_box.pack_start(line_seq);
    main_box.pack_start(line_on_off_toggle);
    main_box.pack_start(line_tempo);
    main_box.pack_start(line_pattern);
    main_box.pack_start(line_set_one_note);
    main_box.pack_start(line_chord);
    main_box.pack_start(line_octave);
    main_box.pack_start(line_play);
    main_box.pack_start(separator);//==========
    main_box.pack_start(label_preview);
    main_box.pack_start(ok_button);

    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_seq.pack_start(label_seq,Gtk::PACK_SHRINK);
    line_tempo.pack_start(label_tempo,Gtk::PACK_SHRINK);
    line_pattern.pack_start(label_pattern,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(label_note_nr,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(notenr_button,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(label_note_seq,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(chordseq_button,Gtk::PACK_SHRINK);

    line_on_off_toggle.pack_start(on_off_toggle_TOGGLE,Gtk::PACK_SHRINK);
    line_on_off_toggle.pack_start(on_off_toggle_ON,Gtk::PACK_SHRINK);
    line_on_off_toggle.pack_start(on_off_toggle_OFF,Gtk::PACK_SHRINK);
    Gtk::RadioButtonGroup group = on_off_toggle_OFF.get_group();
    on_off_toggle_ON.set_group(group);
    on_off_toggle_TOGGLE.set_group(group);
    on_off_toggle_OFF.signal_clicked().connect(std::bind(&ActionGUI::OnOnOffToggleChanged, this));
    on_off_toggle_ON.signal_clicked().connect(std::bind(&ActionGUI::OnOnOffToggleChanged, this));
    on_off_toggle_TOGGLE.signal_clicked().connect(std::bind(&ActionGUI::OnOnOffToggleChanged, this));

    line_play.pack_start(play_TOGGLE,Gtk::PACK_SHRINK);
    line_play.pack_start(play_ON,Gtk::PACK_SHRINK);
    line_play.pack_start(play_OFF,Gtk::PACK_SHRINK);
    group = play_OFF.get_group();
    play_ON.set_group(group);
    play_TOGGLE.set_group(group);
    play_OFF.signal_clicked().connect(std::bind(&ActionGUI::OnPlayOnOffToggleClicked, this));
    play_ON.signal_clicked().connect(std::bind(&ActionGUI::OnPlayOnOffToggleClicked, this));
    play_TOGGLE.signal_clicked().connect(std::bind(&ActionGUI::OnPlayOnOffToggleClicked, this));

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_seq.pack_start(AllSeqs_combo,Gtk::PACK_SHRINK);
    line_seq.pack_start(NoteSeqs_combo,Gtk::PACK_SHRINK);
    line_pattern.pack_start(pattern_button,Gtk::PACK_SHRINK);
    line_tempo.pack_start(tempo_button,Gtk::PACK_SHRINK);
    line_chord.pack_start(chordwidget);

    line_octave.pack_start(label_octave1,Gtk::PACK_SHRINK);
    line_octave.pack_start(octave_spinbutton,Gtk::PACK_SHRINK);
    line_octave.pack_start(label_octave2,Gtk::PACK_SHRINK);

    notenr_button.set_range(1.0,6.0);
    notenr_button.set_increments(1.0,2.0);
    notenr_button.signal_value_changed().connect(std::bind(&ActionGUI::OnNoteNrChanged, this));
    chordseq_button.set_range(-48.0,48.0);
    chordseq_button.set_increments(1.0,12.0);
    chordseq_button.signal_value_changed().connect(std::bind(&ActionGUI::OnNoteSeqChanged, this));
    pattern_button.set_range(0.0,100.0);
    pattern_button.set_increments(1.0,10.0);
    pattern_button.signal_value_changed().connect(std::bind(&ActionGUI::OnPatternChanged, this));
    tempo_button.set_range(30.0,320.0);
    tempo_button.set_increments(1.0,20.0);
    tempo_button.signal_value_changed().connect(std::bind(&ActionGUI::OnTempoChanged, this));
    octave_spinbutton.set_range(-5.0,5.0);
    octave_spinbutton.set_increments(1.0,2.0);
    octave_spinbutton.signal_value_changed().connect(std::bind(&ActionGUI::OnOctaveChanged, this));


    label_type.set_text(_("Type:"));
    label_seq.set_text(_("Sequencer:"));
    label_tempo.set_text(_("Tempo:"));
    label_note_nr.set_text(_("Set note "));
    label_note_seq.set_text(_(" to: "));
    label_pattern.set_text(_("Pattern number: "));
    on_off_toggle_OFF.set_label(_("Off"));
    on_off_toggle_ON.set_label(_("On"));
    on_off_toggle_TOGGLE.set_label(_("Toggle"));
    play_OFF.set_label(_("Pause"));
    play_ON.set_label(_("Play"));
    play_TOGGLE.set_label(_("Toggle"));
    ok_button.set_label(_("OK"));
    label_octave1.set_text(_("Transpose by "));
    label_octave2.set_text(_(" octave(s)."));

    ok_button.signal_clicked().connect(std::bind(&ActionGUI::OnOKClicked, this));

    Types_combo.set_model(TreeModel_ActionTypes);
    Types_combo.pack_start(m_columns_action_types.label);
    Types_combo.signal_changed().connect(std::bind(&ActionGUI::OnTypeChanged, this));

    m_ref_treemodel_allseqs = Gtk::ListStore::create(m_col_seqs);
    m_ref_treemodel_noteseqs = Gtk::ListStore::create(m_col_seqs);

    AllSeqs_combo.set_model(m_ref_treemodel_allseqs);
    AllSeqs_combo.pack_start(m_col_seqs.name);
    AllSeqs_combo.signal_changed().connect(std::bind(&ActionGUI::OnAllSeqComboChanged, this));
    NoteSeqs_combo.set_model(m_ref_treemodel_noteseqs);
    NoteSeqs_combo.pack_start(m_col_seqs.name);
    NoteSeqs_combo.signal_changed().connect(std::bind(&ActionGUI::OnNoteSeqComboChanged, this));

    signal_show().connect(std::bind(&ActionGUI::OnShow, this));
    signal_hide().connect(std::bind(&ActionGUI::OnHide, this));

    //Show all children widgets
    show_all_children(1);


    chordwidget.Select(&chord);
    chordwidget.SetExpandDetails(1);
    chordwidget.ShowApplyOctave(1);
    // TODO: We can get rid of this signal once Actions are polymorphic and we can listen to action's chord events directly.
    chordwidget.on_apply_octave_toggled.connect(std::bind(&ActionGUI::OnApplyOctaveToogled, this, std::placeholders::_1));
    chord.on_change.connect(std::bind(&ActionGUI::OnChordChanged, this));

    SequencerManager::on_sequencer_list_changed.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdateSequencerList(); });});

    //And hide the window at first.
    shown = 0;
    hide();

}


ActionGUI::~ActionGUI(){
}

void ActionGUI::SwitchTarget(Action* t){
    target = t;

    UpdateEverything();
}

void ActionGUI::OnOKClicked(){
    //Updating corresponding row, and hiding the window.
    if(target) target->on_changed();
    hide();

}

void ActionGUI::OnShow(){
    shown = 1;
    SetupTreeModels(); //important that this has to be done befor UpdateEverything, otherwise the SetSeqCombos methou wouldn't be able to select the sequencer
    UpdateEverything();
}

void ActionGUI::OnHide(){
    shown = 0;
}

void ActionGUI::UpdateSequencerList(){
    if (shown){
        SetupTreeModels();
        // Maybe this target does not reference a sequencer - but we
        // don't have to care, since the combo is not visible in such
        // case.
        SetSeqCombos(target->target_seq.lock());
    }
}

void ActionGUI::UpdateEverything(){
    if (!target) return;
    //For following: see we_are_copying_data_from_parent_action_so_do_not_handle_signals delaration.
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = true;

    SetTypeCombo(target->type);
    UpdateVisibleLines();

    int type = target->type;
    switch (type){
        case Action::NONE:
        case Action::SYNC:
            break;
        case Action::SEQ_ON_OFF_TOGGLE:
            SetSeqCombos(target->target_seq.lock());
            switch (target->args[2]){
                case 0:
                    on_off_toggle_OFF.set_active(1);
                    break;
                case 1:
                    on_off_toggle_ON.set_active(1);
                    break;
                case 2:
                    on_off_toggle_TOGGLE.set_active(1);
                    break;
            }
            break;
        case Action::SEQ_CHANGE_PATTERN:
            SetSeqCombos(target->target_seq.lock());
            pattern_button.set_value(target->args[2]);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(target->args[1]);
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            SetSeqCombos(target->target_seq.lock());
            notenr_button.set_value(target->args[2]);
            chordseq_button.set_value(target->args[3]);
            break;
        case Action::SEQ_CHANGE_CHORD:
            SetSeqCombos(target->target_seq.lock());
            target->chord.CopyInto(chord);
            chordwidget.Update();
            chordwidget.UpdateApplyOctave(target->args[3]);
            break;
        case Action::SEQ_PLAY_ONCE:
            SetSeqCombos(target->target_seq.lock());
            break;
        case Action::PLAY_PAUSE:
            switch(target->args[1]){
                case 0:
                    play_OFF.set_active(1);
                    break;
                case 1:
                    play_ON.set_active(1);
                    break;
                case 2:
                    play_TOGGLE.set_active(1);
                    break;
            }
            break;
        case Action::SEQ_TRANSPOSE_OCTAVE:
            SetSeqCombos(target->target_seq.lock());
            octave_spinbutton.set_value(target->args[2]);
            break;
        case Action::TOGGLE_PASS_MIDI:
            break;
        default:
            break;


    }
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = false;

    label_preview.set_text(target->GetLabel());
}

void ActionGUI::UpdateVisibleLines(){
    if (!target) return;
    //if(!Types_combo.get_active()) return; //nothing is selected
    //Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = target->type;

    //Hide all, and show required ones.
    line_seq.hide();
    AllSeqs_combo.hide();
    NoteSeqs_combo.hide();
    line_tempo.hide();
    line_set_one_note.hide();
    line_chord.hide();
    line_on_off_toggle.hide();
    line_play.hide();
    line_pattern.hide();
    line_octave.hide();
    switch (type){
        case Action::NONE:
            break;
        case Action::SYNC:
            break;
        case Action::SEQ_ON_OFF_TOGGLE:
            line_seq.show();
            AllSeqs_combo.show();
            line_on_off_toggle.show();
            break;
        case Action::SEQ_PLAY_ONCE:
            line_seq.show();
            AllSeqs_combo.show();
            break;
        case Action::SEQ_CHANGE_PATTERN:
            line_seq.show();
            AllSeqs_combo.show();
            line_pattern.show();
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            line_seq.show();
            NoteSeqs_combo.show();
            line_set_one_note.show();
            break;
        case Action::SEQ_CHANGE_CHORD:
            line_seq.show();
            NoteSeqs_combo.show();
            line_chord.show();
            break;
        case Action::TEMPO_SET:
            line_tempo.show();
            break;
        case Action::PLAY_PAUSE:
            line_play.show();
            break;
        case Action::SEQ_TRANSPOSE_OCTAVE:
            line_seq.show();
            NoteSeqs_combo.show();
            line_octave.show();
            break;
        case Action::TOGGLE_PASS_MIDI:
            break;
        default:
            break;

    }
    resize(2,2);

}

void ActionGUI::OnTypeChanged(){
    if(we_are_copying_data_from_parent_action_so_do_not_handle_signals) return;
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_action_types.type];
    target->type = type;

    UpdateVisibleLines();
    InitType(type);

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::SetTargetSeq() {
    auto iter = AllSeqs_combo.get_active();
    if(iter){
        target->target_seq = (*(iter))[m_col_seqs.seq];
    }else{
        target->target_seq.reset();
    }
}

void ActionGUI::InitType(int action_type){

    switch (action_type){
        case Action::NONE:
        case Action::SYNC:
            break;
        case Action::SEQ_ON_OFF_TOGGLE:
            AllSeqs_combo.set_active(0);
            SetTargetSeq();
            on_off_toggle_TOGGLE.set_active(1); //it does not triggler signal_clicked, so we have to set the mode mannually!
            target->args[2]=2;
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            NoteSeqs_combo.set_active(0);
            SetTargetSeq();
            notenr_button.set_value(1.0);
            target->args[2] = 1;
            chordseq_button.set_value(0.0);
            break;
        case Action::SEQ_CHANGE_CHORD:
            NoteSeqs_combo.set_active(0);
            SetTargetSeq();
            target->args[3] = 0;
            chordwidget.Update();
            break;
        case Action::SEQ_CHANGE_PATTERN:
            AllSeqs_combo.set_active(0);
            SetTargetSeq();
            pattern_button.set_value(0.0);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(DEFAULT_TEMPO);
            break;
        case Action::SEQ_PLAY_ONCE:
            AllSeqs_combo.set_active(0);
            break;
        case Action::PLAY_PAUSE:
            play_TOGGLE.set_active(1); //it does not triggler signal_clicked, so we have to set the mode mannually!
            target->args[1]=2;
            break;
        case Action::SEQ_TRANSPOSE_OCTAVE:
            NoteSeqs_combo.set_active(0);
            octave_spinbutton.set_value(1.0);
            break;
        case Action::TOGGLE_PASS_MIDI:
            break;
        default:
            break;
    }

}


void ActionGUI::OnTempoChanged(){

    if(target->type == Action::TEMPO_SET){
        target->args[1] = tempo_button.get_value();
    }else *err << _("Error: tempo has changed, while action is not tempo-type.") << ENDL;

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnAllSeqComboChanged(){
    if(we_are_copying_data_from_parent_action_so_do_not_handle_signals) return;
    if(!AllSeqs_combo.get_active()) return; //empty selection
    if(target->type != Action::SEQ_ON_OFF_TOGGLE &&
       target->type != Action::SEQ_PLAY_ONCE &&
       target->type != Action::SEQ_CHANGE_PATTERN &&
       target->type != Action::SEQ_TRANSPOSE_OCTAVE)
        return;

    target->target_seq = (*(AllSeqs_combo.get_active()))[m_col_seqs.seq];

    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);

}

void ActionGUI::OnNoteSeqComboChanged(){
    if(we_are_copying_data_from_parent_action_so_do_not_handle_signals) return;
    if(!NoteSeqs_combo.get_active()) return; //empty selection
    if(target->type != Action::SEQ_CHANGE_ONE_NOTE &&
       target->type != Action::SEQ_CHANGE_CHORD &&
       target->type != Action::SEQ_TRANSPOSE_OCTAVE)
        return;

    target->target_seq = (*(NoteSeqs_combo.get_active()))[m_col_seqs.seq];

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);

}

void ActionGUI::OnChordChanged(){
    if(target->type != Action::SEQ_CHANGE_CHORD)
        return;

    chord.CopyInto(target->chord);

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnNoteSeqChanged(){
    if(target->type != Action::SEQ_CHANGE_ONE_NOTE)
        return;

    target->args[3] = chordseq_button.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnNoteNrChanged(){
    if(target->type != Action::SEQ_CHANGE_ONE_NOTE)
        return;

    target->args[2] = notenr_button.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnOnOffToggleChanged(){
    if(target->type != Action::SEQ_ON_OFF_TOGGLE)
        return;

    if(on_off_toggle_OFF.get_active()) target->args[2] = 0;
    else if(on_off_toggle_ON.get_active()) target->args[2] = 1;
    else if(on_off_toggle_TOGGLE.get_active()) target->args[2] = 2;

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnPlayOnOffToggleClicked(){
    if(target->type != Action::PLAY_PAUSE)
        return;

    if(play_OFF.get_active()) target->args[1] = 0;
    else if(play_ON.get_active()) target->args[1] = 1;
    else if(play_TOGGLE.get_active()) target->args[1] = 2;

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnOctaveChanged(){
    if(target->type != Action::SEQ_TRANSPOSE_OCTAVE)
        return;

    target->args[2] = octave_spinbutton.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}
void ActionGUI::OnPatternChanged(){
    if(target->type != Action::SEQ_CHANGE_PATTERN)
        return;

    target->args[2] = pattern_button.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

void ActionGUI::OnApplyOctaveToogled(bool apply){
    if(target->type != Action::SEQ_CHANGE_CHORD)
        return;
    target->args[3] = apply;

    label_preview.set_text(target->GetLabel());
    target->on_changed();

    Files::SetFileModified(1);
}

//====^^Add new action gui callbacks above ^^==
//======================================
void ActionGUI::SetupTreeModels(){
    m_ref_treemodel_allseqs->clear();
    m_ref_treemodel_noteseqs->clear();
    Gtk::TreeModel::Row row;
    for(auto seq : SequencerManager::GetAll()){
        if(seq->GetType() == SEQ_TYPE_NOTE){
            row = *(m_ref_treemodel_noteseqs->append());
            row[m_col_seqs.seq] = seq;
            row[m_col_seqs.name] = seq->GetName();
        }
        row = *(m_ref_treemodel_allseqs->append());
        row[m_col_seqs.seq] = seq;
        row[m_col_seqs.name] = seq->GetName();
    }
}

void ActionGUI::SetTypeCombo(int type){
    Gtk::TreeModel::iterator iter = TreeModel_ActionTypes->get_iter("0");
    for (;iter;iter++){
        if ((*iter)[m_columns_action_types.type]==type){
            Types_combo.set_active(iter);
            break;
        }

    }

}

void ActionGUI::SetSeqCombos(std::shared_ptr<Sequencer> seq){
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = 1;
    Gtk::TreeModel::iterator iter = m_ref_treemodel_allseqs->get_iter("0");
    for (;iter;iter++){
        std::weak_ptr<Sequencer> s = (*iter)[m_col_seqs.seq];
        if (s.lock() == seq){
            AllSeqs_combo.set_active(iter);
            break;
        }
    }
    iter = m_ref_treemodel_noteseqs->get_iter("0");
    for (; iter; iter++) {
        std::weak_ptr<Sequencer> s = (*iter)[m_col_seqs.seq];
        if (s.lock() == seq) {
            NoteSeqs_combo.set_active(iter);
            break;
        }
    }
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = 0;
}
