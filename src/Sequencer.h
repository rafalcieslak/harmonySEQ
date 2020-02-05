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


#ifndef SEQUENCER_H
#define	SEQUENCER_H
#include "global.h"
#include <gtkmm.h>
#include "seqHandle.h"
#include "AtomContainer.h"

enum SeqType_t{
    SEQ_TYPE_NOTE,
    SEQ_TYPE_CONTROL
};

/**Spawns a new sequencer of given type.*/
Gtk::TreeModel::Row spawn_sequencer(SeqType_t type);
/**Spawns a new note sequencer*/
Gtk::TreeModel::Row spawn_note_sequencer();
/**Spawns a new control sequencer*/
Gtk::TreeModel::Row spawn_control_sequencer();
/**Spawns a new sequencer, identical to the one specified as arg*/
Gtk::TreeModel::Row clone_sequencer(int orig);
/**Clears the list of sequencers, removing every sequencer*/
void ClearSequencers();

/**Base, pure-virtual class for sequencers.*/
class Sequencer {
public:

    /**Sequencer constructor*/
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer& orig);
    virtual ~Sequencer();
    /**Virtual copy constructor.*/
    virtual Sequencer* Clone() = 0;

    /**List of  patterns.
     *    Each pattern is an AtomContainer, which basically
     *    is a list of notes */
    std::vector<AtomContainer> patterns;

    /**Appends a new pattern.*/
    int AddPattern();
    /**Removes selected pattern.*/
    bool RemovePattern(int x);

    /**Returns sequencer's type*/
    virtual SeqType_t GetType() = 0;

    /**Fills in everything with default values*/
    virtual void Init();

    /**Stores the resolution of sequence. Note it should be used only to store in file, the actual resolution is no more used - that's graphical only.*/
    int resolution;

    /**Used to change resolution of this sequencer*/
    void SetResolution(int res);

    /**Used to change this seq's length*/
    void SetLength(int numerator, int denominator);

    /**Returns seq's length*/
    double GetLength() volatile;
    int GetLengthNumerator();
    int GetLengthDenominator();

    void SetActivePatternNumber(int a);
    int GetActivePatternNumber();

    AtomContainer* GetActivePattern();

    /**Sets the sequencer on/off*/
    void SetOn(bool m);

    /**Returns true if sequencer is on, or false otherwise*/
    bool GetOn();

    /** Returns true if this sequencer is playing at the moment. This may be because it's on, or because it's in play-once mode. */
    bool IsPlaying();

    /** Used by the midi driver to mark whether this sequencer is being played or not. Updated only on each bar. */
    void SetPlaying(bool);

    /**Renames the sequencer*/
    void SetName(Glib::ustring n);

    /**Returns current sequencer name*/
    Glib::ustring GetName();

    /**Changes channel*/
    void SetChannel(int ch);

    /**Returns channel*/
    int GetChannel();

    int GetPlayOncePhase();
    void SetPlayOncePhase(int p);

    /**Clears the whole pattern*/
    void ClearPattern(int p);

    /**Remembers where to start playback in next bar.*/
    double play_from_here_marker;

    double playback_marker__start_pos = 0.0;
    double playback_marker__end_pos = 0.0;
    double playback_marker__start_time = 0.0;
    double playback_marker__end_time = 0.0;

    /**Keeps a reference to a row in Sequencers TreeModel*/
    Gtk::TreeRow my_row;

    /**Remembers the handle to this sequencer.*/
    seqHandle MyHandle;

protected:

    /**The active sequence number*/
    int active_pattern;

    /**Stores the length of  sequence*/
    int length_numerator;
    int length_denominator;

    int channel;

    /* Playing status and on status are different - turning a
     * sequencer on is merely a request to start playing it on the
     * next beat. Turning a sequencer off will stop playing on the
     * next bar. So in a way playing status lags after on status. User
     * requests to switch sequencer on/off, but the playing status is
     * what determines wheter playback animation takes place. */
    bool on;
    bool playing = false;

    Glib::ustring name;

    int play_once_phase;
private:

};

#endif	/* SEQUENCER_H */
