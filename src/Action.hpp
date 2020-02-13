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

#ifndef ACTION_H
#define	ACTION_H

#include <gtkmm.h>

#include "global.hpp"
#include "Chord.hpp"

class ActionGUI;

/**Number of arguments each action has*/
#define ACTION_ARGS_NUM 20

/**Item representing one acition.*/
class Action {
public:
    enum ActionTypes {
        NONE = 0,

        /* Note: Do not remove elements from this enum, as their IDs
         * are used in .hseq files. */

        /**1) seq 2) mode: 0 - off, 1 - on, 2 - toggle */
        SEQ_ON_OFF_TOGGLE,
        DEPRACATED_1,
        DEPRACATED_2,
        DEPRACATED_4,
        DEPRACATED_3,
        TEMPO_SET,
        SEQ_CHANGE_ONE_NOTE,
        /**1)seq 3) do not apply octave?*/
        SEQ_CHANGE_CHORD,
        SEQ_PLAY_ONCE,
        TOGGLE_PASS_MIDI,
        /**1)mode: 0- off, 1 - on, 2 - toggle*/
        PLAY_PAUSE,
        SYNC,
        SEQ_CHANGE_PATTERN,
        SEQ_TRANSPOSE_OCTAVE
    };

    /**Constructor.
     *@parram t Action's type
     *@parram a Initial arguments (only two, it's used rarely)*/
    Action(ActionTypes t, int a1 = 0, int a2 = 0);
    Action(const Action& orig);
    virtual ~Action();

    /**Stores the type of this action*/
    int type;

    /**Stores arguments, like sequencer number or note pitch, depending on type. */
    std::vector<int> args;

    /** Some actions store a sequencer reference. */
    std::weak_ptr<Sequencer> target_seq;

    /**Some actions take chord as an argument. Here it is stored.*/
    Chord chord;

    /**Trigggers this action.
     * @parram data The data to pass*/
    void Trigger(int data = 0);

    /**Returns a label representing textual description of this message*/
    Glib::ustring GetLabel();

    /**Returns the name of sequencer. Should be moved from Action class to some global  space.
     *@parram n sequencer number*/
    Glib::ustring GetSeqName(int n);

    /**Shows the GUI*/
    void GUIShowWindow();

    /** This signal is emitted by the events thread (or the UI's
     * thread, if the events thread hasn't been implemented yet). It's
     * up to the subscriber to relay work to a different thread. */
    bs2::signal<void()> on_trigger;

    /** This signal can be emitted by any thread. */
    bs2::signal<void()> on_changed;
};

#endif	/* ACTION_H */
