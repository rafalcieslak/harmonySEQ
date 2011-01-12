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

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <gtkmm.h>
#include <gtkmm/window.h>
#include "global.h"
class MainWindow: public Gtk::Window{


public:
    MainWindow();
    virtual ~MainWindow();

    /**Sets the window title, filling it with filename*/
    void UpdateTitle();
    /**Called when user presses X in the titlebar, meaning he wants to exit the application*/
    bool on_delete_event(GdkEventAny* event);

    /**Clears the treemodel and inits a new, according to sequencers vector*/
    void InitTreeData();
    /**Refreshes a single row, called when some of it's settings had changed*/
    void RefreshRow(Gtk::TreeRowReference it);
    void RefreshRow(Gtk::TreeRow row);

    /**Adds a single row, when a new sequencer is spawned (and return a RowReference, so that the sequencer will know where is it's row)*/
    Gtk::TreeModel::RowReference AddSequencerRow(int n);
    
    /**Called when user changed tempo*/
    void TempoChanged();
    
    /**Called every tempo, starts the animation*/
    void FlashTempoStart();
    /**Ends the animation after timeout*/
    bool FlashTempoEnd();

    /**Sets title and icon for the play-pause button*/
    void UpdatePlayPauseTool();

    Gtk::SpinButton tempo_button;

    void UpdatePassMidiToggle();

    int GetSelectedSequencerID();
    Gtk::TreeModel::iterator GetSelectedSequencerIter();
private:

    /**Reacts on double click on a row*/
    void OnTreeviewRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
     /**Reacts on sequencer settings changes from main window*/
    void OnMutedToggleToggled(const Glib::ustring& path);
    void OnNameEdited(const Glib::ustring& path,const Glib::ustring& newtext);

    
    void OnAddSeqClicked();
    void OnRemoveClicked();
    void OnCloneClicked();
    void OnEventsClicked();
    void OnPlayPauseClicked();
    void OnSelectionChanged();
    void OnPassToggleClicked();
    bool OnKeyPress(GdkEventKey* event);

    bool OnTreviewButtonPress(GdkEventButton* event);

    void OnAboutMenuClicked();
    void OnMenuQuitClicked();
    void OnMenuSaveClicked();
    void OnMenuSaveAsClicked();
    void OnMenuOpenClicked();
    void OnMenuNewClicked();

    void OnPopupEdit();
    void OnPopupPlayOnce();
    void OnPopupRemove();
    void OnPopupDuplicate();

    Gtk::TreeView m_TreeView;

  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  Gtk::ScrolledWindow ScrolledWindow;

    Gtk::VBox main_vbox;
    Gtk::VBox vbox1;

    Gtk::Menu* popup_menu;

    Gtk::Label tempolabel;
    
};


#endif	/* MAINWINDOW_H */

