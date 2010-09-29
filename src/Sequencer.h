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


#ifndef SEQUENCER_H
#define	SEQUENCER_H
#include "global.h"
#include "SequencerGUI.h"
using namespace std;


Gtk::TreeModel::RowReference spawn_sequencer();
Gtk::TreeModel::RowReference clone_sequencer(int orig);
void clear_sequencers();

class Sequencer {
public:

    /**Sequencer constructor*/
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer* orig);
    Sequencer(int seq[],int notes[]);
    Sequencer(int seq[],int notes[], Glib::ustring _name);
    virtual ~Sequencer();

    /**List of sequence notes*/
    vector<int> sequence;

    /**List of notes*/
    vector<int> notes;
    
    /**Fills in everything with default values*/
    void Init();

    /**Stores the length of  sequence*/
    double length;

    /**Stores the resolution of sequence*/
    int resolution;

    /**Used to change resolution of this sequencer*/
    void SetResolution(int res);

    /**Returns a one note of this sequencer
     *  @parram n note number*/
    int GetNotes(int n);

    /**Returns a one note from sequence
     * @parram n sequence note tumber*/
    int GetSequence(int n);



    /**Sets the sequencer on/off*/
    void SetOn(bool m);

    /**Returns true if sequencer is on, or false otherwise*/
    bool GetOn();

    /**Renames the sequencer*/
    void SetName(Glib::ustring n);

    /**Returns current sequencer name*/
    Glib::ustring GetName();

    /**Sets flag ApplyMainNote*/
    void SetApplyMainNote(bool a);

    /**Returns flag ApplyMainNote*/
    bool GetApplyMainNote();

    /**Changes channel*/
    void SetChannel(int ch);

    /**Returns channel*/
    int GetChannel();
    
    /**Sets velocity*/
    void SetVolume(int v);
    /**Returns velocity*/
    int GetVolume();



    /**Shows corresponding SequencerWindow*/
    void ShowWindow();

    /**Updates values in corresponding SequencerWindow*/
    void UpdateGui();

    /**Updates note values in corresponding SequencerWindow*/
    void UpdateGuiNotes();


    /**Switches this sequencer on for one tact*/
    void PlayOnce();

    /**Returns whether this sequencer is played only one tact*/
    bool GetPlayedOnce();

    /**Called from MidiDriver when this sequencer is played once and it gets played, it safely switches the sequencer off*/
    void GotPlayedOnce();

    /**Remembers the last played note*/
    int last_played_note;

    /**Keeps a reference to a row in Sequencers TreeModel*/
    Gtk::TreeRowReference row_in_main_window;
    
    friend class SequencerWindow;
protected:
    int channel;
    bool apply_mainnote;
    bool on;
    int volume;
    Glib::ustring name;
    SequencerWindow *gui_window;
    bool played_once;
    
private:

};

#endif	/* SEQUENCER_H */

