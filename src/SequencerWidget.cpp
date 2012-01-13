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

#include "SequencerWidget.h"
#include "messages.h"
#include "Event.h"
#include "Configuration.h"
#include "Files.h"
#include "MidiDriver.h"
#include "global.h"
#include "MainWindow.h"
#include "NoteSequencer.h"
#include "ControlSequencer.h"
#include "ControllerAtom.h"

extern Glib::RefPtr< Gdk::Pixbuf > icon_slope_linear;
extern Glib::RefPtr< Gdk::Pixbuf > icon_slope_flat;

SequencerWidget::SequencerWidget()
{
    *dbg << "constructing new SEQUENCERWIDGET\n";

    AnythingSelected = 0;
    do_not_react_on_page_changes = 0;
    ignore_signals = 0;

    wMainVbox.pack_start(wUpBox,Gtk::PACK_SHRINK);
    wMainVbox.pack_start(wDownTable,Gtk::PACK_SHRINK);
    
    wDownTable.resize(4,4);
    wDownTable.attach(wUpperHBox1,0,2,0,1);
    wDownTable.attach(wShowChordButton,0,1,1,3,Gtk::FILL);
    wDownTable.attach(wUpperHBox2,1,2,1,2);
    wDownTable.attach(wRightBox,3,4,0,2,Gtk::SHRINK);
    wDownTable.attach(wRightBoxSep,2,3,0,2,Gtk::SHRINK);
    wDownTable.attach(wHSep,1,4,2,3);
    wDownTable.attach(wBoxOfChord,0,1,3,4,Gtk::SHRINK);
    wDownTable.attach(wNotebookAndPatternOpsHBox,1,4,3,4);
    wRightBoxSep.set_size_request(4,0);
    
    wNotebookAndPatternOpsHBox.pack_start(wNotebookVbox/*,Gtk::PACK_EXPAND_WIDGET*/);
    wNotebookAndPatternOpsHBox.pack_start(wNotebook,Gtk::PACK_SHRINK);
    wNotebookAndPatternOpsHBox.pack_end(wPtOpsVBox,Gtk::PACK_SHRINK);
    wViewport = new Gtk::Viewport(*wPatternScroll.get_adjustment(),*wPatternScroll2.get_adjustment());
    wNotebookVbox.pack_start(*wViewport);
    wNotebookVbox.pack_end(wPatternScroll,Gtk::PACK_SHRINK);
    wViewport->add(wPatternWidgetBox);
    wViewport->set_shadow_type(Gtk::SHADOW_NONE);
    wPatternWidgetBox.pack_start(pattern_widget,Gtk::PACK_SHRINK);
    
    pattern_widget.on_selection_changed.connect(sigc::mem_fun(*this,&SequencerWidget::OnSelectionChanged));
    pattern_widget.on_slope_type_needs_additional_refreshing.connect(sigc::mem_fun(*this,&SequencerWidget::UpdateSlopeType));
    
    wUpperHBox1.pack_start(wNameLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wNameEntry,Gtk::PACK_SHRINK);
    wNameEntry.set_width_chars(10);
    wNameEntry.signal_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnNameEdited));
    wUpperHBox1.pack_start(wOnOfColour,Gtk::PACK_SHRINK);
    wOnOfColour.add(wMuteToggle);
    wUpperHBox1.pack_start(wPlayOnceButton,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelSep,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelButton,Gtk::PACK_SHRINK);
    wChannelSep.set_size_request(8,0);
    wUpperHBox1.pack_start(wControllerLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wControllerButton,Gtk::PACK_SHRINK);

    wRightBox.pack_start(wRightBox1,Gtk::PACK_SHRINK);
    wRightBox.pack_start(wRightBox2,Gtk::PACK_SHRINK);
    wRightBox1.pack_start(wResolutionsLabel,Gtk::PACK_SHRINK);
    wRightBox1.pack_start(wResolutions,Gtk::PACK_SHRINK);
    wRightBox2.pack_start(wLengthsLabel,Gtk::PACK_SHRINK);
    wRightBox2.pack_start(wLengthNumerator,Gtk::PACK_SHRINK);
    wRightBox2.pack_start(wLengthDivision,Gtk::PACK_SHRINK);
    wRightBox2.pack_start(wLengthDenominator,Gtk::PACK_SHRINK);
    wRightBox2.pack_start(wLengthResult,Gtk::PACK_SHRINK);
    
    //wUpperHBox2.pack_start(wShowChordButton,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wSlopeSelectorSep,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wCtrlSlopeFlat,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wCtrlSlopeLinear,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wVelSep,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wVelocityLabel,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wVelocityButton,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wValueLabel,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wValueButton,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_start(wSnapToggle,Gtk::PACK_SHRINK);
    wVelSep.set_size_request(8,0);
    wSlopeSelectorSep.set_size_request(8,0);
    
    wShowChordButton.add(wShowChordLabel);
    wShowChordLabel.set_markup(_("<b>Chord</b>"));
    wShowChordButton.set_tooltip_markup(_("Shows/hides the chord's detailed settings."));
    wShowChordButton.signal_toggled().connect(sigc::mem_fun(*this,&SequencerWidget::OnShowChordButtonClicked));
    
    wSnapToggle.set_label(_("Snap to grid"));
    wSnapToggle.set_tooltip_markup(_("If on, notes will be snapped to the visible grid.\nGrid density changes with sequence resolution."));
    wSnapToggle.set_active(1); //As this is default value
    wSnapToggle.signal_toggled().connect(sigc::mem_fun(*this,&SequencerWidget::OnSnapClicked));

    wBoxOfChord.pack_start(wChordNotebook,Gtk::PACK_SHRINK);
    wBoxOfChord.pack_start(wVirtualSpaceLabel,Gtk::PACK_EXPAND_WIDGET,1); //extra alligments space - 1 stands for the notebook's border witdth
    wVirtualSpaceLabel.set_text(" ");
    wChordNotebook.append_page(chordwidget);
    wChordNotebook.append_page(wCtrlHBox);
    wChordNotebook.set_show_tabs(0);
    wChordNotebook.set_show_border(0);
    chordwidget.on_changed.connect(sigc::mem_fun(*this,&SequencerWidget::OnChordWidgetChanged));
    chordwidget.on_note_changed.connect(sigc::mem_fun(*this,&SequencerWidget::OnChordWidgetNoteChanged));

    wCtrlHBox.pack_end(wCtrlScale,Gtk::PACK_SHRINK);
    wCtrlScale.pack_start(wCtrl127,Gtk::PACK_SHRINK);
    wCtrlScale.pack_start(wCtrl64,Gtk::PACK_EXPAND_WIDGET);
    wCtrlScale.pack_start(wCtrl0,Gtk::PACK_SHRINK);
    wCtrl127.set_label("127");
    wCtrl64.set_label("64");
    wCtrl0.set_label("0");
    
    wNotebook.set_tab_pos(Gtk::POS_RIGHT);
    wNotebook.set_show_border(0);
    wNotebook.set_scrollable(1);
    wNotebook.signal_switch_page().connect(sigc::mem_fun(*this, &SequencerWidget::OnNotebookPageChanged));

    wPtOpsVBox.pack_end(wClearPatternHBox,Gtk::PACK_SHRINK);
    wPtOpsVBox.pack_start(wPtOpsHBox1,Gtk::PACK_SHRINK);
    wPtOpsVBox.pack_start(wSetAsActivePatternButton,Gtk::PACK_SHRINK);
    wPtOpsVBox.pack_start(wPtOpsHBox2,Gtk::PACK_SHRINK);

    wPtOpsHBox1.pack_start(wActivePanelLabel,Gtk::PACK_SHRINK);
    wPtOpsHBox1.pack_start(wActivePattern,Gtk::PACK_SHRINK);
    wPtOpsHBox2.pack_start(wAddPatternButton,Gtk::PACK_SHRINK);
    wPtOpsHBox2.pack_start(wClonePattern,Gtk::PACK_SHRINK);
    wPtOpsHBox2.pack_start(wRemovePattern,Gtk::PACK_SHRINK);

    wClearPatternHBox.pack_start(wClearPattern,Gtk::PACK_SHRINK);

    wClearPattern.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnClearPatternClicked));
    wClonePattern.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnClonePatternClicked));
    wSetAsActivePatternButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnSetAsActivePatternClicked));
    wAddPatternButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnAddPatternClicked));
    wRemovePattern.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnRemovePatternClicked));

    wPlayOnceButton.signal_clicked().connect(sigc::mem_fun(*this,&SequencerWidget::OnPlayOnceButtonClicked));

    wChannelLabel.set_text(_("MIDI channel:"));
    wVelocityLabel.set_text(_("Velocity:"));
    wValueLabel.set_text(_("Value:"));
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
    wClearPattern.set_label(_("Clear pattern"));
    wClearPattern.set_tooltip_markup(_("Clears all notes of this pattern."));
    wClonePattern.set_label(_("Clone"));
    wClonePattern.set_tooltip_markup(_("Clones this pattern, creating a new one."));
    /* TRANSLATORS: The space befor this string is to force a tiny space between widgets, please keep it in translations.*/
    wControllerLabel.set_text(_(" Controller No."));
    wControllerButton.set_tooltip_markup(_("The <b>MIDI controller number</b> this sequencer outputs data on.\n\nFor example, synthesizers supporting GM standard should interpret data from controller 7 as volume setting."));

    wVelocityButton.set_range(0,127);
    wValueButton.set_range(0,127);
    wChannelButton.set_range(1,16);
    wControllerButton.set_range(0.0,127.0);
    wVelocityButton.set_increments(1,16);
    wValueButton.set_increments(1,16);
    wChannelButton.set_increments(1,1);
    wControllerButton.set_increments(1.0,16.0);
    wVelocityButton.set_tooltip_markup(_("Sets the <b>velocity</b> of selected note(s).\nUsually higher velocities result in louder sounds."));
    wValueButton.set_tooltip_markup(_("Sets the <b>value</b> of selected point(s)."));
    wChannelButton.set_tooltip_markup(_("Selects the <b>MIDI channel</b> this sequencer will output notes to. "));
    wChannelButton.set_width_chars(2);
    wVelocityButton.set_width_chars(3);
    wValueButton.set_width_chars(3);
    wControllerButton.set_width_chars(3);
    wVelocityButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnVelocityChanged));
    wValueButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnValueChanged));
    wChannelButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnChannelChanged));
    wControllerButton.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnControllerChanged));
    wActivePattern.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnActivePatternChanged));
    wMuteToggle.set_label(_("ON/OFF"));
    wMuteToggle.set_tooltip_markup(_("Turns this sequencer <b>on/off</b>."));
    wMuteToggle.signal_clicked().connect(mem_fun(*this,&SequencerWidget::OnToggleMuteToggled));

    //todo: add icons & tooltips
    wCtrlSlopeFlat.set_label("F");
    wCtrlSlopeLinear.set_label("L");
    wCtrlSlopeFlat.set_mode(0);
    wCtrlSlopeLinear.set_mode(0);
    Gtk::RadioButton::Group group = wCtrlSlopeFlat.get_group();
    wCtrlSlopeLinear.set_group(group);
    wCtrlSlopeNone.set_group(group);
    wCtrlSlopeFlat.signal_toggled().connect(sigc::mem_fun(*this,&SequencerWidget::OnSlopeFlatToggled));
    wCtrlSlopeLinear.signal_toggled().connect(sigc::mem_fun(*this,&SequencerWidget::OnSlopeLinearToggled));
    //my_slope_mode_for_adding = SLOPE_TYPE_LINEAR;
    
    //lengths selector
    wResolutionsLabel.set_text(_("Resolution:"));
    wResolutions.set_range(1.0,64.0);
    wResolutions.set_increments(1.0,4.0);
    wResolutions.set_width_chars(2);
    wResolutions.set_tooltip_markup(_("Selects the <b>resolution</b> of this sequencer. It defines the grid density in this sequencer's patterns."));
    wResolutions.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnResolutionChanged));


    wLengthsLabel.set_text(_("Length:"));
    
    wLengthNumerator.set_tooltip_markup(_("Selects the <b>length</b> of this sequencer. It defines <i>how many bars</i> the sequence in this sequencer will last. In case it's smaller then 1, the sequence may be repeated few times in each bar.\n\nThis it the length's fraction numerator."));
    wLengthDenominator.set_tooltip_markup(_("Selects the <b>length</b> of this sequencer. It defines <i>how many bars</i> the sequence in this sequencer will last. In case it's smaller then 1, the sequence may be repeated few times in each bar.\n\nThis it the length's fraction denominator."));
    wLengthResult.set_tooltip_markup(_("Selects the <b>length</b> of this sequencer. It defines <i>how many bars</i> the sequence in this sequencer will last. In case it's smaller then 1, the sequence may be repeated few times in each bar."));
    wLengthNumerator.set_increments(1.0,1.0);
    wLengthDenominator.set_increments(1.0,1.0);
    wLengthNumerator.set_width_chars(3);
    wLengthDenominator.set_width_chars(3);
    wLengthNumerator.set_range(1.0,64.0);
    wLengthDenominator.set_range(1.0,32.0);
    wLengthDivision.set_text(" / ");
    wLengthNumerator.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnLengthChanged));
    wLengthDenominator.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWidget::OnLengthChanged));
    
    pattern_widget.on_scroll_left.connect(mem_fun(*this,&SequencerWidget::OnPatternWidgetScrollLeft));
    pattern_widget.on_scroll_right.connect(mem_fun(*this,&SequencerWidget::OnPatternWidgetScrollRight));
    
    if(icon_slope_flat){
        wImageSlopeFlat.set(icon_slope_flat);
        wImageSlopeLinear.set(icon_slope_linear);
        wCtrlSlopeFlat.set_image(wImageSlopeFlat);
        wCtrlSlopeLinear.set_image(wImageSlopeLinear);
        wCtrlSlopeFlat.set_label("");
        wCtrlSlopeFlat.set_tooltip_markup(_("Changes selected points mode to <b>flat</b>\nharmonySEQ will output a single MIDI Control Event on such points."));
        wCtrlSlopeLinear.set_label("");
        wCtrlSlopeLinear.set_tooltip_markup(_("Changes selected points mode to <b>linear</b>\nIt allows to create linear slopes, for smooth parameter changes."));
        wImageSlopeFlat.show();
        wImageSlopeLinear.show();
    }
    
    add(wMainVbox);
    
    show_all_children(1);
    hide(); //hide at start, but let the children be shown
}

