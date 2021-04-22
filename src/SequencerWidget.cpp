/*
    Copyright (C) 2010-2012, 2020 Rafał Cieślak

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

#include "SequencerWidget.hpp"

#include <functional>

#include "AtomContainer.hpp"
#include "ControlSequencer.hpp"
#include "Files.hpp"
#include "NoteSequencer.hpp"
#include "SequencerManager.hpp"
#include "resources.hpp"


SequencerWidget::SequencerWidget()
{
    set_name("sequencer-widget");

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

    wNotebookAndPatternOpsHBox.pack_start(wScrolledWindow/*,Gtk::PACK_EXPAND_WIDGET*/);
    wNotebookAndPatternOpsHBox.pack_start(wNotebook,Gtk::PACK_SHRINK);
    wNotebookAndPatternOpsHBox.pack_end(wPtOpsVBox,Gtk::PACK_SHRINK);

    wScrolledWindow.add(wPatternWidgetBox);
    wPatternWidgetBox.pack_start(pattern_widget,Gtk::PACK_SHRINK);

    wScrolledWindow.set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_NEVER);
    wScrolledWindow.set_overlay_scrolling(false);

    pattern_widget.on_selection_changed.connect(std::bind(&SequencerWidget::OnSelectionChanged, this, std::placeholders::_1));
    pattern_widget.on_slope_type_needs_additional_refreshing.connect(std::bind(&SequencerWidget::UpdateSlopeType, this));

    wUpperHBox1.pack_start(wNameLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wNameEntry,Gtk::PACK_SHRINK);
    wNameEntry.set_width_chars(10);
    wNameEntry.signal_changed().connect(std::bind(&SequencerWidget::OnNameEdited, this));
    wUpperHBox1.pack_start(wOnOfColour,Gtk::PACK_SHRINK);
    wOnOfColour.add(wMuteToggle);
    wUpperHBox1.pack_start(wPlayOnceButton,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelSep,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wChannelButton,Gtk::PACK_SHRINK);
    wChannelSep.set_size_request(8,0);
    wUpperHBox1.pack_start(wControllerLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wControllerButton,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wGatePercentLabel,Gtk::PACK_SHRINK);
    wUpperHBox1.pack_start(wGatePercentButton,Gtk::PACK_SHRINK);

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
    wUpperHBox2.pack_end(wZoomOut,Gtk::PACK_SHRINK);
    wUpperHBox2.pack_end(wZoomIn,Gtk::PACK_SHRINK);
    wVelSep.set_size_request(8,0);
    wSlopeSelectorSep.set_size_request(8,0);

    wShowChordButton.add(wShowChordLabel);
    wShowChordLabel.set_markup(_("<b>Chord</b>"));
    wShowChordButton.set_tooltip_markup(_("Shows/hides the chord's detailed settings."));
    wShowChordButton.signal_toggled().connect(std::bind(&SequencerWidget::OnShowChordButtonClicked, this));

    wSnapToggle.set_label(_("Snap to grid"));
    wSnapToggle.set_tooltip_markup(_("If on, notes will be snapped to the visible grid.\nGrid density changes with sequence resolution."));
    wSnapToggle.set_active(1); //As this is default value
    wSnapToggle.signal_toggled().connect(std::bind(&SequencerWidget::OnSnapClicked, this));

    wZoomInImg.set(Gtk::Stock::ZOOM_IN,Gtk::ICON_SIZE_MENU);
    wZoomOutImg.set(Gtk::Stock::ZOOM_OUT,Gtk::ICON_SIZE_MENU);
    wZoomIn.set_image(wZoomInImg);
    wZoomOut.set_image(wZoomOutImg);
    wZoomIn.set_relief(Gtk::RELIEF_NONE);
    wZoomOut.set_relief(Gtk::RELIEF_NONE);
    wZoomIn.set_border_width(0);

    wZoomOut.set_border_width(0);
    wZoomIn.set_tooltip_markup(_("<b>Zoom in</b> (Ctrl+MouseScroll)"));
    wZoomOut.set_tooltip_markup(_("<b>Zoom out</b> (Ctrl+MouseScroll)"));
    wZoomIn.signal_clicked().connect(std::bind(&SequencerWidget::OnZoomInClicked, this));
    wZoomOut.signal_clicked().connect(std::bind(&SequencerWidget::OnZoomOutClicked, this));
    wZoomInImg.show();
    wZoomOutImg.show();

    wBoxOfChord.pack_start(wChordNotebook,Gtk::PACK_EXPAND_WIDGET);
    wBoxOfChord.pack_start(wVirtualSpaceLabel,Gtk::PACK_SHRINK,1); //extra alligments space - 1 stands for the notebook's border witdth
    wVirtualSpaceLabel.set_text(" ");
    wVirtualSpaceLabel.set_size_request(6, 0);
    wChordNotebook.append_page(chordwidget);
    wChordNotebook.append_page(wCtrlHBox);
    wChordNotebook.set_show_tabs(0);
    wChordNotebook.set_show_border(0);

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
    wNotebook.signal_switch_page().connect(std::bind(&SequencerWidget::OnNotebookPageChanged, this, std::placeholders::_1, std::placeholders::_2));

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

    wClearPattern.signal_clicked().connect(std::bind(&SequencerWidget::OnClearPatternClicked, this));
    wClonePattern.signal_clicked().connect(std::bind(&SequencerWidget::OnClonePatternClicked, this));
    wSetAsActivePatternButton.signal_clicked().connect(std::bind(&SequencerWidget::OnSetAsActivePatternClicked, this));
    wAddPatternButton.signal_clicked().connect(std::bind(&SequencerWidget::OnAddPatternClicked, this));
    wRemovePattern.signal_clicked().connect(std::bind(&SequencerWidget::OnRemovePatternClicked, this));

    wPlayOnceButton.signal_clicked().connect(std::bind(&SequencerWidget::OnPlayOnceButtonClicked, this));


    wActivePanelLabel.set_text(_("Active pattern:"));
    wSetAsActivePatternButton.set_label(_("Set as active pattern"));
    wSetAsActivePatternButton.set_tooltip_markup(_("Sets the chosen pattern to be the <b>active</b> one, which means the one that  will be played back."));

    wActivePattern.set_tooltip_markup(_("Selects which patter is <b>active</b>.\nActive pattern is the one that is played back. It's marked on a list with an asterisk (*).\n\n<i>This way all patterns can be edited while only the selected (the active) is played back. </i>"));
    wActivePattern.signal_value_changed().connect(std::bind(&SequencerWidget::OnActivePatternChanged, this));

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

    wChannelLabel.set_text(_("MIDI channel:"));
    wChannelButton.set_increments(1,1);
    wChannelButton.set_range(1,16);
    wChannelButton.set_width_chars(2);
    wChannelButton.set_tooltip_markup(_("Selects the <b>MIDI channel</b> this sequencer will output notes to. "));
    wChannelButton.signal_value_changed().connect(std::bind(&SequencerWidget::OnChannelChanged, this));

    wVelocityLabel.set_text(_("Velocity:"));
    wVelocityButton.set_range(0,127);
    wVelocityButton.set_increments(1,16);
    wVelocityButton.set_width_chars(3);
    wVelocityButton.set_tooltip_markup(_("Sets the <b>velocity</b> of selected note(s).\nUsually higher velocities result in louder sounds."));
    wVelocityButton.signal_value_changed().connect(std::bind(&SequencerWidget::OnVelocityChanged, this));

    wValueLabel.set_text(_("Value:"));
    wValueButton.set_range(0,127);
    wValueButton.set_increments(1,16);
    wValueButton.set_width_chars(3);
    wValueButton.set_tooltip_markup(_("Sets the <b>value</b> of selected point(s)."));
    wValueButton.signal_value_changed().connect(std::bind(&SequencerWidget::OnValueChanged, this));

    /* TRANSLATORS: The space befor this string is to force a tiny space between widgets, please keep it in translations.*/
    wControllerLabel.set_text(_(" Controller No."));
    wControllerButton.set_increments(1.0,16.0);
    wControllerButton.set_range(0.0,127.0);
    wControllerButton.set_width_chars(3);
    wControllerButton.set_tooltip_markup(_("The <b>MIDI controller number</b> this sequencer outputs data on.\n\nFor example, synthesizers supporting GM standard should interpret data from controller 7 as volume setting."));
    wControllerButton.signal_value_changed().connect(std::bind(&SequencerWidget::OnControllerChanged, this));

    wGatePercentLabel.set_text(_("  Gate %:"));
    wGatePercentButton.set_range(0, 110);
    wGatePercentButton.set_increments(1, 10);
    wGatePercentButton.set_width_chars(3);
    wGatePercentButton.set_tooltip_markup(_("Controls sequencer note <b>gating</b>.\n\nAt the value of 100% all notes will be played at their original length.\nAt 50%, all notes will last only half of their time.\nValues slightly higher than 100% are allowed, this is useful for portamento effects on some synthesizers."));
    wGatePercentButton.signal_value_changed().connect(std::bind(&SequencerWidget::OnGatePercentChanged, this));

    wMuteToggle.set_label(_("ON/OFF"));
    wMuteToggle.set_tooltip_markup(_("Turns this sequencer <b>on/off</b>."));
    wMuteToggle.signal_clicked().connect(std::bind(&SequencerWidget::OnToggleMuteToggled, this));


    //todo: add icons & tooltips
    wCtrlSlopeFlat.set_label("F");
    wCtrlSlopeLinear.set_label("L");
    wCtrlSlopeFlat.set_mode(0);
    wCtrlSlopeLinear.set_mode(0);
    Gtk::RadioButton::Group group = wCtrlSlopeFlat.get_group();
    wCtrlSlopeLinear.set_group(group);
    wCtrlSlopeNone.set_group(group);
    wCtrlSlopeFlat.signal_toggled().connect(std::bind(&SequencerWidget::OnSlopeFlatToggled, this));
    wCtrlSlopeLinear.signal_toggled().connect(std::bind(&SequencerWidget::OnSlopeLinearToggled, this));
    //my_slope_mode_for_adding = SLOPE_TYPE_LINEAR;

    //lengths selector
    wResolutionsLabel.set_text(_("Resolution:"));
    wResolutions.set_range(1.0,64.0);
    wResolutions.set_increments(1.0,4.0);
    wResolutions.set_width_chars(2);
    wResolutions.set_tooltip_markup(_("Selects the <b>resolution</b> of this sequencer. It defines the grid density in this sequencer's patterns."));
    wResolutions.signal_value_changed().connect(std::bind(&SequencerWidget::OnResolutionChanged, this));


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
    wLengthNumerator.signal_value_changed().connect(std::bind(&SequencerWidget::OnLengthChanged, this));
    wLengthDenominator.signal_value_changed().connect(std::bind(&SequencerWidget::OnLengthChanged, this));

    pattern_widget.on_scroll_left.connect(std::bind(&SequencerWidget::OnPatternWidgetScrollLeft, this));
    pattern_widget.on_scroll_right.connect(std::bind(&SequencerWidget::OnPatternWidgetScrollRight, this));

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
    // delete wViewport;
}

