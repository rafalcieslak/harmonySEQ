/*
    Copyright (C) 2011 Rafał Cieślak

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

#include "OSC.h"
#include "Configuration.h"
#include "messages.h"
extern error* err;

void error_handler(int num, const char *msg, const char *path){
    *err << "OSC error!\n";
}

int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data)
{
    *dbg << "OSC message got: path = " << path << ENDL;
    return 0;
}

void InitOSC(){
    //Not much to do yet.
    *dbg << "Initing OSC...\n";
    char port[20];
    sprintf(port,"%d",Config::OSC::Port);
    lo_server_thread st = lo_server_thread_new(port,error_handler);
    lo_server_thread_add_method(st,NULL,NULL,generic_handler,NULL);
    lo_server_thread_start(st);
}
 

#endif /*DISABLE_OSC*/

