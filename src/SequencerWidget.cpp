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

#include "SequencerWidget.h"
#include "messages.h"
#include "Event.h"
#include "Configuration.h"
#include "Files.h"
#include "MidiDriver.h"
#include "global.h"
#include "MainWindow.h"

SequencerWidget::SequencerWidget(){
    *dbg << "constructing new SEQUENCERWIDGET\n";

    AnythingSelected = 0;
    previous_box_where_pattern_lines_were_packed = -1;
    do_not_react_on_page_changes = 0;
    wMainTable.resize(3,3);
    wMainTable.attach(wUpperLeftBox,0,1,0,1);
    wMainTable.attach(wVSep,1,2,0,1);
    wVSep.set_size_request(2,0);
    wMainTable.attach(wUpperVBox,2,3,0,1);
    wMainTable.attach(wHSep,0,3,1,2);
    wMainTable.attach(wBoxOfChord,0,1,2,3);
    wMainTable.attach(wNotebookAndPatternOpsHBox,2,3,2,3);

    wNotebookAndPatternOpsHBox.pack_start(wNotebook,Gtk::PACK_SHRINK);
    wNotebookAndPatternOpsHBox.pack_start(wPtOpsVBox,Gtk::PACK_SHRINK);

    wUpperVBox.pack_start(wUpperHBox1,Gtk::PACK_SHRINK);
    wUpperVBox.pack_start(wUpperHBox2,Gtk::PACK_SHRINK);

    wUpperLeftBox.pack_start(wNameBox,Gtk::PACK_SHRINK);
    wUpperLeftBox.pack_start(wOnBox,Gtk::PACK_SHRINK);

    wNameBox.pack_start(wNameLabel,Gtk::PACK_SHRINK);
    wNameBox.pack_start(wNameEntry,Gtk::PACK_SHRINK);
    wNameEntry.set_width_chars(15);
    wNameEntry.signal_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnNameEdited));
    wOnBox.pack_start(wMuteToggle,Gtk::PACK_SHRINK);
    wOnBox.pack_start(wPlayOnceButton,Gtk::PACK_SHRINK);

    wUpperHBox1.pack_start(wChannelLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelButton,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wVolumeLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wVolumeButton,Gtk::PACK_SHRINK);

    wUpperHBox2.pack_start(wResolutionsLabel,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wResolutionsBox,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wLengthsLabel,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wLengthBox,Gtk::PACK_SHRINK);

    //wBoxOfChord.pack_start(*chordwidget);

    wNotebook.set_tab_pos(Gtk::POS_RIGHT);
    wNotebook.set_scrollable(1);
    wNotebook.signal_switch_page().connect(sigc::mem_fun(*this, &SequencerWidget::OnNotebookPageChanged));

    wPtOpsVBox.pack_start(wPtOpsHBox1,Gtk::PACK_SHRINK);
    wPtOpsVBox.pack_start(wSetAsActivePatternButton,Gtk::PACK_SHRINK);
    wPtOpsVBox.pack_start(wPtOpsHBox2,Gtk::PACK_SHRINK);

    wPtOpsHBox1.pack_start(wActivePanelLabel,Gtk::PACK_SHRINK);
    wPtOpsHBox1.pack_start(wActivePattern,Gtk::PACK_SHRINK);
    wPtOpsHBox2.pack_start(wAddPatternButton,Gtk::PACK_SHRINK);
    wPtOpsHBox2.pack_start(wRemovePattern,Gtk::PACK_SHRINK);

    wSetAsActivePatternButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnSetAsActivePatternClicked));
    wAddPatternButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnAddPatternClicked));
    wRemovePattern.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnRemovePatternClicked));

    wPlayOnceButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnPlayOnceButtonClicked));

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
    wNameLabel.set_text(_("Name:"));
    wPlayOnceButton.set_label(_("Play once"));
    wPlayOnceButton.set_tooltip_markup(_("Plays sequence in this sequencer <b>once</b>."));

    wVolumeButton.set_range(0,127);
    wChannelButton.set_range(1,16);
    wVolumeButton.set_increments(1,16);
    wChannelButton.set_increments(1,1);
    wVolumeButton.set_tooltip_markup(_("Sets the <b>velocity</b> of the notes played by this sequencer.\nUsually higher velocities result in louder sounds."));
    wChannelButton.set_tooltip_markup(_("Selects the <b>MIDI channel</b> this sequencer will output notes to. "));
    wVolumeButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnVolumeChanged));
    wChannelButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnChannelChanged));
    wActivePattern.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnActivePatternChanged));
    wMuteToggle.set_label(_("ON/OFF"));
    wMuteToggle.set_tooltip_markup(_("Turns this sequencer <b>on/off</b>."));
    wMuteToggle.signal_clicked().connect(mem_fun(*this,&SequencerWidget::OnToggleMuteToggled));

    //lengths selector
    wResolutionsLabel.set_text(_("Resolution:"));
    m_refTreeModel_res = Gtk::ListStore::create(m_Columns_resol);
    wResolutionsBox.set_model(m_refTreeModel_res);

    int resolutions[7] = RESOLUTIONS;
    for (int x = 0; x < RESOLUTIONS_NUM; x++){
        Gtk::TreeModel::Row row = *(m_refTreeModel_res->append());
        row[m_Columns_resol.resol] = resolutions[x];
    }
    wResolutionsBox.set_tooltip_markup(_("Selects the <b>resolution</b> of this sequencer. It defines how many <i>notes per one sequence</i> the sequencer has.\nIf the length is set to one, than the resolution will determine how many notes will be played in one bar."));
    wResolutionsBox.pack_start(m_Columns_resol.resol);
    wResolutionsBox.signal_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnResolutionChanged));


    wLengthsLabel.set_text(_("Length:"));
    m_refTreeModel_len = Gtk::ListStore::create(m_Columns_len);
    wLengthBox.set_model(m_refTreeModel_len);

    char temp[10];
    double lengths[7] = LENGTHS;
    for (int x = 0; x < LENGTHS_NUM; x++){
        Gtk::TreeModel::Row row = *(m_refTreeModel_len->append());
        row[m_Columns_len.len] = lengths[x];
        sprintf(temp,"%g",lengths[x]);
        row[m_Columns_len.text] = temp;
    }
    wLengthBox.set_tooltip_markup(_("Selects the <b>length</b> of this sequencer. It defines <i>how many bars</i> the sequence in this sequencer will last. In case it's smaller then 1, the sequence will be repeated few times in each bar."));
    wLengthBox.pack_start(m_Columns_len.text);
    wLengthBox.signal_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnLengthChanged));

    add(wMainTable);

    signal_key_press_event().connect(&FindAndProcessEventsKeyPress);

    show_all_children(1);
    hide(); //hide at start, but let the children be shown

    UpdateEverything();
    //chordwidget->UpdateWhatToShowAndWhatIsSensitive();
}

