/* 
 * File:   debug.cpp
 * Author: cielak
 * 
 * Created on 19 sierpień 2010, 14:42
 */

#include "debug.h"

debug::debug(int debug) {
    if (debug != 0){
        printf("Debug mode started.\n");
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

debug & operator <<(debug &dbg, int number){
    if(!dbg.debug_mode) return dbg;
    char temp[20];
    sprintf(temp,"%d",number);
    dbg.say(temp);
    return dbg;
}
