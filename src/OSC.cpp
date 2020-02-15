/*
    Copyright (C) 2011, 2020 Rafał Cieślak

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


#ifndef DISABLE_OSC

#include "OSC.hpp"

#include <lo/lo.h>

#include "Configuration.hpp"
#include "Event.hpp"
#include "MidiDriver.hpp"
#include "messages.hpp"
#include "shared.hpp"


extern MidiDriver* midi;

void error_handler(int num, const char *msg, const char *path){
    *err << "OSC error!\n";
}

int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data)
{
    *dbg << "OSC: unknown message got - path = " << path << ENDL;
    return 1;
}

int pause_handler(const char *path, const char *types, lo_arg **argv,
 		    int argc, void *data, void *user_data)
{
    *dbg << "OSC: pause signal got.\n";
    if (midi->GetPaused() == 0)
        midi->PauseImmediately();
    return 0;
}
int unpause_handler(const char *path, const char *types, lo_arg **argv,
 		    int argc, void *data, void *user_data)
{
    *dbg << "OSC: unpause signal got.\n";
    if (midi->GetPaused() == 1)
        midi->Unpause();
    return 0;
}

int tempo_handler(const char *path, const char *types, lo_arg **argv,
 		    int argc, void *data, void *user_data)
{
    *dbg << "OSC : tempo change got.\n";
    double tmp = argv[0]->f;
    midi->SetTempo(tmp);
    return 0;
}
int sync_handler(const char *path, const char *types, lo_arg **argv,
 		    int argc, void *data, void *user_data)
{
    *dbg << "OSC : sync signal got.\n";
    if (!midi->GetPaused()) //do not sync while in pause!
        midi->Sync();
    return 0;
}
int events_handler(const char *path, const char *types, lo_arg **argv,
 		    int argc, void *data, void *user_data)
{
    int TAG = argv[0]->i;
    *dbg << "OSC : event signal got, TAG = " << TAG << ".\n";
    // For now it is the UI thread that processes events.
    DeferWorkToUIThread(
        [=](){
            FindAndProcessEvents(Event::OSC,TAG,0);
            return false;
        });
    return 0;
}


lo_server_thread st;

void RunOSCThread(){
    char port[20];
    sprintf(port,"%d",Config::OSC::Port);
    /* TODO: Any chance we could manage the thread on our own for consistency? */
    st = lo_server_thread_new(port,error_handler);
    lo_server_thread_add_method(st,"/harmonyseq/event","i",events_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/triger","i",events_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/pause",NULL,pause_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/play",NULL,unpause_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/unpause",NULL,unpause_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/tempo","f",tempo_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/sync",NULL,sync_handler,NULL);
    lo_server_thread_add_method(st,"/harmonyseq/synchronize",NULL,sync_handler,NULL);
    lo_server_thread_add_method(st,NULL,NULL,generic_handler,NULL);
    lo_server_thread_start(st);
}

void StopOSCThead(){
    lo_server_thread_stop(st);
}

#endif /*DISABLE_OSC*/