void SequencerWidget::SelectSeq(std::shared_ptr<Sequencer> seq){
    selectedSeq = seq;

    if(!seq) return;

    seq->on_playstate_change.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdateOnOff(); });});

    seq->on_activepattern_change.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdateActivePattern(); });});

    if(auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq)){
        chordwidget.Select(&noteseq->chord);

        noteseq->on_chord_change.connect(
            [=](){ DeferWorkToUIThread(
                    [=](){ UpdateChord(); });});
    }

    UpdateEverything();
    UpdatePatternWidget();
    DeacivateAllDiodes();
}

void SequencerWidget::SelectNothing(){
    selectedSeq = nullptr;
    chordwidget.UnSelect();
    UpdateEverything();
    UpdatePatternWidget();
}

void SequencerWidget::UpdateEverything(){
    if (!selectedSeq) return;

    HideAndShowWidgetsDependingOnSeqType();

    if(auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq)){
            UpdateShowChord();
            UpdateChord();
            UpdateChannel();
            UpdateOnOff(); //will also update colour
            UpdateName();
            UpdateRelLenBoxes();
            UpdateGatePercent();
            InitNotebook();
            UpdateActivePattern();
    }else if(auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(selectedSeq)){
            UpdateChannel();
            UpdateOnOff(); //will also update colour
            UpdateName();
            InitNotebook();
            UpdateRelLenBoxes();
            UpdateActivePattern();
            UpdateController();
            UpdateSlopeType();
    }
}

