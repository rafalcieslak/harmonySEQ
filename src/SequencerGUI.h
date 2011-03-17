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


#ifndef SEQUENCERGUI_H
#define	SEQUENCERGUI_H
#include "global.h"

#include <gtkmm.h>
#include <vector>
class Sequencer;
class ChordWidget;

/**A widget displaying a horisontal line with 6 CheckBoxes.*/
class PatternLine : public Gtk::HBox{
public:
    PatternLine();
    ~PatternLine();

    /**Sets a button to a given value.*/
    void SetButton(int c, bool value);
    /**Returns the state of a button.*/
    bool GetButton(int c);
    /**Signal emited when one fo the buttons is clicked.*/
    sigc::signal<void,int,bool> OnButtonClicked;
private:
    void OnButtonsToggled(int c);
    std::vector<Gtk::CheckButton *> buttons;

};

class SequencerWindow : public Gtk::Window {
public:
    SequencerWindow(Sequencer* prt);
    virtual ~SequencerWindow();
    /**Updates all values, according to data in the parent sequencer*/
    void UpdateValues();

    /**Just like UpdateValues, but updates only the Chordwidget*/
    void UpdateChord();

    /**Sets active pattern to match the one from sequencer*/
    void UpdatePattern();
    
    /**Inits the notebook*/
    void InitNotebook();

    /**When pages are added or removed etc, the boxes where sliders are stored are removed, and after then they are brougth back again. To make it save to remove these boxes, we must first unpack sliders from them, and then add them back to the appropierate box.*/
    void DetachLines();
    void AttachLines(int where);
    
    /**The chordwidget of this GUI*/
    ChordWidget* chordwidget;

    /**Sequencer associated with this GUI, used to set sequencer's data according to values in GUI widgets*/
    Sequencer *parent;

    friend class Sequencer;
    
private:
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
    Gtk::VBox wMainVbox;
    Gtk::Notebook wNotebook;
    Gtk::VBox wBoxOfSliders;
    Gtk::HBox wBoxOfChord;
    Gtk::HBox wUpperBox;
    std::vector<PatternLine  *> pattern_lines;
    std::vector<Gtk::VBox *> pattern_boxes;
    Gtk::SpinButton wChannelButton;
    Gtk::SpinButton wVolumeButton;
    Gtk::SpinButton wActivePattern;
    Gtk::Button wSetAsActivePatternButton;
    Gtk::Label wChannelLabel;
    Gtk::Label wVolumeLabel;
    Gtk::Label wActivePanelLabel;
    Gtk::Label wPatternLabel;
    Gtk::HBox wPatternOpsHbox;
    Gtk::Button wAddPatternButton;
    Gtk::Button wRemovePattern;
    Gtk::HBox wLowerHBox;
    Gtk::HBox wLineOne;
    Gtk::HBox wLineTwo;
    Gtk::HBox wLineZero;
    Gtk::VBox wSpinnersVBox;
    Gtk::VBox wToggleVBox;
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



#endif	/* SEQUENCERGUI_H */

