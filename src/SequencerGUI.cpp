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


#include "global.h"
#include "SequencerGUI.h"
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "Event.h"
#include "ChordWidget.h"
#include "Files.h"
SequencerWindow::SequencerWindow(Sequencer* prt)
{
    *dbg << "constructing new SequencerWindow\n";

    parent = prt;
    chordwidget = new ChordWidget(&prt->chord);

    set_title(parent->name);
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);
    
    main_vbox.pack_start(upper_box);
    main_vbox.pack_start(box_of_chord);
    box_of_chord.pack_start(*chordwidget);
    main_vbox.pack_start(box_of_sliders);
    main_vbox.pack_start(line_zero, Gtk::PACK_SHRINK);
    main_vbox.pack_start(low_hbox);

    InitSeqSliders();

    low_hbox.pack_start(spinners_vbox,Gtk::PACK_SHRINK);
    low_hbox.pack_start(toggle_vbox,Gtk::PACK_SHRINK);
    spinners_vbox.pack_start(line_one, Gtk::PACK_SHRINK);
    spinners_vbox.pack_start(line_two, Gtk::PACK_SHRINK);
    line_one.pack_end(channel_button,Gtk::PACK_SHRINK);
    line_one.pack_end(channellabel,Gtk::PACK_SHRINK);
    line_two.pack_end(volume_button,Gtk::PACK_SHRINK);
    line_two.pack_end(volumelabel,Gtk::PACK_SHRINK);
    line_zero.pack_end(active_sequence,Gtk::PACK_SHRINK);
    line_zero.pack_end(activesequencelabel,Gtk::PACK_SHRINK);
    line_zero.pack_end(set_as_active_sequence,Gtk::PACK_SHRINK);
    set_as_active_sequence.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnSetAsActiveSequenceClicked));
    channellabel.set_text(_("MIDI channel:"));
    volumelabel.set_text(_("Volume:"));
    activesequencelabel.set_text(_("Active sequence:"));
    set_as_active_sequence.set_label(_("Set as active sequence"));
    volume_button.set_range(0,127);
    channel_button.set_range(1,16);
    volume_button.set_increments(1,16);
    channel_button.set_increments(1,1);
    volume_button.set_value(parent->GetVolume());
    volume_button.property_value() = (double)parent->GetVolume();
    channel_button.set_value(parent->GetChannel());
    volume_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnVolumeChanged));
    channel_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
    UpdateActiveSeqRange();//                <----
    active_sequence.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnActiveSequenceChanged));
    toggle_vbox.pack_start(tgl_mute);
    toggle_vbox.pack_start(tgl_apply_mainnote);
    tgl_mute.set_label(_("On"));
    tgl_apply_mainnote.set_label(_("Apply main note"));
    tgl_mute.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnToggleMuteToggled));
    tgl_mute.set_active(parent->on);
    tgl_apply_mainnote.signal_toggled().connect(mem_fun(*this,&SequencerWindow::OnToggleApplyMainNoteToggled));
    tgl_apply_mainnote.set_active(parent->apply_mainnote);

    //lengths selector
    upper_box.pack_start(reslabel,Gtk::PACK_SHRINK);
    reslabel.set_text(_("Resolution:"));
    upper_box.pack_start(resolution_box,Gtk::PACK_SHRINK);
    m_refTreeModel_res = Gtk::ListStore::create(m_Columns_resol);
    resolution_box.set_model(m_refTreeModel_res);


    int resolutions[7] = RESOLUTIONS;
    for (int x = 0; x < RESOLUTIONS_NUM; x++){
        Gtk::TreeModel::Row row = *(m_refTreeModel_res->append());
        row[m_Columns_resol.resol] = resolutions[x];
        if(parent->resolution==resolutions[x]) resolution_box.set_active(x);
    }
    resolution_box.pack_start(m_Columns_resol.resol);
    resolution_box.signal_changed().connect(mem_fun(*this,&SequencerWindow::OnResolutionChanged));
    
    
    upper_box.pack_start(lenlabel,Gtk::PACK_SHRINK);
    lenlabel.set_text(_("Length:"));
    upper_box.pack_start(length_box,Gtk::PACK_SHRINK);
    m_refTreeModel_len = Gtk::ListStore::create(m_Columns_len);
    length_box.set_model(m_refTreeModel_len);

    char temp[10];
    double lengths[7] = LENGTHS;
    for (int x = 0; x < LENGTHS_NUM; x++){
        Gtk::TreeModel::Row row = *(m_refTreeModel_len->append());
        row[m_Columns_len.len] = lengths[x];
        sprintf(temp,"%g",lengths[x]);
        row[m_Columns_len.text] = temp;
        if(parent->length==lengths[x]) length_box.set_active(x);
    }
    length_box.pack_start(m_Columns_len.text);
    length_box.signal_changed().connect(mem_fun(*this,&SequencerWindow::OnLengthChanged));
    
    add(main_vbox);

    signal_key_press_event().connect(&FindAndProcessEventsKeyPress);
    
    show_all_children(1);
    //show();
    hide(); //hide at start, but let the children be shown
}
SequencerWindow::~SequencerWindow(){
    delete chordwidget;
}

