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
#include "Chord.h"
#include "SequencerGUI.h"
using namespace std;

/**Spawns a new sequencer*/
Gtk::TreeModel::RowReference spawn_sequencer();
/**Spawns a new sequencer, identical to the one specified as arg*/
Gtk::TreeModel::RowReference clone_sequencer(int orig);
/**Clears the list of sequencers, removing every sequencer*/
void clear_sequencers();

class Sequencer {
public:

    /**Sequencer constructor*/
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer* orig);
    Sequencer(int seq[],int chord[]);
    Sequencer(int seq[],int chord[], Glib::ustring _name);
    virtual ~Sequencer();

    /**List of melody notes*/
    vector<vector<int> > melodies;
    /**The active sequence number*/
    int active_melody;

    int AddMelody();
    bool RemoveMelody(int x);
    /**The main chord*/
   Chord chord;
    
    /**Fills in everything with default values*/
    void Init();

    /**Stores the length of  sequence*/
    double length;

    /**Stores the resolution of sequence*/
    int resolution;

    /**Used to change resolution of this sequencer*/
    void SetResolution(int res);

    /**Returns a one note of chord of this sequencer
     *  @parram n note number*/
    int GetNoteOfChord(int n);

    /**Returns a one note from a given sequence
     * @parram n sequence note tumber*/
    int GetMelodyNote(int melody, int n);

    /**Returns a one note from the active sequence
     * @parram n sequence note tumber*/
    int GetActiveMelodyNote( int n);

    /**Sets a note in a given sequence*/
    void SetMelodyNote(int sequence, int n, int value);

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

    /**Changes the melody that is played by this sequencer*/
    void ChangeActiveMelody(int new_one);

    /**Shows corresponding SequencerWindow*/
    void ShowWindow();

    /**Updates values in corresponding SequencerWindow*/
    void UpdateGui();

    /**Updates note values in corresponding SequencerWindow*/
    void UpdateGuiChord();

    int GetPlayOncePhase();
    void SetPlayOncePhase(int p);
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

    int play_once_phase;
private:

};

#endif	/* SEQUENCER_H */

