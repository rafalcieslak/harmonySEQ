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

#ifndef EVENT_H
#define	EVENT_H
#include "global.h"
#include "EventGUI.h"
#include "Action.h"
class Event;

extern bool event_guessing_mode;
extern Event* event_to_guess_to;

class Event {
public:
    Event();
    Event(int typ, int a1, int a2);
    virtual ~Event();

    enum EventTypes{
        NONE = 0,        //no arguments
        KEYBOARD = 1,    //keyboard key code
        NOTE = 2,        //note pitch, channel [0 - all]
        CONTROLLER = 3   //ctrl num, channel [0 - all]
    };

    int type;
    int arg1;
    int arg2;


    /**Triggers this events actions*/
    void Trigger();

    /**Generates a label describing this event*/
    std::string GetLabel();

    /**Shows this event GUI window*/
    void ShowWindow();

    /**Updates the GUI*/
    void UpdateGUI();

    /**Actions list*/
    std::vector<Action*> actions;

    /**Reference to treerow in events window*/
    Gtk::TreeRowReference row_in_event_window;

private:

    EventGUI *gui_window;
};
/**Magic function that triggers all events that are of appropriate type and args (as specified in function arguments)*/
void FindAndProcessEvents(Event::EventTypes ev,int arg1 = 0, int arg2 = 0);

/**Triggers all key-press events, using GdkKeyEvent as argument, very useful when capturing keypresses from widgets*/
bool FindAndProcessEventsKeyPress(GdkEventKey* event);

/**Removes all events, clears whole list*/
void ClearEvents();
#endif	/* EVENT_H */

