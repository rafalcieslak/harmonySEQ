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
#include "Sequencer.h"
#include "ChordWidget.h"
#include "PatternWidget.h"
class Sequencer;

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

class SequencerWidget : public Gtk::VBox{
public:
    SequencerWidget();
    virtual ~SequencerWidget();

    void UpdateEverything();
    void UpdateRelLenBoxes();
    void UpdateName();
    void UpdateOnOff();
    void UpdateChannel();
    void UpdateVelocity();
    void UpdateChord();
    void UpdateActivePattern();
    void UpdateAsterisk(int from, int to);
    void UpdateOnOffColour();

    void SelectSeq(seqHandle h);
    void SelectNothing();

    /**Inits the notebook*/
    void InitNotebook();
    
    bool AnythingSelected;
    seqHandle selectedSeq;

    /**Lights aproprieate diode, where colour is 0 when normal, or 1 when alternate*/
    void Diode(int n, int colour);

    /**Turns all diodes off*/
    void Diodes_AllOff();

private:
    /*
    void AttachLines(int where);
    void DetachLines();
    */
    void UpdatePatternWidget(int pattern=-1);

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
    void OnPatternNoteChanged(int c, bool value, int seq);
    void OnChannelChanged();
    void OnVelocityChanged();
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

    PatternWidget pattern_widget;
    
    Gtk::VBox wMainVbox;
    Gtk::HBox wUpBox;
    Gtk::HBox wDownBox;
    Gtk::HBox wNameBox;
    Gtk::HBox wOnBox;
    Gtk::VBox wUpperLeftBox;
    Gtk::VBox wUpperVBox;
    Gtk::HBox wUpperHBox1;
    Gtk::HBox wUpperHBox2;
    Gtk::HBox wNotebookAndPatternOpsHBox;
    Gtk::VBox wPtOpsVBox;
    Gtk::HBox wPtOpsHBox1;
    Gtk::HBox wPtOpsHBox2;
    Gtk::VBox wBoxOfChord;
    Gtk::HBox wClearPatternHBox;
    Gtk::EventBox wOnOfColour;

    Gtk::HSeparator wHSep;
    Gtk::VSeparator wVSep;

    Gtk::Notebook wNotebook;
    Gtk::VBox wNotebookVbox;
    //std::vector<PatternLine  *> pattern_lines;
    std::vector<Gtk::Label *> notebook_pages;
    //Gtk::HBox pattern_box;
    Gtk::Viewport* wViewport; //must be a pointer, as constructor needs agruments
    Gtk::HScrollbar wPatternScroll;
    Gtk::VScrollbar wPatternScroll2;
    std::vector<Gtk::VSeparator*> note_separators;
    Gtk::SpinButton wChannelButton;
    Gtk::SpinButton wVelocityButton;
    Gtk::SpinButton wActivePattern;
    Gtk::Button wSetAsActivePatternButton;
    Gtk::Label wChannelLabel;
    Gtk::Label wVelocityLabel;
    Gtk::Label wActivePanelLabel;
    Gtk::Label wPatternLabel;
    Gtk::Label wNameLabel;
    Gtk::Label wVirtualSpaceLabel;
    Gtk::Entry wNameEntry;
    Gtk::Button wAddPatternButton;
    Gtk::Button wPlayOnceButton;
    Gtk::Button wRemovePattern;
    Gtk::Button wClearPattern;
    Gtk::CheckButton wMuteToggle;
    Gtk::ComboBox wResolutionsBox;
    Gtk::ComboBox wLengthBox;
    Gtk::Label wResolutionsLabel, wLengthsLabel;

    /**ModelColums used for comboboxes*/
    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(resol);
        }
        Gtk::TreeModelColumn<int> resol;
    };
    ModelColumns m_Columns_resol;

    Glib::RefPtr<Gtk::ListStore> m_refTreeModel_res;

    class ModelColumns2 : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns2() {
            add(len); add(text);
        }
        Gtk::TreeModelColumn<double> len;
        Gtk::TreeModelColumn<Glib::ustring> text;
    };
    ModelColumns2 m_Columns_len;

    Glib::RefPtr<Gtk::ListStore> m_refTreeModel_len;

};

#endif	/* SEQUENCERWIDGET_H */

