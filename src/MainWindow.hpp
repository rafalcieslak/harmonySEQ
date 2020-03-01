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

#ifndef MAINWINDOW_H
#define	MAINWINDOW_H

#include <boost/signals2.hpp>
namespace bs2 = boost::signals2;

#include <gtkmm.h>

#include "SequencerWidget.hpp"


class Sequencer;

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



    /** Called on every beat, animates tempo spinbutton */
    void FlashTempo();

    /**Sets title and icon for the play-pause button*/
    void UpdatePlayPauseTool();

    Gtk::SpinButton tempo_button;

    void UpdatePassMidiToggle();

    Gtk::TreeModel::iterator GetSelectedSequencerIter();
    std::shared_ptr<Sequencer> GetSelectedSequencer();

    SequencerWidget seqWidget;
    EventsWidget eventsWidget;

    /** Emitted when user confirms application quit. */
    bs2::signal<void()> on_quit_request;

private:
    /**Adds a single row, when a new sequencer is spawned.*/
    Gtk::TreeModel::Row AddSequencerRow(std::shared_ptr<Sequencer> seq);
    void AddSequencer(std::shared_ptr<Sequencer> seq);

     /**Reacts on sequencer settings changes from main window*/
    void OnMutedToggleToggled(const Glib::ustring& path);
    void OnNameEdited(const Glib::ustring& path,const Glib::ustring& newtext);

    void OnAddNoteSeqClicked();
    void OnAddControlSeqClicked();
    void OnRemoveClicked();
    void OnCloneClicked();
    void OnPlayPauseClicked();
    void OnSelectionChanged();
    void OnPassToggleClicked();
    bool OnKeyPress(GdkEventKey* event);
    bool OnKeyRelease(GdkEventKey* event);

    /**Called when user changed tempo*/
    void TempoChanged();
    void UpdateTempo();

    bool OnTreviewButtonPress(GdkEventButton* event);

    void OnAboutMenuClicked();
    void OnMenuQuitClicked();
    void OnMenuSaveClicked();
    void OnMenuSaveAsClicked();
    void OnMenuOpenClicked();
    void OnMenuNewClicked();
    void OnPreferencesClicked();
    void OnMIDIClockToggled();
    void OnSyncClicked();
    void OnMetronomeToggleClicked();
    void OnTapTempoClicked();

    void OnPopupPlayOnce();
    void OnPopupRemove();
    void OnPopupDuplicate();

    void OnDiodeEvent(DiodeMidiEvent dev);

    Gtk::TreeView wTreeView;

  Glib::RefPtr<Gtk::UIManager> m_refUIManager;
  Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;

  Gtk::ScrolledWindow wScrolledWindow;

    Gtk::CheckButton midi_clock_button;
    Gtk::Button sync_button;
    Gtk::Button tap_button;

    Gtk::VBox wMainVBox;
    Gtk::VBox wVBox1;

    Gtk::Frame wFrame;
    Gtk::Notebook wFrameNotebook;
    Gtk::Label wNoSeqSelected;

    Gtk::HPaned wHPaned;

    Gtk::Menu* wPopupMenu;

    Gtk::Label wTempoLabel;
    Gtk::Image metronometool_icon;
    Gtk::Image add_ctrl_seq_icon;
    Gtk::Image add_note_seq_icon;
};


#endif	/* MAINWINDOW_H */
