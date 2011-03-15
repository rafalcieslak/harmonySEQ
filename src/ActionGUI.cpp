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
#include "ActionGUI.h"
#include "Action.h"
#include "TreeModels.h"
#include "EventsWindow.h"
#include "messages.h"
#include "Files.h"

ActionGUI::ActionGUI(Action *prt):
                    chordwidget(&prt->chord)
{
    //Set the pointer to point to the parent
    parent = prt;

    //by defalut default
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = false;

    //window's title
    set_title(_("Action"));
 
    set_border_width(5);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    set_resizable(0);
    
    add(main_box);
    main_box.set_spacing(5);

    main_box.pack_start(line_type);
    main_box.pack_start(line_seq);
    main_box.pack_start(line_on_off_toggle);
    main_box.pack_start(line_tempo);
    main_box.pack_start(line_volume);
    main_box.pack_start(line_pattern);
    main_box.pack_start(line_set_one_note);
    main_box.pack_start(line_chord);
    main_box.pack_start(line_play);
    main_box.pack_start(separator);//==========
    main_box.pack_start(label_preview);
    main_box.pack_start(ok_button);

    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_seq.pack_start(label_seq,Gtk::PACK_SHRINK);
    line_tempo.pack_start(label_tempo,Gtk::PACK_SHRINK);
    line_volume.pack_start(label_volume,Gtk::PACK_SHRINK);
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
    on_off_toggle_OFF.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnOnOffToggleChanged));
    on_off_toggle_ON.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnOnOffToggleChanged));
    on_off_toggle_TOGGLE.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnOnOffToggleChanged));

    line_play.pack_start(play_TOGGLE,Gtk::PACK_SHRINK);
    line_play.pack_start(play_ON,Gtk::PACK_SHRINK);
    line_play.pack_start(play_OFF,Gtk::PACK_SHRINK);
    group = play_OFF.get_group();
    play_ON.set_group(group);
    play_TOGGLE.set_group(group);
    play_OFF.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnPlayOnOffToggleClicked));
    play_ON.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnPlayOnOffToggleClicked));
    play_TOGGLE.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnPlayOnOffToggleClicked));

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_seq.pack_start(Seqs_combo,Gtk::PACK_SHRINK);
    line_pattern.pack_start(pattern_button,Gtk::PACK_SHRINK);
    line_tempo.pack_start(tempo_button,Gtk::PACK_SHRINK);
    line_volume.pack_start(vol_button,Gtk::PACK_SHRINK);
    line_chord.pack_start(chordwidget);

    notenr_button.set_range(1.0,6.0);
    notenr_button.set_increments(1.0,2.0);
    notenr_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnNoteNrChanged));
    chordseq_button.set_range(-48.0,48.0);
    chordseq_button.set_increments(1.0,12.0);
    chordseq_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnNoteSeqChanged));
    pattern_button.set_range(0.0,100.0);
    pattern_button.set_increments(1.0,10.0);
    pattern_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnPatternChanged));
    tempo_button.set_range(30.0,320.0);
    tempo_button.set_increments(1.0,20.0);
    tempo_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnTempoChanged));
    vol_button.set_range(0.0,127.0);
    vol_button.set_increments(1.0,16.0);
    vol_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnVolumeChanged));

    chordwidget.on_changed.connect(mem_fun(*this,&ActionGUI::OnChordWidgetChanged));

    label_type.set_text(_("Type:"));
    label_seq.set_text(_("Sequencer:"));
    label_tempo.set_text(_("Tempo:"));
    label_volume.set_text(_("Volume:"));
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
    
    ok_button.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnOKClicked));

    Types_combo.set_model(TreeModel_ActionTypes);
    Types_combo.pack_start(m_columns_action_types.label);
    SetTypeCombo(parent->type); //Setting the typecombo BEFORE connecting the signal is ESSENTIAL, since otherwise when the type in Types_combo is changed (by setting it to parent->type), it emits a signal
    Types_combo.signal_changed().connect(mem_fun(*this,&ActionGUI::OnTypeChanged));
    Seqs_combo.set_model(TreeModel_sequencers);
    Seqs_combo.pack_start(m_columns_sequencers.col_name);
    Seqs_combo.signal_changed().connect(mem_fun(*this,&ActionGUI::OnSeqChanged));

    signal_show().connect(mem_fun(*this,&ActionGUI::OnShow));

    //Setting the label's text, to parent action's name.
    label_preview.set_text(parent->GetLabel());

    //Show all children widgets
    show_all_children(1);

     //Hide some of widgets according to the type
    ChangeVisibleLines();

    //And hide the window at first.
    hide();

}