SequencerWidget::~SequencerWidget(){
    //delete chordwidget;
    for (int x = 0; x < pattern_lines.size();x++) delete pattern_lines[x];
    for (int x = 0; x < pattern_boxes.size();x++) delete pattern_boxes[x];
    for (int x = 0; x < note_separators.size();x++) delete note_separators[x];
}

void SequencerWidget::SelectSeq(seqHandle h){
    *dbg << "SeqencerWidget - selected" << h << "\n";
    AnythingSelected = 1;
    selectedSeq = h;
    UpdateEverything();
}

void SequencerWidget::SelectNothing(){
    AnythingSelected = 0;
    UpdateEverything();
}

void SequencerWidget::UpdateEverything(){
    *dbg << "SeqencerWidget - Updating everything\n";
    if (AnythingSelected){
        //We need to update all controls to the seqlectedSeq.
        Sequencer* seq = seqH(selectedSeq);

        show();
        wChannelButton.set_value(seq->GetChannel());
        wVolumeButton.set_value(seq->GetVolume());
        wMuteToggle.set_active(seq->GetOn());
        UpdateRelLenBoxes();
        UpdateName();
        InitNotebook();
        UpdateActivePatternRange();

    }else{
        //Nothing is selected them! The whole widget must look disabled and inactive!
        hide(); //temporary solution

    }
    
    
}

