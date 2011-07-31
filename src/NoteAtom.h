/*
    Copyright (C)  2011 Rafał Cieślak

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

#ifndef NOTEATOM_H
#define	NOTEATOM_H

#include "Atom.h"

class NoteAtom : public Atom {
public:
    NoteAtom(double _time = 0, double _length = 0.125, int _pitch = 0);
    virtual ~NoteAtom();
    
    /**Currently - 0 to 5*/
    int pitch;
    
    /**Duration time, where 0 = none and 1 = full parent container*/
    double length;
    
    /**Velocity, from 0 to 127*/
    int velocity;
    
    /**Used to store/remember this note's length at drag's beggining*/
    double drag_beggining_length;
private:

};

#endif	/* NOTEATOM_H */

