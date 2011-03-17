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


#include <assert.h>

#include "global.h"
#include "SequencerGUI.h"
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "Event.h"
#include "ChordWidget.h"
#include "Files.h"
#include "MidiDriver.h"
#include "Configuration.h"
SequencerWindow::SequencerWindow(Sequencer* prt)
{
    *dbg << "constructing new SequencerWindow\n";

    parent = prt;
    chordwidget = new ChordWidget(&prt->chord);
    chordwidget->on_changed.connect(mem_fun(*this,&SequencerWindow::OnChordWidgetChanged));
    previous_box_where_pattern_lines_were_packed = -1;
    do_not_react_on_page_changes = 0;

    set_title(parent->name);
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);
    
    wMainVbox.pack_start(wUpperBox);
    wMainVbox.pack_start(wBoxOfChord);
    wBoxOfChord.pack_start(*chordwidget);
    wMainVbox.pack_start(wNotebook);
    wMainVbox.pack_start(wPatternOpsHbox);
    wMainVbox.pack_start(wLineZero, Gtk::PACK_SHRINK);
    wMainVbox.pack_start(wLowerHBox);

    wNotebook.set_tab_pos(Gtk::POS_BOTTOM);
    UpdateActivePatternRange();
    wActivePattern.set_value(parent->active_pattern); //maybe the parent already have an active pattern chosen
    InitNotebook();
    wNotebook.set_scrollable(1);
    wNotebook.signal_switch_page().connect(sigc::mem_fun(*this, &SequencerWindow::OnNotebookPageChanged));

    wLowerHBox.pack_start(wSpinnersVBox,Gtk::PACK_SHRINK);
    wLowerHBox.pack_start(wToggleVBox,Gtk::PACK_SHRINK);
    wSpinnersVBox.pack_start(wLineOne, Gtk::PACK_SHRINK);
    wSpinnersVBox.pack_start(wLineTwo, Gtk::PACK_SHRINK);
    wLineOne.pack_end(wChannelButton,Gtk::PACK_SHRINK);
    wLineOne.pack_end(wChannelLabel,Gtk::PACK_SHRINK);
    wLineTwo.pack_end(wVolumeButton,Gtk::PACK_SHRINK);
    wLineTwo.pack_end(wVolumeLabel,Gtk::PACK_SHRINK);
    wLineZero.pack_end(wActivePattern,Gtk::PACK_SHRINK);
    wLineZero.pack_end(wActivePanelLabel,Gtk::PACK_SHRINK);
    wLineZero.pack_end(wSetAsActivePatternButton,Gtk::PACK_SHRINK);
    wSetAsActivePatternButton.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnSetAsActivePatternClicked));

    wPatternOpsHbox.pack_end(wRemovePattern,Gtk::PACK_SHRINK);
    wPatternOpsHbox.pack_end(wAddPatternButton,Gtk::PACK_SHRINK);
    wPatternOpsHbox.pack_end(wPatternLabel,Gtk::PACK_SHRINK);
    wAddPatternButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWindow::OnAddPatternClicked));
    wRemovePattern.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWindow::OnRemovePatternClicked));

    wChannelLabel.set_text(_("MIDI channel:"));
    wVolumeLabel.set_text(_("Volume:"));
    wActivePanelLabel.set_text(_("Active pattern:"));
    wSetAsActivePatternButton.set_label(_("Set as active pattern"));
    wSetAsActivePatternButton.set_tooltip_markup(_("Sets the chosen pattern to be the <b>active</b> one, which means the one that  will be played back."));
    wActivePattern.set_tooltip_markup(_("Selects which patter is <b>active</b>.\nActive pattern is the one that is played back. It's marked on a list with an asterisk (*).\n\n<i>This way all patterns can be edited while only the selected (the active) is played back. </i>"));
    wPatternLabel.set_text(_("Pattern:"));
    wAddPatternButton.set_label(_("Add"));
    wAddPatternButton.set_tooltip_markup(_("Adds a new pattern to this sequencer."));
    wRemovePattern.set_label(_("Remove"));
    wRemovePattern.set_tooltip_markup(_("Removes this pattern."));

    wVolumeButton.set_range(0,127);
    wChannelButton.set_range(1,16);
    wVolumeButton.set_increments(1,16);
    wChannelButton.set_increments(1,1);
    wVolumeButton.set_value(parent->GetVolume());
    wVolumeButton.set_tooltip_markup(_("Sets the <b>velocity</b> of the notes played by this sequencer.\nUsually higher velocities result in louder sounds."));
    wChannelButton.set_value(parent->GetChannel());
    wChannelButton.set_tooltip_markup(_("Selects the <b>MIDI channel</b> this sequencer will output notes to. "));
    wVolumeButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnVolumeChanged));
    wChannelButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
    wActivePattern.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnActivePatternChanged));
    wToggleVBox.pack_start(wMuteToggle);
    wMuteToggle.set_label(_("On"));
    wMuteToggle.set_tooltip_markup(_("Turns this sequencer <b>on/off</b>."));
    wMuteToggle.signal_clicked().connect(mem_fun(*this,&SequencerWindow::OnToggleMuteToggled));
    wMuteToggle.set_active(parent->on);

    //lengths selector
    wUpperBox.pack_start(wResolutionsLabel,Gtk::PACK_SHRINK);
    wResolutionsLabel.set_text(_("Resolution:"));
    wUpperBox.pack_start(wResolutionsBox,Gtk::PACK_SHRINK);
    m_refTreeModel_res = Gtk::ListStore::create(m_Columns_resol);
    wResolutionsBox.set_model(m_refTreeModel_res);


    int resolutions[7] = RESOLUTIONS;
    for (int x = 0; x < RESOLUTIONS_NUM; x++){
        Gtk::TreeModel::Row row = *(m_refTreeModel_res->append());
        row[m_Columns_resol.resol] = resolutions[x];
        if(parent->resolution==resolutions[x]) wResolutionsBox.set_active(x);
    }
    wResolutionsBox.set_tooltip_markup(_("Selects the <b>resolution</b> of this sequencer. It defines how many <i>notes per one sequence</i> the sequencer has.\nIf the length is set to one, than the resolution will determine how many notes will be played in one bar."));
    wResolutionsBox.pack_start(m_Columns_resol.resol);
    wResolutionsBox.signal_changed().connect(mem_fun(*this,&SequencerWindow::OnResolutionChanged));
    
    
    wUpperBox.pack_start(wLengthsLabel,Gtk::PACK_SHRINK);
    wLengthsLabel.set_text(_("Length:"));
    wUpperBox.pack_start(wLengthBox,Gtk::PACK_SHRINK);
    m_refTreeModel_len = Gtk::ListStore::create(m_Columns_len);
    wLengthBox.set_model(m_refTreeModel_len);

    char temp[10];
    double lengths[7] = LENGTHS;
    for (int x = 0; x < LENGTHS_NUM; x++){
        Gtk::TreeModel::Row row = *(m_refTreeModel_len->append());
        row[m_Columns_len.len] = lengths[x];
        sprintf(temp,"%g",lengths[x]);
        row[m_Columns_len.text] = temp;
        if(parent->length==lengths[x]) wLengthBox.set_active(x);
    }
    wLengthBox.set_tooltip_markup(_("Selects the <b>length</b> of this sequencer. It defines <i>how many bars</i> the sequence in this sequencer will last. In case it's smaller then 1, the sequence will be repeated few times in each bar."));
    wLengthBox.pack_start(m_Columns_len.text);
    wLengthBox.signal_changed().connect(mem_fun(*this,&SequencerWindow::OnLengthChanged));
    
    add(wMainVbox);

    signal_key_press_event().connect(&FindAndProcessEventsKeyPress);
    
    show_all_children(1);
    hide(); //hide at start, but let the children be shown
}
SequencerWindow::~SequencerWindow(){
    delete chordwidget;
    for (int x = 0; x < pattern_lines.size();x++) delete pattern_lines[x];
}