void SequencerWidget::UpdateRelLenBoxes(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);
    
        do_not_react_on_page_changes = 1;
        int resolutions[RESOLUTIONS_NUM] = RESOLUTIONS;
        char temp[10];
        for (int x = 0; x < RESOLUTIONS_NUM; x++){
            sprintf(temp,"%d",x);
            //Gtk::TreeModel::Row row = *(m_refTreeModel_res->get_iter(temp));
            if (resolutions[x] == (seq->resolution)){wResolutionsBox.set_active(x);continue;}
        }
        double lengths[7] = LENGTHS;
        for (int x = 0; x < LENGTHS_NUM; x++){
            sprintf(temp,"%d",x);
            //Gtk::TreeModel::Row row = *(m_refTreeModel_len->get_iter(temp));
            if(seq->length==lengths[x]) wLengthBox.set_active(x);
        }
        do_not_react_on_page_changes = 0;
}

void SequencerWidget::UpdateName(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);

    wNameEntry.set_text(seq->GetName());
}

void SequencerWidget::InitNotebook(){
    if(AnythingSelected == 0){
        *err<<_("ERROR - Cannot init SequencerWidget's notebook - no sequencer selected.\n");
        return;
    }
    Sequencer* seq = seqH(selectedSeq);

    char temp[100];
    do_not_react_on_page_changes = 1;

    *dbg << "SequencerWidget - INITING THE NOTEBOOK!!\n";

    DetachLines(); //to make it save to add/remove pages

    for (unsigned int x = 0; x < pattern_boxes.size();x++){
        if(!pattern_boxes[x]) continue;
        wNotebook.remove_page(*pattern_boxes[x]);
        if(pattern_boxes[x]) //maybe .remove_page removes also the object....
            delete pattern_boxes[x];
    }
    pattern_boxes.clear();

    pattern_boxes.resize(seq->patterns.size(),NULL);
    for (unsigned int x = 0; x < seq->patterns.size();x++){
        pattern_boxes[x] = /*Gtk::manage<Gtk::VBox>*/(new Gtk::HBox); //careful. may cause some strange memory leaks, should be investigated whether the Gtk::manage formule is really unneeded.
        pattern_boxes[x]->show();
        sprintf(temp,_("%d"),x);
        wNotebook.append_page(*pattern_boxes[x],temp);

    }
    do_not_react_on_page_changes = 0;

    //reset the current page
    wNotebook.set_current_page(0);
    AttachLines(0); //to bring the sliders back
    UpdateActivePatternRange();
    OnActivePatternChanged(); //this will mark active tab with a star (Pat x*)
    SetRemoveButtonSensitivity(); //according to the number of pages
}


void SequencerWidget::DetachLines(){
    *dbg << "Deattaching pattern-lines\n";
    if (previous_box_where_pattern_lines_were_packed == -1) return;   //because -1 that means there are not packed anywhere yet
     for(unsigned int x = 0; x < pattern_lines.size() ;x++){
        if(!pattern_lines[x]) continue;
        pattern_lines[x]->hide();
        *dbg << "removing " << x << " from " << previous_box_where_pattern_lines_were_packed << ENDL;
        pattern_boxes[previous_box_where_pattern_lines_were_packed]->remove(*pattern_lines[x]);
        delete pattern_lines[x];
    }
    for(unsigned int x = 0; x < note_separators.size();x++){
        if(!note_separators[x]) continue;
        note_separators[x]->hide();
        pattern_boxes[previous_box_where_pattern_lines_were_packed]->remove(*note_separators[x]);
        delete note_separators[x];
    }

    pattern_lines.clear();
    note_separators.clear();

}

