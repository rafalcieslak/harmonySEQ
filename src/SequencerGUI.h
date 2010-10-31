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

//#include "ChordWidget.h"
#include <gtkmm.h>
#include <vector>
//#include "Sequencer.h"
class Sequencer;
class ChordWidget;

class SequencerWindow : public Gtk::Window {
public:
    SequencerWindow(Sequencer* prt);
    virtual ~SequencerWindow();
    /**Updates all values, according to data in the parent sequencer*/
    void UpdateValues();

    /**Just like UpdateValues, but updates only the Chordwidget*/
    void UpdateChord();

    /**Inits the notebook*/
    void InitNotebook();
    
    /**Inits the sliders, packs then on active notebook page, sets them apropierate values*/
    void InitMelodySliders();

    /**When pages are added or removed etc, the boxes where sliders are stored are removed, and after then they are brougth back again. To make it save to remove these boxes, we must first unpack sliders from them, and then add them back to the appropierate box.*/
    void DetachSliders();
    void ReattachSliders();
    
    /**The chordwidget of this GUI*/
    ChordWidget* chordwidget;

    /**Sequencer associated with this GUI, used to set sequencer's data according to values in GUI widgets*/
    Sequencer *parent;

    friend class Sequencer;
    
private:
    int previous_box_where_sliders_were_packed;
    int do_not_react_on_page_changes;
    void UpdateActiveMelodyRange();

    void OnChordWidgetChanged();
    void OnMelodyNoteChanged(int seq);
    void OnChannelChanged();
    void OnVolumeChanged();
    void OnToggleMuteToggled();
    void OnToggleApplyMainNoteToggled();
    void OnResolutionChanged();
    void OnLengthChanged();
    void OnActiveMelodyChanged();
    void OnSetAsActiveMelodyClicked();
    void OnNotebookPageChanged(GtkNotebookPage* page, guint page_num);
    Gtk::VBox main_vbox;
    Gtk::Notebook notebook;
    Gtk::VBox box_of_sliders;
    Gtk::HBox box_of_chord;
    Gtk::HBox upper_box;
    std::vector<Gtk::HScale *> melody_scales;
    std::vector<Gtk::VBox  *> melody_boxes;
    Gtk::SpinButton channel_button;
    Gtk::SpinButton volume_button;
    Gtk::SpinButton active_melody;
    Gtk::Button set_as_active_melody;
    Gtk::Label channellabel;
    Gtk::Label volumelabel;
    Gtk::Label activemelodylabel;
    Gtk::HBox low_hbox;
    Gtk::HBox line_one;
    Gtk::HBox line_two;
    Gtk::HBox line_zero;
    Gtk::VBox spinners_vbox;
    Gtk::VBox toggle_vbox;
    Gtk::CheckButton tgl_apply_mainnote, tgl_mute;
    Gtk::ComboBox resolution_box;
    Gtk::ComboBox length_box;
    Gtk::Label reslabel, lenlabel;

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