ActionGUI::~ActionGUI(){
}

void ActionGUI::OnOKClicked(){
    //Updating corresponding row, and hiding the window.
    eventswindow->UpdateRow(parent->row_in_event_window);
    hide();

}

void ActionGUI::OnShow(){
    set_transient_for(*eventswindow);
    UpdateValues();
}

void ActionGUI::UpdateChordwidget(){
    chordwidget.Update();
}

void ActionGUI::UpdateValues(){
    //For following: see we_are_copying_data_from_parent_action_so_do_not_handle_signals delaration.
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = true;
    SetTypeCombo(parent->type); 
    ChangeVisibleLines();
    int type = parent->type;
    switch (type){
        case Action::NONE:
        case Action::SYNC:
            break;
        case Action::SEQ_ON_OFF_TOGGLE:
            SetSeqCombo(parent->args[1]);
            switch (parent->args[2]){
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
        case Action::SEQ_VOLUME_SET:
            SetSeqCombo(parent->args[1]);
            vol_button.set_value(parent->args[2]);
            break;
        case Action::SEQ_CHANGE_PATTERN:
            SetSeqCombo(parent->args[1]);
            pattern_button.set_value(parent->args[2]);
            break;
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(parent->args[1]);
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            SetSeqCombo(parent->args[1]);
            notenr_button.set_value(parent->args[2]);
            chordseq_button.set_value(parent->args[3]);
            break;
        case Action::SEQ_CHANGE_CHORD:
            SetSeqCombo(parent->args[1]);
            chordwidget.Update();
            break;
        case Action::SEQ_PLAY_ONCE:
            SetSeqCombo(parent->args[1]);
            break;
        case Action::PLAY_PAUSE:
            switch(parent->args[1]){
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
        case Action::TOGGLE_PASS_MIDI:
            break;
        default:
            break;


    }
    we_are_copying_data_from_parent_action_so_do_not_handle_signals = false;

}

void ActionGUI::ChangeVisibleLines(){
    //if(!Types_combo.get_active()) return; //nothing is selected
    //Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = parent->type;

    //Hide all, and show required ones.
    line_seq.hide();
    line_tempo.hide();
    line_volume.hide();
    line_set_one_note.hide();
    line_chord.hide();
    line_on_off_toggle.hide();
    line_play.hide();
    line_pattern.hide();
    switch (type){
        case Action::NONE:
            break;
        case Action::SYNC:
            break;
        case Action::SEQ_ON_OFF_TOGGLE:
            line_seq.show();
            line_on_off_toggle.show();
            break;
        case Action::SEQ_PLAY_ONCE:
            line_seq.show();
            break;
        case Action::SEQ_VOLUME_SET:
            line_seq.show();
            line_volume.show();
            break;
        case Action::SEQ_CHANGE_PATTERN:
            line_seq.show();
            line_pattern.show();
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            line_seq.show();
            line_set_one_note.show();
            break;
        case Action::SEQ_CHANGE_CHORD:
            line_seq.show();
            line_chord.show();
            break;
        case Action::TEMPO_SET:
            line_tempo.show();
            break;
        case Action::PLAY_PAUSE:
            line_play.show();
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
    parent->type = type;
    ChangeVisibleLines();
    InitType();

    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
    Files::SetFileModified(1);
}

void ActionGUI::InitType(){

    switch (parent->type){
        case Action::NONE:
        case Action::SYNC:
            break;
        case Action::SEQ_ON_OFF_TOGGLE:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_handle];
            on_off_toggle_TOGGLE.set_active(1); //it does not triggler signal_clicked, so we have to set the mode mannually!
            parent->args[2]=2;
            break;
        case Action::SEQ_VOLUME_SET:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_handle];
            vol_button.set_value(DEFAULT_VOLUME);
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_handle];
            notenr_button.set_value(1.0);
            chordseq_button.set_value(0.0);
            break;
        case Action::SEQ_CHANGE_CHORD:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_handle];
            chordwidget.Update();
            break;
        case Action::SEQ_CHANGE_PATTERN:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_handle];
            pattern_button.set_value(0.0);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(120.0);
            break;
        case Action::SEQ_PLAY_ONCE:
            Seqs_combo.set_active(0);
            break;
        case Action::PLAY_PAUSE:
            play_TOGGLE.set_active(1); //it does not triggler signal_clicked, so we have to set the mode mannually!
            parent->args[1]=2;
            break;
        case Action::TOGGLE_PASS_MIDI:
            break;
        default:
            break;
    }

}