SequencerWidget::~SequencerWidget(){
    do_not_react_on_page_changes = 1;
    for(int x = 0; x < (int) notebook_pages.size();x++) delete notebook_pages[x]; //TODO: check if they need to be removed from notebook first.
    delete wViewport;
}

void SequencerWidget::SelectSeq(seqHandle h){
    *dbg << "SeqencerWidget - selected " << h << "\n";
    AnythingSelected = 1;
    selectedSeq = h;
    selectedSeqType = seqH(h)->GetType();
    if(selectedSeqType == SEQ_TYPE_NOTE){
        NoteSequencer* noteseq = dynamic_cast<NoteSequencer*>(seqH(h));
        chordwidget.Select(&noteseq->chord);
    }
    UpdateEverything();
    DeacivateAllDiodes();
}

void SequencerWidget::SelectNothing(){
    AnythingSelected = 0;
    chordwidget.UnSelect();
    UpdateEverything();
}

void SequencerWidget::UpdateEverything(){
    *dbg << "SeqencerWidget - Updating everything\n";
    if (AnythingSelected){
        
        HideAndShowWidgetsDependingOnSeqType();
        
        if(selectedSeqType == SEQ_TYPE_NOTE){
                UpdateShowChord();
                UpdateChord();
                UpdateChannel();
                UpdateOnOff(); //will also update colour
                UpdateName();
                UpdateRelLenBoxes();
                InitNotebook();
                UpdateActivePattern();
        }else if(selectedSeqType == SEQ_TYPE_CONTROL){
                UpdateChannel();
                UpdateOnOff(); //will also update colour
                UpdateName();
                InitNotebook();
                UpdateRelLenBoxes();
                UpdateActivePattern();
                UpdateController();
                UpdateSlopeType();
        }
        //Glib::signal_timeout().connect(mem_fun(pattern_widget,&PatternWidget::RedrawEverything),50);
    }else{

    }
}

