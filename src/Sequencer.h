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
#include <gtkmm.h>
#include "Chord.h"
#include "seqHandle.h"
using namespace std;

/**Spawns a new sequencer*/
Gtk::TreeModel::Row spawn_sequencer();
/**Spawns a new sequencer, identical to the one specified as arg*/
Gtk::TreeModel::Row clone_sequencer(int orig);
/**Clears the list of sequencers, removing every sequencer*/
void ClearSequencers();

class Sequencer {
public:

    /**Sequencer constructor*/
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer* orig);
    virtual ~Sequencer();

    /**List of  notes in patterns:
        * In fact, it's a 3-dimentional array.
        * 1st dimention: pattern number.
        * 2nd dimention: note number (in time - vertical)
        * 3rd dimention: note number (pitch - horisontal)
        *
        * So: patterns[4][1][5] representa a boolan state of the first note (5th pith of chord) in fourth pattern.
        * Hard to explain, easy to use.*/
    vector<vector<vector<bool> > > patterns;
    
    /**The active sequence number*/
    int active_pattern;

    int AddPattern();
    bool RemovePattern(int x);
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
    int GetPatternNote(int pattern, int n, int c);

    /**Returns a one note from the active sequence
     * @parram n sequence note tumber*/
    bool GetActivePatternNote( int n, int c);

    /**Sets a note in a given sequence*/
    void SetPatternNote(int sequence, int n, int c, bool value);

    /**Sets the sequencer on/off*/
    void SetOn(bool m);

    /**Returns true if sequencer is on, or false otherwise*/
    bool GetOn();

    /**Renames the sequencer*/
    void SetName(Glib::ustring n);

    /**Returns current sequencer name*/
    Glib::ustring GetName();

    /**Changes channel*/
    void SetChannel(int ch);

    /**Returns channel*/
    int GetChannel();
    
    /**Sets velocity*/
    void SetVolume(int v);

    /**Returns velocity*/
    int GetVolume();

    /**Changes the pattern that is played by this sequencer*/
    void ChangeActivePattern(int new_one);

    int GetPlayOncePhase();
    void SetPlayOncePhase(int p);
    /**Remembers the last played note*/
    int last_played_note;

    /**Keeps a reference to a row in Sequencers TreeModel*/
    Gtk::TreeRow my_row;

    /**Remembers the handle to this sequencer.*/
    seqHandle MyHandle;
    
    friend class SequencerWindow;

protected:
    int channel;
    bool on;
    int volume;
    Glib::ustring name;

    int play_once_phase;
private:

};

#endif	/* SEQUENCER_H */

