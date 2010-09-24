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

    add(main_box);
    main_box.set_spacing(5);

    main_box.pack_start(line_type);
    main_box.pack_start(line_seq);
    main_box.pack_start(line_note);
    main_box.pack_start(line_tempo);
    main_box.pack_start(line_volume);
    main_box.pack_start(separator);
    main_box.pack_start(label_preview);
    main_box.pack_start(ok_button);

    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_seq.pack_start(label_seq,Gtk::PACK_SHRINK);
    line_note.pack_start(label_note,Gtk::PACK_SHRINK);
    line_tempo.pack_start(label_tempo,Gtk::PACK_SHRINK);
    line_volume.pack_start(label_volume,Gtk::PACK_SHRINK);

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_seq.pack_start(Seqs_combo,Gtk::PACK_SHRINK);
    line_note.pack_start(note_button,Gtk::PACK_SHRINK);
    line_tempo.pack_start(tempo_button,Gtk::PACK_SHRINK);
    line_volume.pack_start(vol_button,Gtk::PACK_SHRINK);

    note_button.set_range(0.0,127.0);
    note_button.set_increments(1.0,12.0);
    note_button.signal_value_changed().connect(mem_fun(*this,&ActionGUI::OnNoteChanged));
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
    TypeChanged(); // to hide some of widgets according to the type
    hide();

}


ActionGUI::ActionGUI(const ActionGUI& orig){
}


ActionGUI::~ActionGUI(){
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
    TypeChanged();
    int type = parent->type;
    switch (type){
        case Action::NONE:
            break;
        case Action::SEQ_ON:
            SetSeqCombo(parent->arg1);
            break;
        case Action::SEQ_OFF:
            SetSeqCombo(parent->arg1);
            break;
        case Action::SEQ_TOGGLE:
            SetSeqCombo(parent->arg1);
            break;
        case Action::SEQ_VOLUME_SET:
            SetSeqCombo(parent->arg1);
            vol_button.set_value(parent->arg2);
            break;
        case Action::MAINOTE_SET:
            note_button.set_value(parent->arg1);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(parent->arg1);
            break;



    }


}

void ActionGUI::TypeChanged(){
    if(!Types_combo.get_active()) return; //nothing is selected
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_action_types.type];
    line_seq.hide();
    line_note.hide();
    line_tempo.hide();
    line_volume.hide();
    switch (type){
        case Action::NONE:
            break;
        case Action::SEQ_ON:
        case Action::SEQ_OFF:
        case Action::SEQ_TOGGLE:
            line_seq.show();
            break;
        case Action::SEQ_VOLUME_SET:
            line_seq.show();
            line_volume.show();
            break;
        case Action::MAINOTE_SET:
            line_note.show();
            break;
        case Action::TEMPO_SET:
            line_tempo.show();
            break;



    }
    resize(2,2);
    
}

void ActionGUI::OnTypeChanged(){
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_action_types.type];
    parent->type = type;
    TypeChanged();
    switch (type){
        case Action::NONE:
            break;
        case Action::SEQ_ON:
            Seqs_combo.set_active(0);
            break;
        case Action::SEQ_OFF:
            Seqs_combo.set_active(0);
            break;
        case Action::SEQ_TOGGLE:
            Seqs_combo.set_active(0);
            break;
        case Action::SEQ_VOLUME_SET:
            Seqs_combo.set_active(0);
            vol_button.set_value(DEFAULT_VOLUME);
            break;
        case Action::MAINOTE_SET:
            note_button.set_value(60.0);
            break;
        case Action::TEMPO_SET:
            tempo_button.set_value(120.0);
            break;



    }
    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);
}

void ActionGUI::OnNoteChanged(){

    if(parent->type == Action::MAINOTE_SET){
        parent->arg1 = note_button.get_value();
    }else *err << _("Error: note has changed, while action is not note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void ActionGUI::OnTempoChanged(){

    if(parent->type == Action::TEMPO_SET){
        parent->arg1 = tempo_button.get_value();
    }else *err << _("Error: tempo has changed, while action is not tempo-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void ActionGUI::OnSeqChanged(){
    if(!Seqs_combo.get_active()) return; //empty selection
    if(parent->type == Action::SEQ_OFF || parent->type == Action::SEQ_ON || parent->type == Action::SEQ_TOGGLE || parent->type == Action::SEQ_VOLUME_SET){
            parent->arg1 = (*(Seqs_combo.get_active()))[m_columns_sequencers.col_ID];
    }else *err << _("Error: sequencer has changed, while action is not sequencer-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

    
}

void ActionGUI::OnVolumeChanged(){

    if(parent->type == Action::SEQ_VOLUME_SET){
        parent->arg2 = vol_button.get_value();
    }else *err << _("Error: volume has changed, while action is not volume-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

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