void ActionGUI::OnTempoChanged(){

    if(parent->type == Action::TEMPO_SET){
        parent->args[1] = tempo_button.get_value();
    }else *err << _("Error: tempo has changed, while action is not tempo-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

void ActionGUI::OnSeqChanged(){
    if(!Seqs_combo.get_active()) return; //empty selection
    if(parent->type == Action::SEQ_ON_OFF_TOGGLE || parent->type == Action::SEQ_VOLUME_SET || parent->type == Action::SEQ_CHANGE_ONE_NOTE || parent->type == Action::SEQ_CHANGE_CHORD || parent->type == Action::SEQ_PLAY_ONCE||parent->type == Action::SEQ_CHANGE_PATTERN){
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_handle];
    }else *err << _("Error: sequencer has changed, while action is not sequencer-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
    
}

void ActionGUI::OnVolumeChanged(){

    if(parent->type == Action::SEQ_VOLUME_SET){
        parent->args[2] = vol_button.get_value();
    }else *err << _("Error: volume has changed, while action is not volume-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

void ActionGUI::OnNoteSeqChanged(){
    if(parent->type == Action::SEQ_CHANGE_ONE_NOTE){
        parent->args[3] = chordseq_button.get_value();
    }else *err << _("Error: note to set has changed, while action is not set-seq-note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

void ActionGUI::OnNoteNrChanged(){
    if(parent->type == Action::SEQ_CHANGE_ONE_NOTE){
        parent->args[2] = notenr_button.get_value();
    }else *err << _("Error: note number has changed, while action is not set-seq-note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

void ActionGUI::OnOnOffToggleChanged(){
    if(parent->type == Action::SEQ_ON_OFF_TOGGLE){
        if(on_off_toggle_OFF.get_active()) parent->args[2] = 0;
        else if(on_off_toggle_ON.get_active()) parent->args[2] = 1;
        else if(on_off_toggle_TOGGLE.get_active()) parent->args[2] = 2;
    }else *err << _("Error: on-off-toggle has changed, while action is not on-off-toggle-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

void ActionGUI::OnPlayOnOffToggleClicked(){
    if(parent->type == Action::PLAY_PAUSE){
        if(play_OFF.get_active()) parent->args[1] = 0;
        else if(play_ON.get_active()) parent->args[1] = 1;
        else if(play_TOGGLE.get_active()) parent->args[1] = 2;
    }else *err << _("Error: play-pause-toggle has changed, while action is not play-pause-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

void ActionGUI::OnChordWidgetChanged(){
        //chord widget updates chord settings automatically, we just need to update labels

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);

}

void ActionGUI::OnPatternChanged(){

    if(parent->type == Action::SEQ_CHANGE_PATTERN){
        parent->args[2] = pattern_button.get_value();
    }else *err << _("Error: pattern has changed, while action is not pattern-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    Files::SetFileModified(1);
}

//====^^Add new action gui callbacks above ^^==
//======================================
void ActionGUI::SetTypeCombo(int type){
    Gtk::TreeModel::iterator iter = TreeModel_ActionTypes->get_iter("0");
    for (;iter;iter++){
        if ((*iter)[m_columns_action_types.type]==type){
            Types_combo.set_active(iter);
            break;
        }

    }

}

void ActionGUI::SetSeqCombo(int seq){
    Gtk::TreeModel::iterator iter = TreeModel_sequencers->get_iter("0");
    for (;iter;iter++){
        if ((*iter)[m_columns_sequencers.col_handle]==seq){
            Seqs_combo.set_active(iter);
            break;
        }

    }

}
