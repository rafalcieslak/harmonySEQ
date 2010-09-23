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
    gui_window = new EventGUI(this);
}

Event::Event(int typ, int a1, int a2){
    type = typ;
    arg1 = a1;
    arg2 = a2;
    gui_window = new EventGUI(this);
}


Event::Event(const Event& orig){
    for (unsigned int x = 0; x < actions.size(); x++){
        delete actions[x];

    }
}


Event::~Event(){
    delete gui_window;
}

std::string Event::GetLabel(){
    std::string a;
    char temp[100];
    switch(type){
        case NOTE:
            if(arg2 == 0) sprintf(temp,_("When note %d on any channel gets on..."),arg1);
            else sprintf(temp,_("When note %d on channel %d gets on..."),arg1,arg2);
            break;
        case KEYBOARD:
            sprintf(temp,_("When key %s is pressed..."),keymap_itos.find(arg1)->second.c_str());

            break;
        case NONE:
            sprintf(temp,_("This is an empty event, that will never be triggered."));

            break;
        case CONTROLLER:
            if(arg2 == 0) sprintf(temp,_("When controller %d on any channel is moved..."),arg1);
            else sprintf(temp,_("When controller %d on channel %d is moved..."),arg1,arg2);

            break;

    }
    a = temp;
    return a;
}


void Event::Trigger(){
    *dbg << "triggered event --'";
    *dbg << GetLabel() << "'--\n";
    eventswindow->ColorizeEvent(row_in_event_window);

    for (unsigned int i = 0; i < actions.size(); i++){
        if (!actions[i]) continue; //it was removed
        actions[i]->Trigger();

    }

}

void Event::ShowWindow(){

    gui_window->show();
}


void FindAndProcessEvents(Event::EventTypes ev,int arg1, int arg2){
    for (unsigned int x = 0; x < events.size();x++){
        if (events[x]==NULL) continue;
        if (events[x]->type == ev){
            switch (ev){
                case Event::KEYBOARD: //args: keycode
                    if (arg1 == events[x]->arg1) //checking whether key is correct
                        events[x]->Trigger();
                    break;
                case Event::NOTE:    //args: note channel
                    if (arg1 == events[x]->arg1 && (events[x]->arg2 == 0 || events[x]->arg2 == arg2))
                        events[x]->Trigger();
                    break;
                case Event::CONTROLLER: //args: controller channel
                    if (arg1 == events[x]->arg1 && (events[x]->arg2 == 0 || events[x]->arg2 == arg2))
                        events[x]->Trigger();

                    break;
                case Event::NONE:
                    *err << _("Error - empty event would be triggered.\n");
            }

        }
    }


}


bool FindAndProcessEventsKeyPress(GdkEventKey* event){
    //*dbg << "triggered " << event->keyval << "\n";
    std::map<int,string>::iterator iter;
    iter = keymap_itos.find(event->keyval);
    if(iter != keymap_itos.end()){
        *dbg << "Pressed key '" << iter->second << "'.\n";

    }else
        *dbg << "Unknown key pressed\n";

    FindAndProcessEvents(Event::KEYBOARD,event->keyval);

    return 1;
}