void SequencerWidget::HideAndShowWidgetsDependingOnSeqType(){
    if(!AnythingSelected) return;
    if(selectedSeqType == SEQ_TYPE_NOTE){
        wChordNotebook.set_current_page(0); //chordwidget
        wValueButton.hide();
        wValueLabel.hide();
        wControllerButton.hide();
        wControllerLabel.hide();
        wSlopeSelectorSep.hide();
        wCtrlSlopeFlat.hide();
        wCtrlSlopeLinear.hide();
        wVelocityButton.show();
        wVelocityLabel.show();
        wShowChordButton.show();
    }else if(selectedSeqType == SEQ_TYPE_CONTROL){
        chordwidget.SetExpandDetails(0);
        chordwidget.UnSelect();
        wChordNotebook.set_current_page(1); //wCtrlHBox
        wShowChordButton.hide();
        wVelocityLabel.hide();
        wVelocityButton.hide();
        wValueButton.show();
        wValueLabel.show();
        wControllerButton.show();
        wControllerLabel.show();
        wSlopeSelectorSep.show();
        wCtrlSlopeFlat.show();
        wCtrlSlopeLinear.show();
    }
}

void SequencerWidget::UpdateOnOff(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);
    ignore_signals = 1;
    wMuteToggle.set_active(seq->GetOn());
    ignore_signals = 0;
    UpdateOnOffColour();
}
void SequencerWidget::UpdateChannel(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);
    ignore_signals = 1;
    wChannelButton.set_value(seq->GetChannel());
    ignore_signals = 0;
}
void SequencerWidget::UpdateActivePattern(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);

    ignore_signals = 1;
    UpdateAsterisk(wActivePattern.get_value(),seq->GetActivePatternNumber());
    wActivePattern.set_value(seq->GetActivePatternNumber());
    ignore_signals = 0;
}
void SequencerWidget::UpdateChord(){
    if (AnythingSelected == 0) return;

    ignore_signals = 1;
    chordwidget.Update();
    ignore_signals = 0;
}

