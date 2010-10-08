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



class Event {
public:
    Event();
    Event(int typ, int a1, int a2);
    Event(const Event& orig);
    virtual ~Event();

    int type;

    void Trigger();

    enum EventTypes{
        NONE = 0,        //no arguments
        KEYBOARD = 1,    //keyboard key code
        NOTE = 2,        //note pitch, channel [0 - all]
        CONTROLLER = 3   //ctrl num, channel [0 - all]
    };

    int arg1;
    int arg2;

    std::string GetLabel();
    void ShowWindow();
    void UpdateGUI();

    std::vector<Action*> actions;

    Gtk::TreeRowReference row_in_event_window;

    EventGUI *gui_window;
private:

};

void FindAndProcessEvents(Event::EventTypes ev,int arg1 = 0, int arg2 = 0);
bool FindAndProcessEventsKeyPress(GdkEventKey* event);
void ClearEvents();
#endif	/* EVENT_H */

