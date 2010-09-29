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


ActionGUI::ActionGUI(Action *prt){
    parent = prt;
    set_title(_("Action"));
 
    set_border_width(5);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

    for (int x = 0; x < 6; x++) notes6_buttons[x] = new Gtk::SpinButton;

    add(main_box);
    main_box.set_spacing(5);

    main_box.pack_start(line_type);
    main_box.pack_start(line_seq);
    main_box.pack_start(line_note);
    main_box.pack_start(line_tempo);
    main_box.pack_start(line_volume);
    main_box.pack_start(line_set_one_note);
    main_box.pack_start(line_6notes);
    main_box.pack_start(separator);
    main_box.pack_start(label_preview);
    main_box.pack_start(ok_button);

    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_seq.pack_start(label_seq,Gtk::PACK_SHRINK);
    line_note.pack_start(label_note,Gtk::PACK_SHRINK);
    line_tempo.pack_start(label_tempo,Gtk::PACK_SHRINK);
    line_volume.pack_start(label_volume,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(label_note_nr,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(notenr_button,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(label_note_seq,Gtk::PACK_SHRINK);
    line_set_one_note.pack_start(noteseq_button,Gtk::PACK_SHRINK);
    line_6notes.pack_start(label_6notes,Gtk::PACK_SHRINK);

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_seq.pack_start(Seqs_combo,Gtk::PACK_SHRINK);
    line_note.pack_start(note_button,Gtk::PACK_SHRINK);
    line_tempo.pack_start(tempo_button,Gtk::PACK_SHRINK);
    line_volume.pack_start(vol_button,Gtk::PACK_SHRINK);
    for (int x = 0; x < 6; x++) line_6notes.pack_start(*notes6_buttons[x],Gtk::PACK_SHRINK);

    for (int x = 0; x < 6; x++){
        notes6_buttons[x]->set_range(-48.0,48.0);
        notes6_buttons[x]->set_increments(1.0,12.0);
        notes6_buttons[x]->signal_value_changed().connect(sigc::bind<int>(mem_fun(*this,&ActionGUI::OnNote6Changed),x));
    }
    note_button.set_range(0.0,127.0);
    note_button.set_increments(1.0,12.0);
    note_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnNoteChanged));
    notenr_button.set_range(1.0,6.0);
    notenr_button.set_increments(1.0,2.0);
    notenr_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnNoteNrChanged));
    noteseq_button.set_range(-48.0,48.0);
    noteseq_button.set_increments(1.0,12.0);
    noteseq_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnNoteSeqChanged));
    tempo_button.set_range(30.0,320.0);
    tempo_button.set_increments(1.0,20.0);
    tempo_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnTempoChanged));
    vol_button.set_range(0.0,127.0);
    vol_button.set_increments(1.0,16.0);
    vol_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnVolumeChanged));

    label_type.set_text(_("Type:"));
    label_seq.set_text(_("Sequencer:"));
    label_tempo.set_text(_("Tempo:"));
    label_note.set_text(_("Note:"));
    label_volume.set_text(_("Volume:"));
    label_note_nr.set_text(_("Set note "));
    label_note_seq.set_text(_(" to: "));
    label_6notes.set_text(_("Notes:"));
    ok_button.set_label(_("OK"));
    
    ok_button.signal_clicked().connect(mem_fun(*this,&ActionGUI::OnOKClicked));

    Types_combo.set_model(m_refTreeModel_ActionTypes);
    Types_combo.pack_start(m_columns_action_types.label);
    SetTypeCombo(parent->type); //Setting the typecombo BEFORE connecting the signal is ESSENTIAL, since otherwise when the type in Types_combo is changed (by setting it to parent->type), it emits a signal
    Types_combo.signal_changed().connect(mem_fun(*this,&ActionGUI::OnTypeChanged));
    Seqs_combo.set_model(m_refTreeModel_sequencers);
    Seqs_combo.pack_start(m_columns_sequencers.col_name);
    Seqs_combo.signal_changed().connect(mem_fun(*this,&ActionGUI::OnSeqChanged));

    signal_show().connect(mem_fun(*this,&ActionGUI::OnShow));

    label_preview.set_text(parent->GetLabel());
    show_all_children(1);
    ChangeVisibleLines(); // to hide some of widgets according to the type
    hide();

}


ActionGUI::ActionGUI(const ActionGUI& orig){
}


ActionGUI::~ActionGUI(){
    for (int x = 0; x <  6; x++){
        delete notes6_buttons[x];

    }
}

void ActionGUI::OnOKClicked(){
    hide();

}

void ActionGUI::OnShow(){
    set_transient_for(*eventswindow);
    UpdateValues();

}

void ActionGUI::UpdateValues(){
    SetTypeCombo(parent->type); 
    ChangeVisibleLines();
    int type = parent->type;
    switch (type){
        case Action::NONE:
            break;
        case Action::SEQ_ON:
            SetSeqCombo(parent->args[1]);
            break;
        case Action::SEQ_OFF:
            SetSeqCombo(parent->args[1]);
            break;
        case Action::SEQ_TOGGLE:
            SetSeqCombo(parent->args[1]);
            break;
        case Action::SEQ_VOLUME_SET:
            SetSeqCombo(parent->args[1]);
            vol_button.set_value(parent->args[2]);
            break;
        case Action::MAINOTE_SET:
            note_button.set_value(parent->args[1]);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(parent->args[1]);
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            SetSeqCombo(parent->args[1]);
            notenr_button.set_value(parent->args[2]);
            noteseq_button.set_value(parent->args[3]);
            break;
        case Action::SEQ_CHANGE_ALL_NOTES:
            SetSeqCombo(parent->args[1]);
            for (int x = 0; x <  6; x++) notes6_buttons[x]->set_value(parent->args[x+2]);
            break;
        case Action::SEQ_PLAY_ONCE:
            SetSeqCombo(parent->args[1]);
            break;
        default:
            break;


    }


}