void SequencerWidget::UpdateShowChord(){
    if (AnythingSelected == 0 || selectedSeqType != SEQ_TYPE_NOTE) return;
    NoteSequencer* noteseq = dynamic_cast<NoteSequencer*>(seqH(selectedSeq));
    ignore_signals = 1;
    wShowChordButton.set_active(noteseq->expand_chord);
    ignore_signals = 0;
    chordwidget.SetExpandDetails(noteseq->expand_chord);
}

void SequencerWidget::UpdateController(){
    if (AnythingSelected == 0 || selectedSeqType != SEQ_TYPE_CONTROL) return;
    ignore_signals = 1;
    ControlSequencer* ctrlseq = dynamic_cast<ControlSequencer*>(seqH(selectedSeq));
    wControllerButton.set_value(ctrlseq->controller_number);
    ignore_signals = 0;
}

void SequencerWidget::UpdateRelLenBoxes(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);
    
    ignore_signals = 1;
    
   wLengthNumerator.set_value(seq->GetLengthNumerator());
   wLengthDenominator.set_value(seq->GetLengthDenominator());
    char temp[20];
   sprintf(temp," = %.4f",seq->GetLength());
   wLengthResult.set_text(temp);
        
   wResolutions.set_value(seq->resolution);
   
   ignore_signals = 0;
}

