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

#include "global.h"

class Action {
public:

    enum ActionTypes {
        NONE = 0,
        SEQ_OFF,
        SEQ_ON,
        SEQ_TOGGLE,
        SEQ_VOLUME_SET,
        MAINOTE_SET,
        TEMPO_SET

    };

    Action(ActionTypes t, int a1 = 0, int a2 = 0);
    Action(const Action& orig);
    virtual ~Action();

    


    Gtk::TreeRowReference row_in_event_window;


    ActionTypes type;

    int arg1;
    int arg2;
    
    void Trigger(int data = 0);
    Glib::ustring GetLabel();

private:

};

#endif	/* ACTION_H */

