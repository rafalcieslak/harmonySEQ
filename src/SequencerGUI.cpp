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
    main_vbox.pack_start(melody_ops_hbox);
    main_vbox.pack_start(line_zero, Gtk::PACK_SHRINK);
    main_vbox.pack_start(low_hbox);

    notebook.set_tab_pos(Gtk::POS_BOTTOM);
    UpdateActiveMelodyRange();
    active_melody.set_value(parent->active_melody); //maybe the parent already have an active melody chosen
    InitNotebook();
    notebook.set_scrollable(1);
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

    melody_ops_hbox.pack_end(remove_melody,Gtk::PACK_SHRINK);
    melody_ops_hbox.pack_end(add_melody_button,Gtk::PACK_SHRINK);
    melody_ops_hbox.pack_end(melodylabel,Gtk::PACK_SHRINK);
    add_melody_button.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWindow::OnAddMelodyClicked));
    remove_melody.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWindow::OnRemoveMelodyClicked));

    channellabel.set_text(_("MIDI channel:"));
    volumelabel.set_text(_("Volume:"));
    activemelodylabel.set_text(_("Active melody:"));
    set_as_active_melody.set_label(_("Set as active melody"));
    melodylabel.set_text(_("Melody:"));
    add_melody_button.set_label(_("Add"));
    remove_melody.set_label(_("Remove"));

    volume_button.set_range(0,127);
    channel_button.set_range(1,16);
    volume_button.set_increments(1,16);
    channel_button.set_increments(1,1);
    volume_button.set_value(parent->GetVolume());
    volume_button.property_value() = (double)parent->GetVolume();
    channel_button.set_value(parent->GetChannel());
    volume_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnVolumeChanged));
    channel_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
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
    parent->SetMelodyNote(notebook.get_current_page(),seq,melody_scales[seq]->get_value());
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

    DetachSliders();
    parent->SetResolution(row[m_Columns_resol.resol]);
    //if(!do_not_react_on_page_changes)
    AttachSliders(previous_box_where_sliders_were_packed);
    
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}


void SequencerWindow::OnLengthChanged(){
    Gtk::TreeModel::Row row = *(length_box.get_active());
    parent->length = row[m_Columns_len.len];
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
        sprintf(temp,_("%d"),x);
        notebook.append_page(*melody_boxes[x],temp);
        
    }
    resize(2,2);
    do_not_react_on_page_changes = 0;

    //reset the current page
    notebook.set_current_page(0);
    AttachSliders(0); //to bring the sliders back
    UpdateActiveMelodyRange();
    OnActiveMelodyChanged(); //this will mark active tab with a star (Mel x*)
    SetRemoveButtonSensitivity(); //according to the number of pages
}

void SequencerWindow::DetachSliders(){

    *dbg << "Deattaching sliders\n";
    if (previous_box_where_sliders_were_packed == -1) return;   //because -1 that means there are not packed anywhere yet
     for(unsigned int x = 0; x < melody_scales.size() ;x++){
        if(!melody_scales[x]) continue;
        melody_scales[x]->hide();
        *dbg << "removing " << x << " from " << previous_box_where_sliders_were_packed << ENDL;
        melody_boxes[previous_box_where_sliders_were_packed]->remove(*melody_scales[x]);
        delete melody_scales[x];
    }
    
    melody_scales.clear();

}

void SequencerWindow::AttachSliders(int where){
    *dbg << "Attaching sliders to page " << where << ".\n";
    if(where >= melody_boxes.size()) {*err<< "Cannot attach sliders to box  "<<where<<", out of range.\n";return;}

     melody_scales.resize(parent->resolution,NULL);

    for (int x= 0; x < parent->resolution; x++){
        melody_scales[x] = (new Gtk::HScale(0,6,1)); //cannot use Gtk::manage, since deleting the box would delete the sliders!
        melody_scales[x]->set_value(parent->GetMelodyNote(where,x));
        melody_scales[x]->set_increments(1.0,1.0);
        //sequence_scales[x]->set_draw_value(0);  //<- hehe, here is a bug in gtkmm ^^
        melody_scales[x]->set_value_pos(Gtk::POS_RIGHT); //<- temporary workaround
        melody_scales[x]->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnMelodyNoteChanged),x));
        //*dbg << "putting slider " << x << " to page " << active_page << ".\n";
        melody_boxes[where]->pack_start(*melody_scales[x],Gtk::PACK_SHRINK);
        melody_scales[x]->show();
    }

    previous_box_where_sliders_were_packed = where;

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

    sprintf(temp,_(" %d"),old);
    notebook.set_tab_label_text(*melody_boxes[old],temp);

    parent->active_melody = activemelody; //applying to parent

    sprintf(temp,_("%d*"),activemelody);
    notebook.set_tab_label_text(*melody_boxes[activemelody],temp);

    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::OnSetAsActiveMelodyClicked(){

    int current = notebook.get_current_page();
    active_melody.set_value((double)current);

}

void SequencerWindow::OnNotebookPageChanged(GtkNotebookPage* page, guint page_num){
    if(do_not_react_on_page_changes) return;
    *dbg << "page changed!\n";
    DetachSliders();
    AttachSliders(notebook.get_current_page());

}

void SequencerWindow::OnAddMelodyClicked(){
    char temp[100];

    parent->melodies.push_back( vector<int>(parent->resolution,0));

    melody_boxes.push_back(NULL);
    int x = melody_boxes.size() - 1;
    melody_boxes[x] = new Gtk::VBox;
    melody_boxes[x]->show();
    sprintf(temp, _("%d"), x);
    notebook.append_page(*melody_boxes[x], temp);
    notebook.set_current_page(notebook.get_n_pages()-1); //will show the last page AND THE SIGNAL HANDLER WILL ATTACH THE SLIDERS!
    UpdateActiveMelodyRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWindow::OnRemoveMelodyClicked(){
    int n = notebook.get_current_page();
    *dbg << "removing melody " << n <<"\n";
    DetachSliders();
    notebook.remove(*melody_boxes[n]);
    delete melody_boxes[n];
    melody_boxes.erase(melody_boxes.begin()+n);
    parent->melodies.erase(parent->melodies.begin()+n);
    if (parent->active_melody == n ) { parent->active_melody = 0;active_melody.set_value(0.0);}
    if (parent->active_melody > n ) {parent->active_melody = parent->active_melody-1;active_melody.set_value(parent->active_melody); }
    previous_box_where_sliders_were_packed = -1;
    InitNotebook();
    notebook.set_current_page(n);
    UpdateActiveMelodyRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWindow::SetRemoveButtonSensitivity(){
    if(notebook.get_n_pages() == 1){
        remove_melody.set_sensitive(0);
    }else{
        remove_melody.set_sensitive(1);
    }

}

void SequencerWindow::UpdateMelody(){

    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    char temp[100];
    int activemelody = parent->active_melody;
    int old = active_melody.get_value();

    assert(activemelody < parent->melodies.size());

    sprintf(temp,_(" %d"),old);
    notebook.set_tab_label_text(*melody_boxes[old],temp);

    active_melody.set_value(activemelody);

    sprintf(temp,_("%d*"),activemelody);
    notebook.set_tab_label_text(*melody_boxes[activemelody],temp);

    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    
}