void SequencerWidget::UpdateName(){
    if (AnythingSelected == 0) return;
    Sequencer* seq = seqH(selectedSeq);
    ignore_signals = 1;
    wNameEntry.set_text(seq->GetName());
    ignore_signals = 0;
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

    for (unsigned int x = 0; x < notebook_pages.size();x++){
        if(!notebook_pages[x]) continue;
        wNotebook.remove_page(*notebook_pages[x]);
        delete notebook_pages[x];
    }
    notebook_pages.clear();

    notebook_pages.resize(seq->patterns.size(),NULL);
    for (unsigned int x = 0; x < seq->patterns.size();x++){
        notebook_pages[x] = new Gtk::Label;
        notebook_pages[x]->show();
        sprintf(temp,_("%d"),x);
        wNotebook.append_page(*notebook_pages[x],temp);
    }
    do_not_react_on_page_changes = 0;

    //reset the current page
    wNotebook.set_current_page(seqH(selectedSeq)->GetActivePatternNumber());
    UpdatePatternWidget();

    UpdateActivePatternRange();
    UpdateAsterisk(wActivePattern.get_value(),seq->GetActivePatternNumber()); //this will mark active tab with a star (Pat x*)
    SetRemoveButtonSensitivity(); //according to the number of pages
}

void SequencerWidget::UpdatePatternWidget(int pattern){
    *dbg << "Updating pattern widget... \n";
    if (!AnythingSelected) return;
    //if called without parameter...:
    if (pattern == -1) pattern = wNotebook.get_current_page();
    Sequencer* seq = seqH(selectedSeq);
    *dbg<<"Assigining pattern no. " << pattern << ", type: " << ((selectedSeqType == SEQ_TYPE_NOTE)?"Note":"Control") << ENDL;
    pattern_widget.AssignPattern(&seq->patterns[pattern],selectedSeqType);
    pattern_widget.SetInternalHeight(chordwidget.get_height());
}

void SequencerWidget::UpdateActivePatternRange(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    ignore_signals = 1;
    int v = wActivePattern.get_value();
    wActivePattern.set_range(0.0,(double)seq->patterns.size()-1);
    wActivePattern.set_increments(1.0,1.0);
    wActivePattern.set_value(v); //if it's too high, it will change to largest possible
    ignore_signals = 0;
}

void SequencerWidget::UpdateOnOffColour(){
    Sequencer* seq = seqH(selectedSeq);
    if (seq->GetOn()){
        SetOnOffColour(ON);
    }else if (seq->GetPlayOncePhase() == 2 || seq->GetPlayOncePhase() == 3) {
        SetOnOffColour(ONCE);
    } else if (seq->GetPlayOncePhase() == 1) {
        SetOnOffColour(ONCE_PRE);
    } else {
        SetOnOffColour(NONE);
    }
}

void SequencerWidget::OnChannelChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    
    seq->SetChannel( wChannelButton.get_value());
    mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}

void SequencerWidget::OnVelocityChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    
    pattern_widget.SetSelectionVelocity(wVelocityButton.get_value());
    
    Files::SetFileModified(1);
}

void SequencerWidget::OnValueChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    
    pattern_widget.SetSelectionValue(wValueButton.get_value());
    
    Files::SetFileModified(1);
}