void ActionGUI::ChangeVisibleLines(){
    //if(!Types_combo.get_active()) return; //nothing is selected
    //Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = parent->type;
    line_seq.hide();
    line_note.hide();
    line_tempo.hide();
    line_volume.hide();
    line_set_one_note.hide();
    line_6notes.hide();
    switch (type){
        case Action::NONE:

            break;
        case Action::SEQ_ON:
        case Action::SEQ_OFF:
        case Action::SEQ_TOGGLE:
        case Action::SEQ_PLAY_ONCE:
            line_seq.show();
            break;
        case Action::SEQ_VOLUME_SET:
            line_seq.show();
            line_volume.show();
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            line_seq.show();
            line_set_one_note.show();
            break;
        case Action::SEQ_CHANGE_ALL_NOTES:
            line_seq.show();
            line_6notes.show();
            break;
        case Action::MAINOTE_SET:
            line_note.show();
            break;
        case Action::TEMPO_SET:
            line_tempo.show();
            break;

        default:
            break;

    }
    resize(2,2);
    
}

void ActionGUI::OnTypeChanged(){
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_action_types.type];
    parent->type = type;
    ChangeVisibleLines();
    InitType();

    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
}

void ActionGUI::InitType(){

    switch (parent->type){
        case Action::NONE:
            break;
        case Action::SEQ_ON:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_ID];
            break;
        case Action::SEQ_OFF:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_ID];
            break;
        case Action::SEQ_TOGGLE:
            Seqs_combo.set_active(0);
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_ID];
            break;
        case Action::SEQ_VOLUME_SET:
            Seqs_combo.set_active(0);
            vol_button.set_value(DEFAULT_VOLUME);
            break;
        case Action::SEQ_CHANGE_ONE_NOTE:
            Seqs_combo.set_active(0);
            notenr_button.set_value(1.0);
            noteseq_button.set_value(0.0);
            break;
        case Action::SEQ_CHANGE_ALL_NOTES:
            Seqs_combo.set_active(0);
            for (int x = 0; x <  6; x++) notes6_buttons[x]->set_value(0.0);
            break;
        case Action::MAINOTE_SET:
            note_button.set_value(60.0);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(120.0);
            break;
        case Action::SEQ_PLAY_ONCE:
            Seqs_combo.set_active(0);
            break;
        default:
            break;
    }

}

void ActionGUI::OnNoteChanged(){

    if(parent->type == Action::MAINOTE_SET){
        parent->args[1] = note_button.get_value();
    }else *err << _("Error: note has changed, while action is not note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void ActionGUI::OnTempoChanged(){

    if(parent->type == Action::TEMPO_SET){
        parent->args[1] = tempo_button.get_value();
    }else *err << _("Error: tempo has changed, while action is not tempo-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void ActionGUI::OnSeqChanged(){
    if(!Seqs_combo.get_active()) return; //empty selection
    if(parent->type == Action::SEQ_OFF || parent->type == Action::SEQ_ON || parent->type == Action::SEQ_TOGGLE || parent->type == Action::SEQ_VOLUME_SET || parent->type == Action::SEQ_CHANGE_ONE_NOTE || parent->type == Action::SEQ_CHANGE_ALL_NOTES || parent->type == Action::SEQ_PLAY_ONCE){
            parent->args[1] = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_ID];
    }else *err << _("Error: sequencer has changed, while action is not sequencer-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    
}

void ActionGUI::OnVolumeChanged(){

    if(parent->type == Action::SEQ_VOLUME_SET){
        parent->args[2] = vol_button.get_value();
    }else *err << _("Error: volume has changed, while action is not volume-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void ActionGUI::OnNoteSeqChanged(){
    if(parent->type == Action::SEQ_CHANGE_ONE_NOTE){
        parent->args[3] = noteseq_button.get_value();
    }else *err << _("Error: note to set has changed, while action is not set-seq-note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void ActionGUI::OnNoteNrChanged(){
    if(parent->type == Action::SEQ_CHANGE_ONE_NOTE){
        parent->args[2] = notenr_button.get_value();
    }else *err << _("Error: note number has changed, while action is not set-seq-note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);
    
}

void ActionGUI::OnNote6Changed(int n){
    if(parent->type == Action::SEQ_CHANGE_ALL_NOTES){
        parent->args[n+2] = notes6_buttons[n]->get_value();
    }else *err << _("Error: one of 6 notes to set has changed, while action is not of change-all-notes type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);
}

//====^^Add new action gui callbacks above ^^==
//======================================
void ActionGUI::SetTypeCombo(int type){
    Gtk::TreeModel::iterator iter = m_refTreeModel_ActionTypes->get_iter("0");
    for (;iter;iter++){
        if ((*iter)[m_columns_action_types.type]==type){
            Types_combo.set_active(iter);
            break;
        }

    }

}

void ActionGUI::SetSeqCombo(int seq){
    Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->get_iter("0");
    for (;iter;iter++){
        if ((*iter)[m_columns_sequencers.col_ID]==seq){
            Seqs_combo.set_active(iter);
            break;
        }

    }

}