/*
    Copyright (C) 2010, 2020 Rafał Cieślak

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


#ifndef DEBUG_H
#define	DEBUG_H
#include <glibmm/ustring.h>
#include <cstdio>

/* The entire concept of this mechanism is completely unnecessary.
 */

class debug {
public:
    int debug_mode;
    debug(int debug);
    debug(const debug& orig);
    void say(const char* message);
    virtual ~debug();
private:

};


class error {
public:
    error();
    error(const error& orig);
    void say(const char* message);
    virtual ~error();
private:

};

error & operator <<(error &err, const char* message);
error & operator <<(error &err, int number);
error & operator <<(error &err, long int number);
error & operator <<(error &err, double number);
error & operator <<(error &dbg, Glib::ustring message);
debug & operator <<(debug &dbg, const char* message);
debug & operator <<(debug &dbg, long unsigned number);
debug & operator <<(debug &dbg, unsigned number);
debug & operator <<(debug &dbg, int number);
debug & operator <<(debug &dbg, double number);
debug & operator <<(debug &dbg, Glib::ustring message);
debug & operator <<(debug &dbg, std::string message);


class timer{
public:
    timer(bool start=1);
    void Start();
    double Stop();
    double Time();
private:
    clock_t starttime;
    double time;
};
#endif	/* DEBUG_H */