void SequencerWidget::OnSlopeFlatToggled(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    bool f = wCtrlSlopeFlat.get_active();
    //bool l = wCtrlSlopeLinear.get_active();
    if(f){
        pattern_widget.SetSlopeType(SLOPE_TYPE_FLAT);
    }else{
        wCtrlSlopeLinear.set_active(1);
    }
    
    Files::SetFileModified(1);
}

void SequencerWidget::OnSlopeLinearToggled(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    //bool f = wCtrlSlopeFlat.get_active();
    bool l = wCtrlSlopeLinear.get_active();
    if(l){
        pattern_widget.SetSlopeType(SLOPE_TYPE_LINEAR);
    }else{
        wCtrlSlopeFlat.set_active(1);
    }
    
    Files::SetFileModified(1);
}

void SequencerWidget::UpdateSlopeType(){
    ignore_signals = 1;
    SlopeType s = pattern_widget.GetSlopeType();
    if(s == SLOPE_TYPE_NONE){
        wCtrlSlopeNone.set_active(1);
    }else if(s == SLOPE_TYPE_FLAT){
        wCtrlSlopeFlat.set_active(1);
    }else if(s == SLOPE_TYPE_LINEAR){
        wCtrlSlopeLinear.set_active(1);
    }
    ignore_signals = 0;
}

void SequencerWidget::OnChordWidgetChanged(){
    //The chord updates the seq on it's own.
    //Just refresh the row.
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    mainwindow->RefreshRow(seq->my_row);
}
void SequencerWidget::OnChordWidgetNoteChanged(int n, int p){
    //if(!AnythingSelected) return;
    //Sequencer* seq = seqH(selectedSeq);
}

void SequencerWidget::OnToggleMuteToggled(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    seq->SetOn(wMuteToggle.get_active());
    seq->SetPlayOncePhase(0);
    mainwindow->RefreshRow(seq->my_row);
    UpdateOnOffColour();

    //Files::SetFileModified(1); come on, do not write mutes.
}
void SequencerWidget::OnResolutionChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    
    Sequencer* seq = seqH(selectedSeq);


    seq->SetResolution(wResolutions.get_value());
    pattern_widget.RedrawGrid();

    mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}
void SequencerWidget::OnLengthChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    seq->SetLength(wLengthNumerator.get_value(),wLengthDenominator.get_value());
    char temp[20];
    sprintf(temp, " = %.4f", seq->GetLength());
    wLengthResult.set_text(temp);
    seq->play_from_here_marker = 0.0; //important, to avoid shifts

    mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}
void SequencerWidget::OnActivePatternChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    int activepattern = wActivePattern.get_value();
    int old = seq->GetActivePatternNumber();

    UpdateAsterisk(old,activepattern);

    seq->SetActivePatternNumber(activepattern); //store in parent

    mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}

void SequencerWidget::UpdateAsterisk(int from, int to){
    //changing notepad tab labels
    char temp[100];

    if(wNotebook.get_n_pages() == 0) return;
    
    sprintf(temp,_(" %d"),from);
    wNotebook.set_tab_label_text(*notebook_pages[from],temp);

    sprintf(temp,_("%d*"),to);
    wNotebook.set_tab_label_text(*notebook_pages[to],temp);

}
void SequencerWidget::OnSetAsActivePatternClicked(){
    int current = wNotebook.get_current_page();
    wActivePattern.set_value((double)current);    
}
void SequencerWidget::OnNotebookPageChanged(GtkNotebookPage* page, guint page_num){
    if(ignore_signals) return;
    if(do_not_react_on_page_changes) return;
    *dbg << "page changed!\n";
    UpdatePatternWidget();
}
void SequencerWidget::OnAddPatternClicked(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    char temp[100];

    seq->AddPattern();

    notebook_pages.push_back(new Gtk::Label);
    int x = notebook_pages.size() - 1;
    notebook_pages[x]->show();
    sprintf(temp, _("%d"), x);
    wNotebook.append_page(*notebook_pages[x], temp);
    wNotebook.set_current_page(wNotebook.get_n_pages()-1); //will show the last page AND THE SIGNAL HANDLER WILL UPDATE THE PATTERNWIDGET!
    //In case you wonder why the patternbox is NOT updated: read the comment above.
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}
void SequencerWidget::OnRemovePatternClicked(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    int n = wNotebook.get_current_page();
    *dbg << "removing pattern " << n <<"\n";
    do_not_react_on_page_changes = 1;
    wNotebook.remove(*notebook_pages[n]);
    delete notebook_pages[n];
    notebook_pages.erase(notebook_pages.begin()+n);
    seq->patterns.erase(seq->patterns.begin()+n);
    do_not_react_on_page_changes = 0;
    if (seq->GetActivePatternNumber() == n ) { seq->SetActivePatternNumber(0);wActivePattern.set_value(0.0);}
    if (seq->GetActivePatternNumber() > n ) {seq->SetActivePatternNumber(seq->GetActivePatternNumber()-1);wActivePattern.set_value(seq->GetActivePatternNumber()); }
    InitNotebook();
    wNotebook.set_current_page(n);
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    mainwindow->RefreshRow(seq->my_row);
    Files::SetFileModified(1);
}