void SequencerWindow::OnPatternNoteChanged(int c, bool value, int seq){
    
    parent->SetPatternNote(wNotebook.get_current_page(),seq,c,value);

    //Playing on edit...
    if(Config::Interaction::PlayOnEdit)
    if(value) midi->SendNoteEvent(parent->GetChannel(),parent->GetNoteOfChord(c),parent->GetVolume(),PLAY_ON_EDIT_MS);

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
        if (resolutions[x] == (parent->resolution)){wResolutionsBox.set_active(x);continue;}
    }

    double lengths[7] = LENGTHS;
    for (int x = 0; x < LENGTHS_NUM; x++){
        sprintf(temp,"%d",x);
        //Gtk::TreeModel::Row row = *(m_refTreeModel_len->get_iter(temp));
        if(parent->length==lengths[x]) wLengthBox.set_active(x);
    }

    do_not_react_on_page_changes = 0;

    if (parent->active_pattern != wActivePattern.get_value()){
        wActivePattern.set_value(parent->active_pattern); //this doesn't trigger the signal handler
        OnActivePatternChanged();
    }

    InitNotebook();
    UpdateChord();
}


    void SequencerWindow::UpdateChord(){
        chordwidget->Update();
    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    }

void SequencerWindow::OnChannelChanged(){

    parent->channel = wChannelButton.get_value();
    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    Files::SetFileModified(1);
}

void SequencerWindow::OnToggleMuteToggled(){
    parent->on = wMuteToggle.get_active();
    parent->play_once_phase = 0;
    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);

    //Files::SetFileModified(1); come on, do not write mutes.
}

void SequencerWindow::OnVolumeChanged(){
    parent->volume = wVolumeButton.get_value();
    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    Files::SetFileModified(1);
}

void SequencerWindow::OnResolutionChanged(){
    Gtk::TreeModel::Row row = *(wResolutionsBox.get_active());

    DetachLines();
    parent->SetResolution(row[m_Columns_resol.resol]);

    AttachLines(previous_box_where_pattern_lines_were_packed);
    resize(2,2);

    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    Files::SetFileModified(1);
}


