/* 
 * File:   debug.h
 * Author: cielak
 *
 * Created on 19 sierpień 2010, 14:42
 */

#ifndef DEBUG_H
#define	DEBUG_H
#include <cstdio>
#include <cstdlib>
class debug {
public:
    int debug_mode;
    debug(int debug);
    debug(const debug& orig);
    void say(const char* message);
    virtual ~debug();
private:

};

debug & operator <<(debug &dbg, const char* message);
debug & operator <<(debug &dbg, int number);
#endif	/* DEBUG_H */

