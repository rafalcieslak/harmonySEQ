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
    previous_box_where_pattern_lines_were_packed = -1;
    do_not_react_on_page_changes = 0;

    set_title(parent->name);
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);
    
    main_vbox.pack_start(upper_box);
    main_vbox.pack_start(box_of_chord);
    box_of_chord.pack_start(*chordwidget);
    main_vbox.pack_start(notebook);
    main_vbox.pack_start(pattern_ops_hbox);
    main_vbox.pack_start(line_zero, Gtk::PACK_SHRINK);
    main_vbox.pack_start(low_hbox);

    notebook.set_tab_pos(Gtk::POS_BOTTOM);
    UpdateActivePatternRange();
    active_pattern.set_value(parent->active_pattern); //maybe the parent already have an active pattern chosen
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
    line_zero.pack_end(active_pattern,Gtk::PACK_SHRINK);
    line_zero.pack_end(activepanellabel,Gtk::PACK_SHRINK);
    line_zero.pack_end(set_as_active_pattern,Gtk::PACK_SHRINK);
    set_as_active_pattern.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnSetAsActivePatternClicked));

    pattern_ops_hbox.pack_end(remove_pattern,Gtk::PACK_SHRINK);
    pattern_ops_hbox.pack_end(add_pattern_button,Gtk::PACK_SHRINK);
    pattern_ops_hbox.pack_end(patternlabel,Gtk::PACK_SHRINK);
    add_pattern_button.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWindow::OnAddPatternClicked));
    remove_pattern.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWindow::OnRemovePatternClicked));

    channellabel.set_text(_("MIDI channel:"));
    volumelabel.set_text(_("Volume:"));
    activepanellabel.set_text(_("Active pattern:"));
    set_as_active_pattern.set_label(_("Set as active pattern"));
    patternlabel.set_text(_("Pattern:"));
    add_pattern_button.set_label(_("Add"));
    remove_pattern.set_label(_("Remove"));

    volume_button.set_range(0,127);
    channel_button.set_range(1,16);
    volume_button.set_increments(1,16);
    channel_button.set_increments(1,1);
    volume_button.set_value(parent->GetVolume());
    volume_button.property_value() = (double)parent->GetVolume();
    channel_button.set_value(parent->GetChannel());
    volume_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnVolumeChanged));
    channel_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
    active_pattern.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnActivePatternChanged));
    toggle_vbox.pack_start(tgl_mute);
    tgl_mute.set_label(_("On"));
    tgl_mute.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnToggleMuteToggled));
    tgl_mute.set_active(parent->on);

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
    for (int x = 0; x < pattern_lines.size();x++) delete pattern_lines[x];
}

void SequencerWindow::OnPatternNoteChanged(int c, bool value, int seq){
    
    parent->SetPatternNote(notebook.get_current_page(),seq,c,value);
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

    active_pattern.set_value(parent->active_pattern); //this doesn't trigger the signal handler
    OnActivePatternChanged();

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

void SequencerWindow::OnVolumeChanged(){
    parent->volume = volume_button.get_value();
    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::OnResolutionChanged(){
    Gtk::TreeModel::Row row = *(resolution_box.get_active());

    DetachLines();
    parent->SetResolution(row[m_Columns_resol.resol]);

    AttachLines(previous_box_where_pattern_lines_were_packed);
    resize(2,2);

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

    DetachLines(); //to make it save to add/remove pages

    for (unsigned int x = 0; x < pattern_boxes.size();x++){
        if(!pattern_boxes[x]) continue;
        notebook.remove_page(*pattern_boxes[x]);
        if(pattern_boxes[x]) //maybe .remove_page removes also the object....
            delete pattern_boxes[x];
    }
    pattern_boxes.clear();
    
    pattern_boxes.resize(parent->patterns.size(),NULL);
    for (unsigned int x = 0; x < parent->patterns.size();x++){
        pattern_boxes[x] = /*Gtk::manage<Gtk::VBox>*/(new Gtk::VBox); //careful. may cause some strange memory leaks, should be investigated whether the Gtk::manage formule is really unneeded.
        pattern_boxes[x]->show();
        sprintf(temp,_("%d"),x);
        notebook.append_page(*pattern_boxes[x],temp);
        
    }
    resize(2,2);
    do_not_react_on_page_changes = 0;

    //reset the current page
    notebook.set_current_page(0);
    AttachLines(0); //to bring the sliders back
    UpdateActivePatternRange();
    OnActivePatternChanged(); //this will mark active tab with a star (Pat x*)
    SetRemoveButtonSensitivity(); //according to the number of pages
}

void SequencerWindow::DetachLines(){

    *dbg << "Deattaching pattern-lines\n";
    if (previous_box_where_pattern_lines_were_packed == -1) return;   //because -1 that means there are not packed anywhere yet
     for(unsigned int x = 0; x < pattern_lines.size() ;x++){
        if(!pattern_lines[x]) continue;
        pattern_lines[x]->hide();
        *dbg << "removing " << x << " from " << previous_box_where_pattern_lines_were_packed << ENDL;
        pattern_boxes[previous_box_where_pattern_lines_were_packed]->remove(*pattern_lines[x]);
        delete pattern_lines[x];
    }
    
    pattern_lines.clear();

}

void SequencerWindow::AttachLines(int where){
    *dbg << "Attaching pattern-lines to page " << where << ".\n";
    if(where >= pattern_boxes.size()) {*err<< "Cannot attach pattern-lines to box  "<<where<<", out of range.\n";return;}

     //assert(pattern_lines.size() == 0);
     pattern_lines.resize(parent->resolution,NULL);

    for (int x= 0; x < parent->resolution; x++){
        pattern_lines[x] = (new PatternLine); //cannot use Gtk::manage, since deleting the box would delete the lines!
        for(int c = 0; c < 6; c++)
            pattern_lines[x]->SetButton(c,parent->GetPatternNote(where,x,c));
                    
        pattern_lines[x]->OnButtonClicked.connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnPatternNoteChanged),x));
        pattern_boxes[where]->pack_start(*pattern_lines[x],Gtk::PACK_SHRINK);
        pattern_lines[x]->show();
    }

    previous_box_where_pattern_lines_were_packed = where;

}