void SequencerWidget::HideAndShowWidgetsDependingOnSeqType(){
    if(!selectedSeq) return;
    if(auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq)){
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
        wGatePercentButton.show();
        wGatePercentLabel.show();
    }else if(auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(selectedSeq)){
        chordwidget.SetExpandDetails(0);
        chordwidget.UnSelect();
        wChordNotebook.set_current_page(1); //wCtrlHBox
        wShowChordButton.hide();
        wVelocityLabel.hide();
        wVelocityButton.hide();
        wGatePercentButton.hide();
        wGatePercentLabel.hide();
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
    if (!selectedSeq) return;

    ignore_signals = 1;
    wMuteToggle.set_active(selectedSeq->GetOn());
    ignore_signals = 0;
    UpdateOnOffColour();
}
void SequencerWidget::UpdateChannel(){
    if (!selectedSeq) return;

    ignore_signals = 1;
    wChannelButton.set_value(selectedSeq->GetChannel());
    ignore_signals = 0;
}
void SequencerWidget::UpdateActivePattern(){
    if (!selectedSeq) return;

    ignore_signals = 1;
    UpdateAsterisk(wActivePattern.get_value(), selectedSeq->GetActivePatternNumber());
    wActivePattern.set_value(selectedSeq->GetActivePatternNumber());
    ignore_signals = 0;
}
void SequencerWidget::UpdateChord(){
    if (!selectedSeq) return;

    ignore_signals = 1;
    chordwidget.Update();
    ignore_signals = 0;
}

