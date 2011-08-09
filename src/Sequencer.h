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
#include "AtomContainer.h"
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

    /**List of  patterns.
     *    Each pattern is an AtomContainer, which basically
     *    is a list of notes */
    vector<AtomContainer> patterns;

    int AddPattern();
    bool RemovePattern(int x);
    /**The main chord*/
   Chord chord;
    
    /**Fills in everything with default values*/
    void Init();

    /**Stores the resolution of sequence. Note it should be used only to store in file, the actual resolution is no more used - that's graphical.*/
    int resolution;

    /**Used to change resolution of this sequencer*/
    void SetResolution(int res);

    /**Used to change this seq's length*/
    void SetLength(double len);

    /**Returns seq's length*/
    double GetLength();

    void SetActivePatternNumber(int a);
    int GetActivePatternNumber();
    
    AtomContainer* GetActivePattern();
    
    /**Returns a one note of chord of this sequencer
     *  @parram n note number*/
    int GetNoteOfChord(int n);

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

    /**Changes the pattern that is played by this sequencer*/
    void ChangeActivePattern(int new_one);

    int GetPlayOncePhase();
    void SetPlayOncePhase(int p);

    /**Clears the whole pattern*/
    void ClearPattern(int p);

    /**Remembers where to start playback in next bar.*/
    double play_from_here_marker;

    /**Keeps a reference to a row in Sequencers TreeModel*/
    Gtk::TreeRow my_row;

    /**Remembers the handle to this sequencer.*/
    seqHandle MyHandle;

    bool expand_chord;
    
protected:

    /**The active sequence number*/
    int active_pattern;
    
    /**Stores the length of  sequence*/
    double length;

    int channel;
    bool on;
    Glib::ustring name;

    int play_once_phase;
private:

};

#endif	/* SEQUENCER_H */