void SequencerWindow::OnChordWidgetChanged(){
    Files::SetFileModified(1);
}

void SequencerWindow::UpdateActivePatternRange(){
    int v = active_pattern.get_value();
    active_pattern.set_range(0.0,(double)parent->patterns.size()-1);
    active_pattern.set_increments(1.0,1.0);
    active_pattern.set_value(v); //if it's too high, it will change to largest possible
}

void SequencerWindow::OnActivePatternChanged(){
    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    char temp[100];
    int activepattern = active_pattern.get_value();
    int old = parent->active_pattern;

    assert(activepattern < parent->patterns.size());

    sprintf(temp,_(" %d"),old);
    notebook.set_tab_label_text(*pattern_boxes[old],temp);

    parent->active_pattern = activepattern; //applying to parent

    sprintf(temp,_("%d*"),activepattern);
    notebook.set_tab_label_text(*pattern_boxes[activepattern],temp);

    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    Files::SetFileModified(1);
}

void SequencerWindow::OnSetAsActivePatternClicked(){

    int current = notebook.get_current_page();
    active_pattern.set_value((double)current);

}

void SequencerWindow::OnNotebookPageChanged(GtkNotebookPage* page, guint page_num){
    if(do_not_react_on_page_changes) return;
    *dbg << "page changed!\n";
    DetachLines();
    AttachLines(notebook.get_current_page());

}

void SequencerWindow::OnAddPatternClicked(){
    char temp[100];

    parent->AddPattern();
    
    pattern_boxes.push_back(NULL);
    int x = pattern_boxes.size() - 1;
    pattern_boxes[x] = new Gtk::VBox;
    pattern_boxes[x]->show();
    sprintf(temp, _("%d"), x);
    notebook.append_page(*pattern_boxes[x], temp);
    notebook.set_current_page(notebook.get_n_pages()-1); //will show the last page AND THE SIGNAL HANDLER WILL ATTACH THE SLIDERS!
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWindow::OnRemovePatternClicked(){
    int n = notebook.get_current_page();
    *dbg << "removing pattern " << n <<"\n";
    DetachLines();
    notebook.remove(*pattern_boxes[n]);
    delete pattern_boxes[n];
    pattern_boxes.erase(pattern_boxes.begin()+n);
    parent->patterns.erase(parent->patterns.begin()+n);
    if (parent->active_pattern == n ) { parent->active_pattern = 0;active_pattern.set_value(0.0);}
    if (parent->active_pattern > n ) {parent->active_pattern = parent->active_pattern-1;active_pattern.set_value(parent->active_pattern); }
    previous_box_where_pattern_lines_were_packed = -1;
    InitNotebook();
    notebook.set_current_page(n);
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWindow::SetRemoveButtonSensitivity(){
    if(notebook.get_n_pages() == 1){
        remove_pattern.set_sensitive(0);
    }else{
        remove_pattern.set_sensitive(1);
    }

}

void SequencerWindow::UpdatePattern(){

    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    char temp[100];
    int activepattern = parent->active_pattern;
    int old = active_pattern.get_value();

    assert(activepattern < parent->patterns.size());

    sprintf(temp,_(" %d"),old);
    notebook.set_tab_label_text(*pattern_boxes[old],temp);

    active_pattern.set_value(activepattern);

    sprintf(temp,_("%d*"),activepattern);
    notebook.set_tab_label_text(*pattern_boxes[activepattern],temp);

    if(parent->row_in_main_window) mainwindow->RefreshRow(parent->row_in_main_window);
    
}
//====================PATTERNLINE=========================
PatternLine::PatternLine(){
    set_border_width(0);
    for (int x = 0; x < 6; x++){
        buttons.push_back(new Gtk::CheckButton);
        pack_start(*buttons[x],Gtk::PACK_EXPAND_PADDING); //check the pack flag
        buttons[x]->signal_toggled().connect(sigc::bind<int>(sigc::mem_fun(*this,&PatternLine::OnButtonsToggled),x));
        buttons[x]->set_border_width(0);
        buttons[x]->show();
    }

}

PatternLine::~PatternLine(){
    for (int x = 0; x < 6; x++){
        remove(*buttons[x]);
        delete buttons[x];
    }
    
}

void PatternLine::SetButton(int c, bool value){
    buttons[c]->set_active(value);
}

bool PatternLine::GetButton(int c){
    return buttons[c]->get_active();
}

void PatternLine::OnButtonsToggled(int c){

    OnButtonClicked.emit(c,buttons[c]->get_active());
}
