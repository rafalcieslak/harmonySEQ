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

#include <gtkmm.h>

#include "global.hpp"

class Action;
class Event;

extern bool event_capturing_mode;
extern Event* event_to_capture_to;

extern std::vector<Event *> Events;

class Event {
public:
    Event();
    Event(int typ, int a1, int a2);
    virtual ~Event();

    enum EventTypes{
        NONE = 0,        //no arguments
        KEYBOARD = 1,    //keyboard key code
        NOTE = 2,        //note pitch, channel [0 - all]
        CONTROLLER = 3,  //ctrl num, channel [0 - all]
        OSC = 4,         //tag
    };

    int type;
    int arg1;
    int arg2;


    /**Triggers this events actions*/
    void Trigger();

    /**Generates a label describing this event*/
    std::string GetLabel();

    /**Actions list*/
    std::vector<Action*> actions;

    /** This signal is emitted by the events thread (or the UI's
     * thread, if the events thread hasn't been implemented yet). It's
     * up to the subscriber to relay work to a different thread. */
    bs2::signal<void()> on_trigger;

    /** This signal can be emitted by any thread. */
    bs2::signal<void()> on_changed;
};

/**Magic function that triggers all events that are of appropriate type and args (as specified in function arguments)*/
void FindAndProcessEvents(Event::EventTypes ev,int arg1 = 0, int arg2 = 0);

/**Triggers all key-press events, using GdkKeyEvent as argument, very useful when capturing keypresses from widgets*/
bool FindAndProcessEventsKeyPress(GdkEventKey* event);

/**Removes all events, clears whole list*/
void ClearEvents();

/** Triggered when events list is rebuilt. This is used by some GUI
 * elements to rebuild their list of events. This signal will be moved
 * to event manger class.*/
extern bs2::signal<void()> on_events_list_changed;

#endif	/* EVENT_H */