void SequencerWidget::OnClearPatternClicked(){
    if(!AnythingSelected) return;
    seqH(selectedSeq)->ClearPattern(wNotebook.get_current_page());
    UpdatePatternWidget();
}

void SequencerWidget::OnClonePatternClicked(){
    if(!AnythingSelected) return;
    AtomContainer pattern = seqH(selectedSeq)->patterns[wNotebook.get_current_page()];
    int n = seqH(selectedSeq)->AddPattern();
    AtomContainer* newpattern = &seqH(selectedSeq)->patterns[n];
    *newpattern = pattern;
    InitNotebook();
    wNotebook.set_current_page(n);
    UpdateActivePatternRange();
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
    if(ignore_signals) return;
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);
    
    seq->SetName(wNameEntry.get_text());
    mainwindow->RefreshRow(seq->my_row);
    mainwindow->eventsWidget.SeqListChanged();
    Files::SetFileModified(1);

}
void SequencerWidget::OnPlayOnceButtonClicked(){
    if(!AnythingSelected) return;
    Sequencer* seq = seqH(selectedSeq);

    seq->SetPlayOncePhase(1);
    mainwindow->RefreshRow(seq->my_row);
    UpdateOnOffColour();
}

void SequencerWidget::OnSelectionChanged(int n){
    if(ignore_signals) return;
    ignore_signals = 1;
    if(selectedSeqType == SEQ_TYPE_NOTE){
        if(n == 0){
            //empty selection
            wVelocityButton.set_sensitive(0);
        }else{
            wVelocityButton.set_sensitive(1);
            wVelocityButton.set_value(pattern_widget.GetSelectionVelocity());
        }
    }else if(selectedSeqType == SEQ_TYPE_CONTROL){
        if(n == 0){
            //empty selection
            wValueButton.set_sensitive(0);
        }else{
            wValueButton.set_sensitive(1);
            wValueButton.set_value(pattern_widget.GetSelectionValue());
        }
        UpdateSlopeType();
    }
    ignore_signals = 0;
}

void SequencerWidget::OnSnapClicked(){
    pattern_widget.SetSnap(wSnapToggle.get_active());
}

void SequencerWidget::OnControllerChanged(){
    if(ignore_signals) return;
    if(!AnythingSelected || selectedSeqType != SEQ_TYPE_CONTROL) return;
    ControlSequencer* ctrlseq = dynamic_cast<ControlSequencer*>(seqH(selectedSeq));
    ctrlseq->controller_number = wControllerButton.get_value();
    mainwindow->RefreshRow(ctrlseq->my_row);
}

void SequencerWidget::OnShowChordButtonClicked(){
    if(ignore_signals) return;
    if(!AnythingSelected || selectedSeqType != SEQ_TYPE_NOTE) return;
    NoteSequencer* noteseq = dynamic_cast<NoteSequencer*>(seqH(selectedSeq));
    bool show = wShowChordButton.get_active();
    noteseq->expand_chord = show;
    chordwidget.SetExpandDetails(show);
}

void SequencerWidget::SetOnOffColour(OnOffColour c){
    if (c == NONE) {
        wOnOfColour.unset_bg(Gtk::STATE_NORMAL);
        wMuteToggle.unset_bg(Gtk::STATE_PRELIGHT);
    }else if (c == ON) {
        wOnOfColour.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("green1"));
        wMuteToggle.modify_bg(Gtk::STATE_PRELIGHT,Gdk::Color("green3"));
    }else if (c ==ONCE) {
        wOnOfColour.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("yellow"));
        wMuteToggle.modify_bg(Gtk::STATE_PRELIGHT,Gdk::Color("yellow2"));
    }else if (c == ONCE_PRE){
        wOnOfColour.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("gold"));
        wMuteToggle.modify_bg(Gtk::STATE_PRELIGHT,Gdk::Color("gold2"));
        
    }
}

