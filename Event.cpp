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

#include "Event.h"
#include "EventsWindow.h"
#include "global.h"
Event::Event(){
    type = 0;
    arg1 = 0;
    arg2 = 0;
}

Event::Event(int typ, int a1, int a2){
    type = typ;
    arg1 = a1;
    arg2 = a2;
}


Event::Event(const Event& orig){
}


Event::~Event(){
}

std::string Event::GetLabel(){
    std::string a;
    char temp[15];
    a += "This is ";
    switch(type){
        case EVENT_TYPE_NOTE:
            a += "a note ";
            sprintf(temp,"%d",arg1);
            a += temp;
            a += " event";
            break;
        case EVENT_TYPE_KEYBOARD:
            a += "a ";
            a += keymap_itos.find(arg1)->second;
            a += " key event";

            break;
        case EVENT_TYPE_NONE:
            a += "an empty event";

            break;
        case EVENT_TYPE_CONTROLLER:
            a += "a controller ";
            sprintf(temp,"%d",arg1);
            a += temp;
            a += " event";

            break;

    }
    a+=".";
    return a;
}


void Event::Trigger(){
    *dbg << "triggered event ==";
    *dbg << GetLabel() << "== :-)\n";
    eventswindow->ColorizeRow(row_in_event_window);


}


void FindAndProcessEvents(Event::EventTypes ev,int arg1, int arg2){
    for (unsigned int x = 0; x < events.size();x++){
        if (events[x]==NULL) continue;
        if (events[x]->type == ev){
            switch (ev){
                case Event::EVENT_TYPE_KEYBOARD: //args: keycode
                    if (arg1 == events[x]->arg1) //checking whether key is correct
                        events[x]->Trigger();
                    break;
                case Event::EVENT_TYPE_NOTE:    //args: note channel
                    if (arg1 == events[x]->arg1 && (events[x]->arg2 == 0 || events[x]->arg2 == arg2))
                        events[x]->Trigger();
                    break;
                case Event::EVENT_TYPE_CONTROLLER: //args: controller channel
                    if (arg1 == events[x]->arg1 && (events[x]->arg2 == 0 || events[x]->arg2 == arg2))
                        events[x]->Trigger();

                    break;
                case Event::EVENT_TYPE_NONE:
                    *err << _("Error - empty event would be triggered.\n");
            }

        }
    }


}



void TriggerEvent(int number){
    if (events[number]==NULL)return;
    
    events[number]->Trigger();


}