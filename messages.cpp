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

#include "messages.h"

debug::debug(int debug) {
    if (debug != 0){
        printf(_("Debug mode started.\n"));
        debug_mode = 1;
    }else{
        debug_mode = 0;
    }
}

debug::debug(const debug& orig) {
}

debug::~debug() {
}

void debug::say(const char* message){
    if(!debug_mode) return;
    fputs(message,stdout);

}
debug & operator <<(debug &dbg, const char* message){
    //printf("debugmode = %d\n",dbg.debug_mode); /*debugging debug ^^ */
    dbg.say(message);
    return dbg;
}

debug & operator <<(debug &dbg, Glib::ustring message){

    dbg.say(message.c_str());
    return dbg;
}


debug & operator <<(debug &dbg, int number){
    if(!dbg.debug_mode) return dbg;
    char temp[20];
    sprintf(temp,"%d",number);
    dbg.say(temp);
    return dbg;
}
//================================================

error::error() {

}

error::error(const error& orig) {
}

error::~error() {
}

void error::say(const char* message){
    //fputs("ERROR - ",stdout);
    fputs(message,stdout);

}
error & operator <<(error &err, const char* message){
    err.say(message);
    return err;
}

error & operator <<(error &err, int number){
    char temp[20];
    sprintf(temp,"%d",number);
    err.say(temp);
    return err;
}