void SequencerWindow::OnSequenceChanged(int seq){
    *dbg << "seq changed";
    parent->SetSequenceNote(active_sequence.get_value(),seq,sequence_scales[seq]->get_value());
    Files::SetFileModified(1);
}

void SequencerWindow::UpdateValues(){
//TODO const
    //finding a num to a res should be const complexity!
    int resolutions[RESOLUTIONS_NUM] = RESOLUTIONS;
    char temp[10];
    for (int x = 0; x < RESOLUTIONS_NUM; x++){
        sprintf(temp,"%d",x);
        //Gtk::TreeModel::Row row = *(m_refTreeModel_res->get_iter(temp));
        if (resolutions[x] == (parent->resolution)){resolution_box.set_active(x);continue;}
    }

    double lengths[7] = LENGTHS;
    for (int x = 0; x < LENGTHS_NUM; x++){
        sprintf(temp,"%d",x);
        //Gtk::TreeModel::Row row = *(m_refTreeModel_len->get_iter(temp));
        if(parent->length==lengths[x]) length_box.set_active(x);
    }

    InitSeqSliders();
    UpdateChord();
}


    void SequencerWindow::UpdateChord(){
        chordwidget->Update();
    }

void SequencerWindow::OnChannelChanged(){

    parent->channel = channel_button.get_value();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::OnToggleMuteToggled(){
    *dbg << "Toggle in GUI signal called\n";
    parent->on = tgl_mute.get_active();
    parent->play_once_phase = 0;
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);

    //Files::SetFileModified(1); come on, do not write mutes.
}

void SequencerWindow::OnToggleApplyMainNoteToggled(){
    parent->apply_mainnote = tgl_apply_mainnote.get_active();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::OnVolumeChanged(){
    parent->volume = volume_button.get_value();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::OnResolutionChanged(){
    Gtk::TreeModel::Row row = *(resolution_box.get_active());

    parent->SetResolution(row[m_Columns_resol.resol]);
    InitSeqSliders();
    
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::InitSeqSliders(){

    for(unsigned int x = 0; x < sequence_scales.size();x++){
        if(!sequence_scales[x]) continue;
        box_of_sliders.remove(*sequence_scales[x]);
    }
    sequence_scales.clear();
    resize(2,2);


    sequence_scales.resize(parent->resolution,NULL);
    for (int x= 0; x < parent->resolution; x++){
        sequence_scales[x] = Gtk::manage<Gtk::HScale>(new Gtk::HScale(0,6,1));
        sequence_scales[x]->set_value(parent->GetSequenceNote(active_sequence.get_value(),x));
        sequence_scales[x]->set_increments(1.0,1.0);
        //sequence_scales[x]->set_draw_value(0);  //<- hehe, here is a bug in gtkmm ^^
        sequence_scales[x]->set_value_pos(Gtk::POS_RIGHT); //<- temporary workaround
        sequence_scales[x]->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnSequenceChanged),x));
        sequence_scales[x]->show();
        box_of_sliders.pack_start(*sequence_scales[x]);
    }

}

void SequencerWindow::OnLengthChanged(){
    Gtk::TreeModel::Row row = *(length_box.get_active());
    *dbg << "setting length\n";
    parent->length = row[m_Columns_len.len];
    *dbg << "will refresh row\n";
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);

    Files::SetFileModified(1);

}

void SequencerWindow::OnChordWidgetChanged(){
    Files::SetFileModified(1);
}

void SequencerWindow::UpdateActiveSeqRange(){
    int v = active_sequence.get_value();
    active_sequence.set_range(0.0,(double)parent->sequences.size()-1);
    active_sequence.set_increments(1.0,1.0);
    active_sequence.set_value(v); //if it's too high, it will change to largest possible
}

void SequencerWindow::OnActiveSequenceChanged(){
    //changing notepad tab labels and active tab
    //changing active seq in parent, but only if in range!!!!
    
}

void SequencerWindow::OnSetAsActiveSequenceClicked(){
    //recognize which is now shown and set it to be active
}