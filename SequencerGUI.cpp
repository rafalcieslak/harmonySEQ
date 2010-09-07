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


#include "SequencerGUI.h"
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "global.h"
extern MainWindow *mainwindow;
extern debug *dbg;

SequencerWindow::SequencerWindow(Sequencer* prt){
    *dbg << "constructing new SequencerWindow\n";

    parent = prt;

    set_title(parent->name);
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);
    main_vbox.pack_start(upper_box);
    main_vbox.pack_start(box_of_notes);
    for (int x = 0; x < 6; x++){
        note_buttons[x] = new Gtk::SpinButton;
        note_buttons[x]->set_range(-128,128);
        note_buttons[x]->set_increments(1,12);
        //*dbg << parent->GetNotes(0);
        note_buttons[x]->set_value(parent->GetNotes(x));
        note_buttons[x]->signal_value_changed().connect(sigc::bind<int>(sigc::mem_fun(*this,&SequencerWindow::OnNotesChanged),x));
        box_of_notes.pack_start(*note_buttons[x]);
    }

    main_vbox.pack_start(box_of_sliders);
    InitSeqSliders();

    low_hbox.pack_start(channellabel,Gtk::PACK_SHRINK);
    low_hbox.pack_start(channel_button, Gtk::PACK_SHRINK);
    channellabel.set_text(_("MIDI channel:"));
    channel_button.set_value(1);
    channel_button.set_range(1,16);
    channel_button.set_increments(1,1);
    channel_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
    low_hbox.pack_start(toggle_vbox,Gtk::PACK_SHRINK);
    toggle_vbox.pack_start(tgl_mute);
    toggle_vbox.pack_start(tgl_apply_mainnote);
    tgl_mute.set_label(_("On"));
    tgl_apply_mainnote.set_label(_("Apply main note"));
    tgl_mute.signal_toggled().connect(mem_fun(*this,&SequencerWindow::OnToggleMuteToggled));
    tgl_mute.set_active(parent->on);
    tgl_apply_mainnote.signal_toggled().connect(mem_fun(*this,&SequencerWindow::OnToggleApplyMainNoteToggled));
    tgl_apply_mainnote.set_active(parent->apply_mainnote);
    main_vbox.pack_start(low_hbox);

    //lengths selector
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
    
    add(main_vbox);
    
    show_all_children(1);
    //show();
    hide(); //hide at start, but let the children be shown
}
SequencerWindow::~SequencerWindow(){

    
}

void SequencerWindow::OnNotesChanged(int note){
    parent->notes[note] = note_buttons[note]->get_value();

}

void SequencerWindow::OnSequenceChanged(int seq){
    *dbg << "seq changed";
    parent->sequence[seq] = sequence_scales[seq]->get_value();

}

void SequencerWindow::UpdateValues(){
    if((int)sequence_scales.size()!=parent->resolution) sequence_scales.resize(parent->resolution);
    for (int x = 0; x < parent->resolution; x++){
        sequence_scales[x]->set_value(parent->sequence[x]);
    }
    for (int x = 0; x < NOTES_CONST_SIZE; x++) {
        note_buttons[x]->set_value(parent->GetNotes(x));
    }
}

void SequencerWindow::OnChannelChanged(){

    parent->channel = channel_button.get_value();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
}

void SequencerWindow::OnToggleMuteToggled(){
    parent->on = tgl_mute.get_active();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
}

void SequencerWindow::OnToggleApplyMainNoteToggled(){
    parent->apply_mainnote = tgl_apply_mainnote.get_active();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);

}

void SequencerWindow::OnResolutionChanged(){
    Gtk::TreeModel::Row row = *(resolution_box.get_active());

    parent->SetResolution(row[m_Columns_resol.resol]);
    InitSeqSliders();
    //sequence_scales.clear();
}

void SequencerWindow::InitSeqSliders(){

    for(int x = 0; x < sequence_scales.size();x++){
        box_of_sliders.remove(*sequence_scales[x]);
    }
    sequence_scales.clear();
    resize(2,2);


    sequence_scales.resize(parent->resolution,NULL);
    for (int x= 0; x < parent->resolution; x++){
        sequence_scales[x] = Gtk::manage<Gtk::HScale>(new Gtk::HScale(0,6,1));
        sequence_scales[x]->set_value(parent->GetSequence(x));
        sequence_scales[x]->set_increments(1.0,1.0);
        //sequence_scales[x]->set_draw_value(0);  //<- hehe, here is a bug in gtkmm ^^
        sequence_scales[x]->set_value_pos(Gtk::POS_RIGHT); //<- temporary workaround
        sequence_scales[x]->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnSequenceChanged),x));
        sequence_scales[x]->show();
        box_of_sliders.pack_start(*sequence_scales[x]);
    }
    realize();

}