void SequencerWidget::AttachLines(int where){
    Sequencer* seq = seqH(selectedSeq);

    char temp[50];
    *dbg << "Attaching pattern-lines to page " << where << ".\n";
    if(where >= pattern_boxes.size()) {*err<< "Cannot attach pattern-lines to box  "<<where<<", out of range.\n";return;}

     //assert(pattern_lines.size() == 0);
     pattern_lines.resize(seq->resolution,NULL);
     note_separators.resize(seq->resolution/4+1);

     int separator_counter = 0;

    for (int x= 0; x < seq->resolution; x++){
        if(x%4==0){
            if(x!=0){ //do note add a separator at the very beggining
                note_separators[separator_counter] = new Gtk::VSeparator;
                pattern_boxes[where]->pack_start(*note_separators[separator_counter]);
                note_separators[separator_counter]->show();
                separator_counter++;
            }
            sprintf(temp,"%d",x+1);
            pattern_lines[x] = (new PatternLine2(temp)); //cannot use Gtk::manage, since deleting the box would delete the lines!
        }else{
            pattern_lines[x] = (new PatternLine2); //cannot use Gtk::manage, since deleting the box would delete the lines!
        }

        for(int c = 0; c < 6; c++)
            pattern_lines[x]->SetButton(c,seq->GetPatternNote(where,x,c));

        pattern_lines[x]->OnButtonClicked.connect(sigc::bind(sigc::mem_fun(*this,&SequencerWidget::OnPatternNoteChanged),x));
        pattern_boxes[where]->pack_start(*pattern_lines[x],Gtk::PACK_SHRINK);
        pattern_lines[x]->show();
    }

    previous_box_where_pattern_lines_were_packed = where;

}

void SequencerWidget::UpdateActivePatternRange(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    
    int v = wActivePattern.get_value();
    wActivePattern.set_range(0.0,(double)seq->patterns.size()-1);
    wActivePattern.set_increments(1.0,1.0);
    wActivePattern.set_value(v); //if it's too high, it will change to largest possible
}

void SequencerWidget::OnPatternNoteChanged(int c, bool value, int seq){
    if(!AnythingSelected) return;
    Sequencer* sequ = seqH(selectedSeq);

    sequ->SetPatternNote(wNotebook.get_current_page(),seq,c,value);

    //Playing on edit...
    if(Config::Interaction::PlayOnEdit)
    if(value) midi->SendNoteEvent(sequ->GetChannel(),sequ->GetNoteOfChord(c),sequ->GetVolume(),PLAY_ON_EDIT_MS);

    Files::SetFileModified(1);
}

void SequencerWidget::OnChannelChanged(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    
    seq->SetChannel( wChannelButton.get_value());
    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}

void SequencerWidget::OnVolumeChanged(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    
    seq->SetVolume(wVolumeButton.get_value());
    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}
void SequencerWidget::OnChordWidgetChanged(){;}
void SequencerWidget::OnChordWidgetNoteChanged(int n, int p){;}
void SequencerWidget::OnToggleMuteToggled(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    seq->SetOn(wMuteToggle.get_active());
    seq->SetPlayOncePhase(0);
    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);

    //Files::SetFileModified(1); come on, do not write mutes.
}
void SequencerWidget::OnResolutionChanged(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    Gtk::TreeModel::Row row = *(wResolutionsBox.get_active());

    DetachLines();
    seq->SetResolution(row[m_Columns_resol.resol]);

    AttachLines(previous_box_where_pattern_lines_were_packed);

    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}
void SequencerWidget::OnLengthChanged(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    Gtk::TreeModel::Row row = *(wLengthBox.get_active());
    seq->length = row[m_Columns_len.len];
    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);

    Files::SetFileModified(1);
}
void SequencerWidget::OnActivePatternChanged(){
    //changing notepad tab labels
    //changing active seq in parent, but only if in range!!!!
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    char temp[100];
    int activepattern = wActivePattern.get_value();
    int old = seq->active_pattern;

    assert(activepattern < seq->patterns.size());

    sprintf(temp,_(" %d"),old);
    wNotebook.set_tab_label_text(*pattern_boxes[old],temp);

    seq->active_pattern = activepattern; //applying to parent

    sprintf(temp,_("%d*"),activepattern);
    wNotebook.set_tab_label_text(*pattern_boxes[activepattern],temp);

    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}
