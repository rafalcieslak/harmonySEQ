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


#ifndef ERROR_H
#define	ERROR_H
#include <cstdio>
#include <cstdlib>
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
#endif	/* ERROR_H */

