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


#include <assert.h>

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
    previous_box_where_sliders_were_packed = -1;
    do_not_react_on_page_changes = 0;

    set_title(parent->name);
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);
    
    main_vbox.pack_start(upper_box);
    main_vbox.pack_start(box_of_chord);
    box_of_chord.pack_start(*chordwidget);
    main_vbox.pack_start(notebook);
    main_vbox.pack_start(line_zero, Gtk::PACK_SHRINK);
    main_vbox.pack_start(low_hbox);

    notebook.set_tab_pos(Gtk::POS_BOTTOM);
    InitNotebook();
    notebook.signal_switch_page().connect(sigc::mem_fun(*this, &SequencerWindow::OnNotebookPageChanged));

    low_hbox.pack_start(spinners_vbox,Gtk::PACK_SHRINK);
    low_hbox.pack_start(toggle_vbox,Gtk::PACK_SHRINK);
    spinners_vbox.pack_start(line_one, Gtk::PACK_SHRINK);
    spinners_vbox.pack_start(line_two, Gtk::PACK_SHRINK);
    line_one.pack_end(channel_button,Gtk::PACK_SHRINK);
    line_one.pack_end(channellabel,Gtk::PACK_SHRINK);
    line_two.pack_end(volume_button,Gtk::PACK_SHRINK);
    line_two.pack_end(volumelabel,Gtk::PACK_SHRINK);
    line_zero.pack_end(active_melody,Gtk::PACK_SHRINK);
    line_zero.pack_end(activemelodylabel,Gtk::PACK_SHRINK);
    line_zero.pack_end(set_as_active_melody,Gtk::PACK_SHRINK);
    set_as_active_melody.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnSetAsActiveMelodyClicked));
    channellabel.set_text(_("MIDI channel:"));
    volumelabel.set_text(_("Volume:"));
    activemelodylabel.set_text(_("Active melody:"));
    set_as_active_melody.set_label(_("Set as active melody"));
    volume_button.set_range(0,127);
    channel_button.set_range(1,16);
    volume_button.set_increments(1,16);
    channel_button.set_increments(1,1);
    volume_button.set_value(parent->GetVolume());
    volume_button.property_value() = (double)parent->GetVolume();
    channel_button.set_value(parent->GetChannel());
    volume_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnVolumeChanged));
    channel_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
    UpdateActiveMelodyRange();
    OnActiveMelodyChanged();
    active_melody.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnActiveMelodyChanged));
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
    hide(); //hide at start, but let the children be shown
}
SequencerWindow::~SequencerWindow(){
    delete chordwidget;
    for (int x = 0; x < melody_scales.size();x++) delete melody_scales[x];
    for (int x = 0; x < melody_boxes.size();x++) delete melody_boxes[x];
}

void SequencerWindow::OnMelodyNoteChanged(int seq){
    *dbg << "seq changed";
    parent->SetMelodyNote(active_melody.get_value(),seq,melody_scales[seq]->get_value());
    Files::SetFileModified(1);
}

void SequencerWindow::UpdateValues(){
//TODO const
    //finding a num to a res should be const complexity!
    do_not_react_on_page_changes = 1;
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

    do_not_react_on_page_changes = 0;

    active_melody.set_value(parent->active_melody); //this doesn't trigger the signal handler
    OnActiveMelodyChanged();

    InitNotebook();
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
    if(!do_not_react_on_page_changes)
        InitMelodySliders();;
    
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}


void SequencerWindow::OnLengthChanged(){
    Gtk::TreeModel::Row row = *(length_box.get_active());
    *dbg << "setting length\n";
    parent->length = row[m_Columns_len.len];
    *dbg << "will refresh row\n";
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);

    Files::SetFileModified(1);

}

void SequencerWindow::InitNotebook(){
    char temp[100];
    do_not_react_on_page_changes = 1;

    *dbg << "INITING THE NOTEBOOK!!\n";

    DetachSliders(); //to make it save to add/remove pages

    for (unsigned int x = 0; x < melody_boxes.size();x++){
        if(!melody_boxes[x]) continue;
        notebook.remove_page(*melody_boxes[x]);
        if(melody_boxes[x]) //maybe .remove_page removes also the object....
            delete melody_boxes[x];
    }
    melody_boxes.clear();
    
    melody_boxes.resize(parent->melodies.size(),NULL);
    for (unsigned int x = 0; x < parent->melodies.size();x++){
        melody_boxes[x] = /*Gtk::manage<Gtk::VBox>*/(new Gtk::VBox); //careful. may cause some strange memory leaks, should be investigated whether the Gtk::manage formule is really unneeded.
        melody_boxes[x]->show();
        sprintf(temp,_("Mel %d"),x);
        notebook.append_page(*melody_boxes[x],temp);
        
    }
    resize(2,2);
    do_not_react_on_page_changes = 0;

    //reset the current page
    notebook.set_current_page(0);
    previous_box_where_sliders_were_packed = 0;
    ReattachSliders(); //to bring the sliders back
    UpdateActiveMelodyRange();
    OnActiveMelodyChanged(); //this will mark active tab with a star (Mel x*)
}

