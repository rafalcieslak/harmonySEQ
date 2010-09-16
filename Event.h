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



class Event {
public:
    Event();
    Event(int typ, int a1, int a2);
    Event(const Event& orig);
    virtual ~Event();

    int type;

    void Trigger();

    enum EventTypes{
        EVENT_TYPE_NONE,        //no arguments
        EVENT_TYPE_KEYBOARD,    //keyboard key code
        EVENT_TYPE_NOTE,        //note pitch, channel [0 - all]
        EVENT_TYPE_CONTROLLER   //ctrl num, channel [0 - all]
    };

    int arg1;
    int arg2;

    std::string GetLabel();

    Gtk::TreeRowReference row_in_event_window;

    EventGUI *gui_window;
private:

};

void FindAndProcessEvents(Event::EventTypes ev,int arg1 = 0, int arg2 = 0);
#endif	/* EVENT_H */