void SequencerWidget::OnPatternWidgetScrollLeft(){
    double inc = wViewport->get_hadjustment()->get_step_increment();
    wViewport->get_hadjustment()->set_value(-inc + wViewport->get_hadjustment()->get_value());
    if (wViewport->get_hadjustment()->get_value() > wViewport->get_hadjustment()->get_upper() - wViewport->get_hadjustment()->get_page_size()) wViewport->get_hadjustment()->set_value(wViewport->get_hadjustment()->get_upper() - wViewport->get_hadjustment()->get_page_size());

}

void SequencerWidget::OnPatternWidgetScrollRight(){
    double inc = wViewport->get_hadjustment()->get_step_increment();
    if (!(wViewport->get_hadjustment()->get_value() + wViewport->get_hadjustment()->get_page_size() + inc > wViewport->get_hadjustment()->get_upper()))
        wViewport->get_hadjustment()->set_value(inc + wViewport->get_hadjustment()->get_value());
    else
        //too high value, trimming to desired
        wViewport->get_hadjustment()->set_value(wViewport->get_hadjustment()->get_upper() - wViewport->get_hadjustment()->get_page_size());
    
    if (wViewport->get_hadjustment()->get_value() > wViewport->get_hadjustment()->get_upper() - wViewport->get_hadjustment()->get_page_size()) wViewport->get_hadjustment()->set_value(wViewport->get_hadjustment()->get_upper() - wViewport->get_hadjustment()->get_page_size());

}

void SequencerWidget::DeacivateAllDiodes(){
    pattern_widget.AllDiodesOff();
}

void SequencerWidget::ActivateDiode(DiodeMidiEvent diodev){
    pattern_widget.LightUpDiode(diodev);
}

/*
void SequencerWidget::Diode(int n, int c){
    if(!AnythingSelected) return;
    
    //double x = (double)n/(double)DIODES_RES;
    int res = seqH(selectedSeq)->resolution;
    if (res == 0) return;
    if(n >= res) return;
    int curr = n;//(double)*(double)res; //yep, rounding down
    int prev = (curr-1);
    if (prev == -1) prev = res-1; //if the previous is too small, wrap it and select the last one
    if(prev < res && pattern_lines[prev]) pattern_lines[prev]->LightOff();
        
    if(curr < res  && pattern_lines[curr]) {
        if (c == 0)
            pattern_lines[curr]->LightOn();
        else
            pattern_lines[curr]->LightOnAlternate();
    }
}

void SequencerWidget::Diodes_AllOff(){
    if(!AnythingSelected) return;
    //double x = (double)n/(double)DIODES_RES;
    
    for (int x = 0; x < pattern_lines.size(); x++) if (pattern_lines[x]) pattern_lines[x]->LightOff();*
}*/

//====================PATTERNLINE=========================
/* Depracated.
PatternLine::PatternLine(){
    set_border_width(0);
    pack_end(marker,Gtk::PACK_SHRINK);
    pack_end(diode,Gtk::PACK_SHRINK);
    for (int x = 0; x < 6; x++){
        buttons.push_back(new Gtk::CheckButton);
        pack_end(*buttons[x],Gtk::PACK_EXPAND_PADDING); //check the pack flag
        buttons[x]->signal_toggled().connect(sigc::bind<int>(sigc::mem_fun(*this,&PatternLine::OnButtonsToggled),x));
        //buttons[x]->set_border_width(0);
        buttons[x]->show();
    }
    marker.set_text(" ");
    diode.set_size_request(-1,4);
    diode.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("brown3"));
    marker.show();
    diode.show();
    diode_on = 0;
}

PatternLine::PatternLine(Glib::ustring mark){
    set_border_width(0);
    pack_end(marker,Gtk::PACK_SHRINK);
    pack_end(diode,Gtk::PACK_SHRINK);
    for (int x = 0; x < 6; x++){
        buttons.push_back(new Gtk::CheckButton);
        pack_end(*buttons[x],Gtk::PACK_EXPAND_PADDING); //check the pack flag
        buttons[x]->signal_toggled().connect(sigc::bind<int>(sigc::mem_fun(*this,&PatternLine::OnButtonsToggled),x));
        //buttons[x]->set_border_width(0);
        buttons[x]->show();
    }
    marker.set_text(mark);
    diode.set_size_request(-1,4);
    diode.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("brown3"));
    marker.show();
    diode.show();
    diode_on = 0;
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

void PatternLine::LightOn(){
    if(diode_on == 1) return;
    diode.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("green1"));
    diode_on = 1;
}

void PatternLine::LightOnAlternate(){
    if(diode_on == 1) return;
    diode.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("yellow1"));
    diode_on = 1;
}

void PatternLine::LightOff(){
    if(diode_on == 0) return;
    diode.modify_bg(Gtk::STATE_NORMAL,Gdk::Color("brown3"));
    diode_on = 0;
}
 **/