void SequencerWidget::OnSetAsActivePatternClicked(){
    int current = wNotebook.get_current_page();
    wActivePattern.set_value((double)current);
}
void SequencerWidget::OnNotebookPageChanged(GtkNotebookPage* page, guint page_num){
    if(do_not_react_on_page_changes) return;
    *dbg << "page changed!\n";
    DetachLines();
    AttachLines(wNotebook.get_current_page());
}
void SequencerWidget::OnAddPatternClicked(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    char temp[100];

    seq->AddPattern();

    pattern_boxes.push_back(NULL);
    int x = pattern_boxes.size() - 1;
    pattern_boxes[x] = new Gtk::HBox;
    pattern_boxes[x]->show();
    sprintf(temp, _("%d"), x);
    wNotebook.append_page(*pattern_boxes[x], temp);
    wNotebook.set_current_page(wNotebook.get_n_pages()-1); //will show the last page AND THE SIGNAL HANDLER WILL ATTACH THE SLIDERS!
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}
void SequencerWidget::OnRemovePatternClicked(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    int n = wNotebook.get_current_page();
    *dbg << "removing pattern " << n <<"\n";
    DetachLines();
    wNotebook.remove(*pattern_boxes[n]);
    delete pattern_boxes[n];
    pattern_boxes.erase(pattern_boxes.begin()+n);
    seq->patterns.erase(seq->patterns.begin()+n);
    if (seq->active_pattern == n ) { seq->active_pattern = 0;wActivePattern.set_value(0.0);}
    if (seq->active_pattern > n ) {seq->active_pattern = seq->active_pattern-1;wActivePattern.set_value(seq->active_pattern); }
    previous_box_where_pattern_lines_were_packed = -1;
    InitNotebook();
    wNotebook.set_current_page(n);
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}
void SequencerWidget::SetRemoveButtonSensitivity(){
    if(wNotebook.get_n_pages() == 1){
        wRemovePattern.set_sensitive(0);
    }else{
        wRemovePattern.set_sensitive(1);
    }
}
void SequencerWidget::OnNameEdited(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    
    seq->SetName(wNameEntry.get_text());
    if(seq->my_row) mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);

}
void SequencerWidget::OnPlayOnceButtonClicked(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    seq->SetPlayOncePhase(1);
}

//====================PATTERNLINE=========================
PatternLine2::PatternLine2(){
    set_border_width(0);
    pack_end(marker);
    for (int x = 0; x < 6; x++){
        buttons.push_back(new Gtk::CheckButton);
        pack_end(*buttons[x],Gtk::PACK_EXPAND_PADDING); //check the pack flag
        buttons[x]->signal_toggled().connect(sigc::bind<int>(sigc::mem_fun(*this,&PatternLine2::OnButtonsToggled),x));
        buttons[x]->set_border_width(0);
        buttons[x]->show();
    }
    marker.set_text(" ");
    marker.show();

}

PatternLine2::PatternLine2(Glib::ustring mark){
    set_border_width(0);
    pack_end(marker);
    for (int x = 0; x < 6; x++){
        buttons.push_back(new Gtk::CheckButton);
        pack_end(*buttons[x],Gtk::PACK_EXPAND_PADDING); //check the pack flag
        buttons[x]->signal_toggled().connect(sigc::bind<int>(sigc::mem_fun(*this,&PatternLine2::OnButtonsToggled),x));
        buttons[x]->set_border_width(0);
        buttons[x]->show();
    }
    marker.set_text(mark);
    marker.show();

}

PatternLine2::~PatternLine2(){
    for (int x = 0; x < 6; x++){
        remove(*buttons[x]);
        delete buttons[x];
    }

}

void PatternLine2::SetButton(int c, bool value){
    buttons[c]->set_active(value);
}

bool PatternLine2::GetButton(int c){
    return buttons[c]->get_active();
}

void PatternLine2::OnButtonsToggled(int c){

    OnButtonClicked.emit(c,buttons[c]->get_active());
}