void SequencerWindow::OnLengthChanged(){
    Gtk::TreeModel::Row row = *(wLengthBox.get_active());
    parent->length = row[m_Columns_len.len];
    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);

    Files::SetFileModified(1);

}

void SequencerWindow::InitNotebook(){
    char temp[100];
    do_not_react_on_page_changes = 1;

    *dbg << "INITING THE NOTEBOOK!!\n";

    DetachLines(); //to make it save to add/remove pages

    for (unsigned int x = 0; x < pattern_boxes.size();x++){
        if(!pattern_boxes[x]) continue;
        wNotebook.remove_page(*pattern_boxes[x]);
        if(pattern_boxes[x]) //maybe .remove_page removes also the object....
            delete pattern_boxes[x];
    }
    pattern_boxes.clear();
    
    pattern_boxes.resize(parent->patterns.size(),NULL);
    for (unsigned int x = 0; x < parent->patterns.size();x++){
        pattern_boxes[x] = /*Gtk::manage<Gtk::VBox>*/(new Gtk::VBox); //careful. may cause some strange memory leaks, should be investigated whether the Gtk::manage formule is really unneeded.
        pattern_boxes[x]->show();
        sprintf(temp,_("%d"),x);
        wNotebook.append_page(*pattern_boxes[x],temp);
        
    }
    resize(2,2);
    do_not_react_on_page_changes = 0;

    //reset the current page
    wNotebook.set_current_page(0);
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
    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    Files::SetFileModified(1);
}

void SequencerWindow::UpdateActivePatternRange(){
    int v = wActivePattern.get_value();
    wActivePattern.set_range(0.0,(double)parent->patterns.size()-1);
    wActivePattern.set_increments(1.0,1.0);
    wActivePattern.set_value(v); //if it's too high, it will change to largest possible
}

void SequencerWindow::OnActivePatternChanged(){
    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    char temp[100];
    int activepattern = wActivePattern.get_value();
    int old = parent->active_pattern;

    assert(activepattern < parent->patterns.size());

    sprintf(temp,_(" %d"),old);
    wNotebook.set_tab_label_text(*pattern_boxes[old],temp);

    parent->active_pattern = activepattern; //applying to parent

    sprintf(temp,_("%d*"),activepattern);
    wNotebook.set_tab_label_text(*pattern_boxes[activepattern],temp);

    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    Files::SetFileModified(1);
}

void SequencerWindow::OnSetAsActivePatternClicked(){

    int current = wNotebook.get_current_page();
    wActivePattern.set_value((double)current);

}

void SequencerWindow::OnNotebookPageChanged(GtkNotebookPage* page, guint page_num){
    if(do_not_react_on_page_changes) return;
    *dbg << "page changed!\n";
    DetachLines();
    AttachLines(wNotebook.get_current_page());

}

void SequencerWindow::OnAddPatternClicked(){
    char temp[100];

    parent->AddPattern();
    
    pattern_boxes.push_back(NULL);
    int x = pattern_boxes.size() - 1;
    pattern_boxes[x] = new Gtk::VBox;
    pattern_boxes[x]->show();
    sprintf(temp, _("%d"), x);
    wNotebook.append_page(*pattern_boxes[x], temp);
    wNotebook.set_current_page(wNotebook.get_n_pages()-1); //will show the last page AND THE SIGNAL HANDLER WILL ATTACH THE SLIDERS!
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWindow::OnRemovePatternClicked(){
    int n = wNotebook.get_current_page();
    *dbg << "removing pattern " << n <<"\n";
    DetachLines();
    wNotebook.remove(*pattern_boxes[n]);
    delete pattern_boxes[n];
    pattern_boxes.erase(pattern_boxes.begin()+n);
    parent->patterns.erase(parent->patterns.begin()+n);
    if (parent->active_pattern == n ) { parent->active_pattern = 0;wActivePattern.set_value(0.0);}
    if (parent->active_pattern > n ) {parent->active_pattern = parent->active_pattern-1;wActivePattern.set_value(parent->active_pattern); }
    previous_box_where_pattern_lines_were_packed = -1;
    InitNotebook();
    wNotebook.set_current_page(n);
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWindow::SetRemoveButtonSensitivity(){
    if(wNotebook.get_n_pages() == 1){
        wRemovePattern.set_sensitive(0);
    }else{
        wRemovePattern.set_sensitive(1);
    }

}

void SequencerWindow::UpdatePattern(){

    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    char temp[100];
    int activepattern = parent->active_pattern;
    int old = wActivePattern.get_value();

    assert(activepattern < parent->patterns.size());

    sprintf(temp,_(" %d"),old);
    wNotebook.set_tab_label_text(*pattern_boxes[old],temp);

    wActivePattern.set_value(activepattern);

    sprintf(temp,_("%d*"),activepattern);
    wNotebook.set_tab_label_text(*pattern_boxes[activepattern],temp);

    if(parent->my_row) mainwindow->RefreshRow(parent->my_row);
    
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