void SequencerWidget::UpdateShowChord(){
    if (!selectedSeq) return;
    auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq);
    if (!noteseq) return;

    ignore_signals = 1;
    wShowChordButton.set_active(noteseq->expand_chord);
    ignore_signals = 0;
    chordwidget.SetExpandDetails(noteseq->expand_chord);
}

void SequencerWidget::UpdateController(){
    if (!selectedSeq) return;
    auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(selectedSeq);
    if (!ctrlseq) return;

    ignore_signals = 1;
    wControllerButton.set_value(ctrlseq->GetControllerNumber());
    ignore_signals = 0;
}

void SequencerWidget::UpdateGatePercent(){
    auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq);
    if (!noteseq) return;

    int gate_percent = noteseq->GetGatePercent();
    if (gate_percent != wGatePercentButton.get_value())
        wGatePercentButton.set_value(gate_percent);
}

void SequencerWidget::UpdateRelLenBoxes(){
    if (!selectedSeq) return;

    ignore_signals = 1;

   wLengthNumerator.set_value(selectedSeq->GetLengthNumerator());
   wLengthDenominator.set_value(selectedSeq->GetLengthDenominator());

   char temp[20];
   sprintf(temp," = %.4f",selectedSeq->GetLength());
   wLengthResult.set_text(temp);

   wResolutions.set_value(selectedSeq->GetResolution());

   ignore_signals = 0;
}

void SequencerWidget::UpdateName(){
    if (!selectedSeq) return;
    ignore_signals = 1;
    wNameEntry.set_text(selectedSeq->GetName());
    ignore_signals = 0;
}

void SequencerWidget::InitNotebook(){
    if(!selectedSeq) return;

    char temp[100];
    do_not_react_on_page_changes = 1;

    for (unsigned int x = 0; x < notebook_pages.size();x++){
        if(!notebook_pages[x]) continue;
        wNotebook.remove_page(*notebook_pages[x]);
        delete notebook_pages[x];
    }
    notebook_pages.clear();

    notebook_pages.resize(selectedSeq->patterns.size(),NULL);
    for (unsigned int x = 0; x < selectedSeq->patterns.size();x++){
        notebook_pages[x] = new Gtk::Label;
        notebook_pages[x]->show();
        sprintf(temp,_("%d"),x);
        wNotebook.append_page(*notebook_pages[x],temp);
    }
    do_not_react_on_page_changes = 0;

    //reset the current page
    wNotebook.set_current_page(selectedSeq->GetActivePatternNumber());
    UpdatePatternWidget();

    UpdateActivePatternRange();
    UpdateAsterisk(wActivePattern.get_value(),
                   selectedSeq->GetActivePatternNumber()); //this will mark active tab with a star (Pat x*)
    SetRemoveButtonSensitivity(); //according to the number of pages
}

