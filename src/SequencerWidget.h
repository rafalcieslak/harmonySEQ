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
#ifndef SEQUENCERWIDGET_H
#define	SEQUENCERWIDGET_H
#include <gtkmm.h>
#include "seqHandle.h"
#include "ChordWidget.h"
#include "PatternWidget.h"
class Sequencer;
#ifdef YES_I_HOMEHOW_NEED_TO_ACCESS_DEPRACATED_CONTENTS
class PatternLine : public Gtk::VBox{
public:
    PatternLine();
    PatternLine(Glib::ustring mark);
    ~PatternLine();

    /**Sets a button to a given value.*/
    void SetButton(int c, bool value);
    /**Returns the state of a button.*/
    bool GetButton(int c);
    /**Signal emited when one fo the buttons is clicked.*/
    sigc::signal<void,int,bool> OnButtonClicked;
    /**Lights the diode up*/
    void LightOn();
    /**Lights the diode up, using alternative colur.*/
    void LightOnAlternate();
    /**Dims the light*/
    void LightOff();
    Gtk::Label marker;
private:
    void OnButtonsToggled(int c);
    std::vector<Gtk::CheckButton *> buttons;
    Gtk::EventBox diode;
    bool diode_on;
};
#endif

/**Displays given sequencer's properties, and allows to edit them.*/
class SequencerWidget : public Gtk::VBox{
public:
    SequencerWidget();
    virtual ~SequencerWidget();

    void UpdateEverything();
    void UpdateRelLenBoxes();
    void UpdateName();
    void UpdateOnOff();
    void UpdateChannel();
    void UpdateChord();
    void UpdateActivePattern();
    void UpdateAsterisk(int from, int to);
    void UpdateOnOffColour();
    void UpdateShowChord();
    void UpdateAddMode();
    void UpdateController();

    /**Used to select sequencer that has to be displayed by the SequencerWidget*/
    void SelectSeq(seqHandle h);
    void SelectNothing();

    /**Inits the notebook*/
    void InitNotebook();
    
    bool AnythingSelected;
    seqHandle selectedSeq;
    SeqType_t selectedSeqType;

private:
    /*
    void AttachLines(int where);
    void DetachLines();
    */
    void UpdatePatternWidget(int pattern=-1);
    /**This comment is absolutelly unnecessary, the function name speaks by itself.*/
    void HideAndShowWidgetsDependingOnSeqType();

    enum OnOffColour{
        NONE,
        ON,
        ONCE_PRE,
        ONCE
    };
    void SetOnOffColour(OnOffColour i);


    ChordWidget chordwidget;

    bool ignore_signals;
    int do_not_react_on_page_changes;
    void UpdateActivePatternRange();
    void OnChordWidgetChanged();
    void OnChordWidgetNoteChanged(int n, int p);
    void OnChannelChanged();
    void OnVelocityChanged();
    void OnValueChanged();
    void OnToggleMuteToggled();
    void OnResolutionChanged();
    void OnLengthChanged();
    void OnActivePatternChanged();
    void OnSetAsActivePatternClicked();
    void OnNotebookPageChanged(GtkNotebookPage* page, guint page_num);
    void OnAddPatternClicked();
    void OnRemovePatternClicked();
    void SetRemoveButtonSensitivity();
    void OnNameEdited();
    void OnPlayOnceButtonClicked();
    void OnClearPatternClicked();
    bool OnPatternMouseScroll(GdkEventScroll * e);
    void OnSnapClicked();
    void OnAddToggled();
    void OnDeleteClicked();
    void OnShowChordButtonClicked();
    void OnControllerChanged();
    /**Called when pattern's selection was changed and we need to display new velocity*/
    void OnSelectionChanged(int n);
    void LeaveAddMode();
    void OnSlopeFlatToggled();
    void OnSlopeLinearToggled();
    void UpdateSlopeType();
    
    PatternWidget pattern_widget;
    
    Gtk::VBox wMainVbox;
    Gtk::VBox wUpBox;
    Gtk::Table wDownTable;
    Gtk::HBox wUpperHBox1;
    Gtk::HBox wUpperHBox2;
    Gtk::HBox wNotebookAndPatternOpsHBox;
    Gtk::VBox wPtOpsVBox;
    Gtk::HBox wPtOpsHBox1;
    Gtk::HBox wPtOpsHBox2;
    Gtk::VBox wBoxOfChord;
    Gtk::Notebook wChordNotebook;
    Gtk::HBox wCtrlHBox;
    Gtk::VBox wCtrlScale;
    Gtk::Label wCtrl127;
    Gtk::Label wCtrl64;
    Gtk::Label wCtrl0;
    Gtk::VBox wRightBox;
    Gtk::HBox wRightBox1;
    Gtk::HBox wRightBox2;
    Gtk::HBox wClearPatternHBox;
    Gtk::EventBox wOnOfColour;

    Gtk::HSeparator wHSep;
    Gtk::VSeparator wRightBoxSep;
    Gtk::VSeparator wChannelSep;
    Gtk::VSeparator wVelSep;
    Gtk::VSeparator wSlopeSelectorSep;

    Gtk::Notebook wNotebook;
    Gtk::VBox wNotebookVbox;
    std::vector<Gtk::Label *> notebook_pages;
    /**It allows the patternwidget to not fill the whole viewport area.*/
    Gtk::HBox wPatternWidgetBox;
    Gtk::Viewport* wViewport; //must be a pointer, as constructor needs agruments
    Gtk::HScrollbar wPatternScroll;
    Gtk::VScrollbar wPatternScroll2;
    Gtk::SpinButton wChannelButton;
    Gtk::SpinButton wVelocityButton;
    Gtk::SpinButton wValueButton;
    Gtk::SpinButton wActivePattern;
    Gtk::Button wSetAsActivePatternButton;
    Gtk::Label wChannelLabel;
    Gtk::Label wVelocityLabel;
    Gtk::Label wValueLabel;
    Gtk::Label wActivePanelLabel;
    Gtk::Label wPatternLabel;
    Gtk::Label wNameLabel;
    Gtk::Label wVirtualSpaceLabel;
    Gtk::Entry wNameEntry;
    Gtk::Label wControllerLabel;
    Gtk::SpinButton wControllerButton;
    Gtk::Button wAddPatternButton;
    Gtk::Button wPlayOnceButton;
    Gtk::Button wRemovePattern;
    Gtk::Button wClearPattern;
    Gtk::CheckButton wMuteToggle;
    Gtk::CheckButton wSnapToggle;
    Gtk::ToggleButton wAddToggle;
    Gtk::Button wDelete;
    Gtk::SpinButton wResolutions;
    Gtk::SpinButton wLengthNumerator;
    Gtk::Label wLengthDivision;
    Gtk::SpinButton wLengthDenominator;
    Gtk::Label wLengthResult;
    Gtk::Label wResolutionsLabel, wLengthsLabel;
    Gtk::ToggleButton wShowChordButton;
    Gtk::Label wShowChordLabel;

    Gtk::RadioButton wCtrlSlopeFlat;
    Gtk::RadioButton wCtrlSlopeLinear;
    //not displayed, but used to choose none of the other two
    Gtk::RadioButton wCtrlSlopeNone;
    
    Gtk::Image wImageAdd;
    Gtk::Image wImageRemove;
    Gtk::Image wImageSlopeLinear;
    Gtk::Image wImageSlopeFlat;\

};

#endif	/* SEQUENCERWIDGET_H */

