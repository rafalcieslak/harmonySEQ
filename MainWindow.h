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

#include <gtkmm.h>
#include <gtkmm/window.h>
#ifndef MAINWINDOW_H
#define	MAINWINDOW_H
class MainWindow: public Gtk::Window{


public:
    MainWindow();
    virtual ~MainWindow();

    //when user presses exit
    bool on_delete_event(GdkEventAny* event);
    
    //buttons functions
    void OnButtonAddClicked();
    void OnSaveClicked();
    void OnLoadClicked();
    void OnRemoveClicked();
    void OnCloneClicked();

    //called when user changes values from gui
    void MainNoteChanged();
    void TempoChanged();
    
    //called every tempo, starts the animation
    void FlashTempoStart();
    //ends the animation after timeout
    bool FlashTempoEnd();

    //clears the treemodel and inits a new, according to sequencers vector
    void InitTreeData();

    //adds a single row, when a new sequencer is spawned
    Gtk::TreeModel::RowReference AddSequencerRow(int n);

    //refreshes a single row, when some of it's settings had changed
    void RefreshRow(Gtk::TreeRowReference it);

    //reacts on double click on a row
    void OnTreeviewRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

    //reacts on settings changes from main window
    void OnMutedToggleToggled(const Glib::ustring& path);
    void OnApplyMainNoteToggleToggled(const Glib::ustring& path);
    void OnNameEdited(const Glib::ustring& path,const Glib::ustring& newtext);


    void OnPassToggleClicked();
    bool OnKeyPress(GdkEventKey* event);
    Gtk::VBox vbox1;
    Gtk::HBox hbox_up, hbox_down;
    Gtk::SpinButton main_note;
    Gtk::SpinButton tempo_button;
    Gtk::Label tempolabel, mainnotelabel;
    Gtk::Button button_add;
    Gtk::Button button_save;
    Gtk::Button button_open;
    Gtk::Button button_remove;
    Gtk::Button button_clone;
    Gtk::CheckButton pass_toggle;


private:

    class ModelColumns : public Gtk::TreeModel::ColumnRecord{
    public:
        ModelColumns(){add(col_ID);add(col_name);add(col_muted);add(col_apply_mainnote);add(col_channel);add(col_res);add(col_len);}
        Gtk::TreeModelColumn<int> col_ID;
        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<bool> col_muted;
        Gtk::TreeModelColumn<bool> col_apply_mainnote;
        Gtk::TreeModelColumn<int> col_channel;
        Gtk::TreeModelColumn<int> col_res;
        Gtk::TreeModelColumn<double> col_len;
    };

    ModelColumns m_columns;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

};


#endif	/* MAINWINDOW_H */