void SequencerWidget::UpdatePatternWidget(int pattern){
    if (!selectedSeq) {
        pattern_widget.AssignPattern(nullptr, nullptr);
        return;
    }
    //if called without parameter...:
    if (pattern == -1) pattern = wNotebook.get_current_page();

    pattern_widget.AssignPattern(selectedSeq->patterns[pattern], selectedSeq);
    pattern_widget.SetInternalHeight(chordwidget.get_height());
}

void SequencerWidget::UpdateActivePatternRange(){
    if(!selectedSeq) return;

    ignore_signals = 1;
    int v = wActivePattern.get_value();
    wActivePattern.set_range(0.0,(double)selectedSeq->patterns.size()-1);
    wActivePattern.set_increments(1.0,1.0);
    wActivePattern.set_value(v); //if it's too high, it will change to largest possible
    ignore_signals = 0;
}

void SequencerWidget::UpdateOnOffColour(){
    int phase = selectedSeq->GetPlayOncePhase();
    if (selectedSeq->GetOn()){
        SetOnOffColour(ON);
    }else if (phase == 2 || phase == 3) {
        SetOnOffColour(ONCE);
    } else if (phase == 1) {
        SetOnOffColour(ONCE_PRE);
    } else {
        SetOnOffColour(NONE);
    }
}

void SequencerWidget::OnChannelChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    selectedSeq->SetChannel(wChannelButton.get_value());
    Files::SetFileModified(1);
}

void SequencerWidget::OnVelocityChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    pattern_widget.SetSelectionVelocity(wVelocityButton.get_value());

    Files::SetFileModified(1);
}

void SequencerWidget::OnValueChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    pattern_widget.SetSelectionValue(wValueButton.get_value());

    Files::SetFileModified(1);
}

void SequencerWidget::OnSlopeFlatToggled(){
    if(ignore_signals) return;
    if(!selectedSeq) return;
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
    if(!selectedSeq) return;
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

void SequencerWidget::OnToggleMuteToggled(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    selectedSeq->SetOn(wMuteToggle.get_active());
    selectedSeq->SetPlayOncePhase(0);
    UpdateOnOffColour();

    //Files::SetFileModified(1); come on, do not write mutes.
}
void SequencerWidget::OnResolutionChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    selectedSeq->SetResolution(wResolutions.get_value());
    pattern_widget.RedrawGrid();

    Files::SetFileModified(1);
}
void SequencerWidget::OnLengthChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    selectedSeq->SetLength(wLengthNumerator.get_value(),wLengthDenominator.get_value());
    char temp[20];
    sprintf(temp, " = %.4f", selectedSeq->GetLength());
    wLengthResult.set_text(temp);
    selectedSeq->play_from_here_marker = 0.0; //important, to avoid shifts

    Files::SetFileModified(1);
}
void SequencerWidget::OnActivePatternChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    int activepattern = wActivePattern.get_value();
    int old = selectedSeq->GetActivePatternNumber();

    UpdateAsterisk(old, activepattern);

    selectedSeq->SetActivePatternNumber(activepattern); //store in parent

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
void SequencerWidget::OnNotebookPageChanged(Gtk::Widget* page, guint page_num){
    if(ignore_signals) return;
    if(do_not_react_on_page_changes) return;
    UpdatePatternWidget();
}
void SequencerWidget::OnAddPatternClicked(){
    if(!selectedSeq) return;

    char temp[100];

    selectedSeq->AddPattern(std::make_shared<AtomContainer>());

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
    if(!selectedSeq) return;

    int n = wNotebook.get_current_page();
    do_not_react_on_page_changes = 1;
    wNotebook.remove(*notebook_pages[n]);
    delete notebook_pages[n];
    notebook_pages.erase(notebook_pages.begin()+n);

    selectedSeq->RemovePattern(n);

    do_not_react_on_page_changes = 0;

    InitNotebook();
    wNotebook.set_current_page(n);
    UpdateActivePatternRange();
    SetRemoveButtonSensitivity();
    Files::SetFileModified(1);
}

void SequencerWidget::OnClearPatternClicked(){
    if(!selectedSeq) return;
    selectedSeq->ClearPattern(wNotebook.get_current_page());
    UpdatePatternWidget();
}

