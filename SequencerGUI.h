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


#ifndef SEQUENCERGUI_H
#define	SEQUENCERGUI_H
#include "global.h"
//#include "Sequencer.h"
class Sequencer;

class SequencerWindow : public Gtk::Window {
public:
    SequencerWindow(Sequencer* prt);
    virtual ~SequencerWindow();
    void UpdateValues();
    Gtk::VBox main_vbox;
    Gtk::VBox box_of_sliders;
    Gtk::HBox box_of_notes;
    Gtk::HBox upper_box;
    vector<Gtk::HScale *> sequence_scales;
    Gtk::SpinButton* note_buttons[6];
    Gtk::SpinButton channel_button;
    Gtk::Label channellabel;
    Gtk::HBox low_hbox;
    Gtk::VBox toggle_vbox;
    Gtk::CheckButton tgl_apply_mainnote, tgl_mute;
    Gtk::ComboBox resolution_box;
    Gtk::ComboBox length_box;
    Gtk::Label reslabel, lenlabel;
    void InitSeqSliders();


    Sequencer *parent;


private:
    void OnNotesChanged(int note);
    void OnSequenceChanged(int seq);
    void OnChannelChanged();
    void OnToggleMuteToggled();
    void OnToggleApplyMainNoteToggled();
    void OnResolutionChanged();
    void OnLengthChanged();

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