void SequencerWindow::InitMelodySliders(){
    *dbg << "sliders were on  " << previous_box_where_sliders_were_packed << " page. \n";
    int active_page = notebook.get_current_page();
    *dbg << "setting sliders to be on page " << active_page << ". \n";
    if(active_page == previous_box_where_sliders_were_packed) //so the current page didn't change, so:
        return;

    if (previous_box_where_sliders_were_packed != -1)   //because -1 that means there are not packed anywhere yet
    for(unsigned int x = 0; x < melody_scales.size();x++){
        if(!melody_scales[x]) continue;
        melody_scales[x]->hide();
        melody_boxes[previous_box_where_sliders_were_packed]->remove(*melody_scales[x]);
        delete melody_scales[x];
    }
    
    melody_scales.clear();

    melody_scales.resize(parent->resolution,NULL);

    for (int x= 0; x < parent->resolution; x++){
        melody_scales[x] = (new Gtk::HScale(0,6,1)); //cannot use Gtk::manage, since deleting the box would delete the sliders!
        melody_scales[x]->set_value(parent->GetMelodyNote(active_page,x));
        melody_scales[x]->set_increments(1.0,1.0);
        //sequence_scales[x]->set_draw_value(0);  //<- hehe, here is a bug in gtkmm ^^
        melody_scales[x]->set_value_pos(Gtk::POS_RIGHT); //<- temporary workaround
        melody_scales[x]->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnMelodyNoteChanged),x));
        //*dbg << "putting slider " << x << " to page " << active_page << ".\n";
        melody_boxes[active_page]->pack_start(*melody_scales[x],Gtk::PACK_SHRINK);
        melody_scales[x]->show();
    }
    previous_box_where_sliders_were_packed=active_page; //remember where sliders are packed, becuse this information is  nessesary to unpack them

}

void SequencerWindow::DetachSliders(){

    if (previous_box_where_sliders_were_packed == -1) return;   //because -1 that means there are not packed anywhere yet
     for(unsigned int x = 0; x < melody_scales.size();x++){
        if(!melody_scales[x]) continue;
        melody_scales[x]->hide();
        melody_boxes[previous_box_where_sliders_were_packed]->remove(*melody_scales[x]);
        delete melody_scales[x];
    }
    
    melody_scales.clear();

}

void SequencerWindow::ReattachSliders(){

    //sliders are reattached to previous_box_where_sliders_were_packed
    if (previous_box_where_sliders_were_packed == -1) ;   //because -1 that means there are not packed anywhere yet

    melody_scales.resize(parent->resolution,NULL);
    
    for (int x= 0; x < parent->resolution; x++){
        melody_scales[x] = (new Gtk::HScale(0,6,1)); //cannot use Gtk::manage, since deleting the box would delete the sliders!
        melody_scales[x]->set_value(parent->GetMelodyNote(previous_box_where_sliders_were_packed,x));
        melody_scales[x]->set_increments(1.0,1.0);
        //sequence_scales[x]->set_draw_value(0);  //<- hehe, here is a bug in gtkmm ^^
        melody_scales[x]->set_value_pos(Gtk::POS_RIGHT); //<- temporary workaround
        melody_scales[x]->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnMelodyNoteChanged),x));
        //*dbg << "putting slider " << x << " to page " << active_page << ".\n";
        melody_boxes[previous_box_where_sliders_were_packed]->pack_start(*melody_scales[x],Gtk::PACK_SHRINK);
        melody_scales[x]->show();
    }


}

void SequencerWindow::OnChordWidgetChanged(){
    Files::SetFileModified(1);
}

void SequencerWindow::UpdateActiveMelodyRange(){
    int v = active_melody.get_value();
    active_melody.set_range(0.0,(double)parent->melodies.size()-1);
    active_melody.set_increments(1.0,1.0);
    active_melody.set_value(v); //if it's too high, it will change to largest possible
}

void SequencerWindow::OnActiveMelodyChanged(){
    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    char temp[100];
    int activemelody = active_melody.get_value();
    int old = parent->active_melody;

    assert(activemelody < parent->melodies.size());

    sprintf(temp,_("Mel %d"),old);
    notebook.set_tab_label_text(*melody_boxes[old],temp);

    parent->active_melody = activemelody; //applying to parent

    sprintf(temp,_("Mel %d*"),activemelody);
    notebook.set_tab_label_text(*melody_boxes[activemelody],temp);
}

void SequencerWindow::OnSetAsActiveMelodyClicked(){

    int current = notebook.get_current_page();
    active_melody.set_value((double)current); //singnal handler wont be triggered, it reacts on user changes, so call it manually
    OnActiveMelodyChanged();

}

void SequencerWindow::OnNotebookPageChanged(GtkNotebookPage* page, guint page_num){
    if(do_not_react_on_page_changes) return;
    *dbg << "page changed!\n";
    InitMelodySliders();

}