void SequencerWidget::OnClonePatternClicked(){
    if(!selectedSeq) return;

    auto pattern = selectedSeq->patterns[wNotebook.get_current_page()];
    auto newpattern = std::make_shared<AtomContainer>();
    *newpattern = *pattern;
    int n = selectedSeq->AddPattern(newpattern);

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
    if(!selectedSeq) return;

    selectedSeq->SetName(wNameEntry.get_text());

    /* TODO: Ideally this signal would never get triggered outside of
     * SequencerManager. */
    SequencerManager::on_sequencer_list_changed();
    Files::SetFileModified(1);
}

void SequencerWidget::OnPlayOnceButtonClicked(){
    if(!selectedSeq) return;

    selectedSeq->SetPlayOncePhase(1);
    UpdateOnOffColour();
}

void SequencerWidget::OnSelectionChanged(int n){
    if(ignore_signals) return;
    if(!selectedSeq) return;

    ignore_signals = 1;
    if(auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq)){
        if(n == 0){
            //empty selection
            wVelocityButton.set_sensitive(0);
        }else{
            wVelocityButton.set_sensitive(1);
            wVelocityButton.set_value(pattern_widget.GetSelectionVelocity());
        }
    }else if(auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(selectedSeq)){
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
    if(!selectedSeq) return;
    auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(selectedSeq);
    if(!ctrlseq) return;

    ctrlseq->SetControllerNumber(wControllerButton.get_value());
}

void SequencerWidget::OnGatePercentChanged(){
    if(ignore_signals) return;
    if(!selectedSeq) return;
    auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq);
    if(!noteseq) return;

    noteseq->SetGatePercent(wGatePercentButton.get_value());
}

void SequencerWidget::OnShowChordButtonClicked(){
    if(ignore_signals) return;
    if(!selectedSeq) return;
    auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(selectedSeq);
    if(!noteseq) return;

    bool show = wShowChordButton.get_active();
    noteseq->expand_chord = show;
    chordwidget.SetExpandDetails(show);
}

void SequencerWidget::SetOnOffColour(OnOffColour c){
  wOnOfColour.get_style_context()->remove_class("seq-on");
  wMuteToggle.get_style_context()->remove_class("seq-on");
  wOnOfColour.get_style_context()->remove_class("seq-once");
  wMuteToggle.get_style_context()->remove_class("seq-once");
  wOnOfColour.get_style_context()->remove_class("seq-oncepre");
  wMuteToggle.get_style_context()->remove_class("seq-oncepre");

  if(c == ON) {
    wOnOfColour.get_style_context()->add_class("seq-on");
    wMuteToggle.get_style_context()->add_class("seq-on");
  }else if(c == ONCE) {
    wOnOfColour.get_style_context()->add_class("seq-once");
    wMuteToggle.get_style_context()->add_class("seq-once");
  }else if(c == ONCE_PRE) {
    wOnOfColour.get_style_context()->add_class("seq-oncepre");
    wMuteToggle.get_style_context()->add_class("seq-oncepre");
  }
}

void SequencerWidget::OnPatternWidgetScrollLeft(){
    auto adjustment = wScrolledWindow.get_hadjustment();
    double inc = adjustment->get_step_increment();
    adjustment->set_value(-inc + adjustment->get_value());
    if (adjustment->get_value() > adjustment->get_upper() - adjustment->get_page_size()) adjustment->set_value(adjustment->get_upper() - adjustment->get_page_size());

}

void SequencerWidget::OnPatternWidgetScrollRight(){
    auto adjustment = wScrolledWindow.get_hadjustment();
    double inc = adjustment->get_step_increment();
    if (!(adjustment->get_value() + adjustment->get_page_size() + inc > adjustment->get_upper()))
        adjustment->set_value(inc + adjustment->get_value());
    else
        //too high value, trimming to desired
        adjustment->set_value(adjustment->get_upper() - adjustment->get_page_size());

    if (adjustment->get_value() > adjustment->get_upper() - adjustment->get_page_size()) adjustment->set_value(adjustment->get_upper() - adjustment->get_page_size());

}

void SequencerWidget::OnZoomInClicked(){
    pattern_widget.ZoomIn();
}
void SequencerWidget::OnZoomOutClicked(){
    pattern_widget.ZoomOut();
}

void SequencerWidget::DeacivateAllDiodes(){
    pattern_widget.AllDiodesOff();
}

void SequencerWidget::ActivateDiode(DiodeMidiEvent diodev){
    pattern_widget.LightUpDiode(diodev);
}
