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

#ifndef ACTION_H
#define	ACTION_H

//#include "global.h"
#include "ActionGUI.h"
#define ACTION_ARGS_NUM 20

class Action {
public:
    enum ActionTypes {
        NONE = 0,
        SEQ_OFF,
        SEQ_ON,
        SEQ_TOGGLE,
        SEQ_VOLUME_SET,
        MAINOTE_SET,
        TEMPO_SET,
        SEQ_CHANGE_ONE_NOTE,
        SEQ_CHANGE_ALL_NOTES,
        SEQ_PLAY_ONCE,
        TOGGLE_PASS_MIDI
    };

    Action(ActionTypes t, int a1 = 0, int a2 = 0);
    Action(const Action& orig);
    virtual ~Action();

    

    /**Keeps a reference to events TreeModel row (a child of an event). Useful f.e. for refreshing data in that row*/
    Gtk::TreeRowReference row_in_event_window;

    /**Stores the type of this action*/
    int type;

    /**Stores arguments, like sequencer number or note pitch, depending on type. */
    vector<int> args;

    /**Trigggers this action.
     * @parram data The data to pass*/
    void Trigger(int data = 0);

    /**Returns a label representing textual description of this message*/
    Glib::ustring GetLabel();

    /**Returns the name of sequencer. Should be moved from Action class to some global  space.
     *@parram n sequencer number*/
    Glib::ustring GetSeqName(int n);

    /**Shows the GUI*/
    void ShowWindow();

private:

    ActionGUI *gui_window;
};

#endif	/* ACTION_H */

