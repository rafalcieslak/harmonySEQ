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
class Sequencer;

class PatternLine2 : public Gtk::VBox{
public:
    PatternLine2();
    PatternLine2(Glib::ustring mark);
    ~PatternLine2();

    /**Sets a button to a given value.*/
    void SetButton(int c, bool value);
    /**Returns the state of a button.*/
    bool GetButton(int c);
    /**Signal emited when one fo the buttons is clicked.*/
    sigc::signal<void,int,bool> OnButtonClicked;
private:
    void OnButtonsToggled(int c);
    std::vector<Gtk::CheckButton *> buttons;
    Gtk::Label marker;
};

class SequencerWidget : public Gtk::VBox{
public:
    SequencerWidget();
    virtual ~SequencerWidget();

    void UpdateEverything();
    void UpdateRelLenBoxes();
    void UpdateName();

    void SelectSeq(seqHandle h);
    void SelectNothing();

    /**Inits the notebook*/
    void InitNotebook();
    
    bool AnythingSelected;
    seqHandle selectedSeq;

private:
    void AttachLines(int where);
    void DetachLines();


    int previous_box_where_pattern_lines_were_packed;
    int do_not_react_on_page_changes;
    void UpdateActivePatternRange();
    void OnChordWidgetChanged();
    void OnChordWidgetNoteChanged(int n, int p);
    void OnPatternNoteChanged(int c, bool value, int seq);
    void OnChannelChanged();
    void OnVolumeChanged();
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

    Gtk::Table wMainTable;
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
    Gtk::HBox wBoxOfChord;

    Gtk::HSeparator wHSep;
    Gtk::VSeparator wVSep;

    Gtk::Notebook wNotebook;
    std::vector<PatternLine2  *> pattern_lines;
    std::vector<Gtk::HBox *> pattern_boxes;
    std::vector<Gtk::VSeparator*> note_separators;
    Gtk::SpinButton wChannelButton;
    Gtk::SpinButton wVolumeButton;
    Gtk::SpinButton wActivePattern;
    Gtk::Button wSetAsActivePatternButton;
    Gtk::Label wChannelLabel;
    Gtk::Label wVolumeLabel;
    Gtk::Label wActivePanelLabel;
    Gtk::Label wPatternLabel;
    Gtk::Label wNameLabel;
    Gtk::Entry wNameEntry;
    Gtk::Button wAddPatternButton;
    Gtk::Button wPlayOnceButton;
    Gtk::Button wRemovePattern;
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

