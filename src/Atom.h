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
#ifndef ATOM_H
#define	ATOM_H

class Atom;

//Used for sorting atoms. Compares it's time.
bool AtomComparison(Atom * a1, Atom * a2);

//Needed by std::set
struct AtomComparingClass {
  bool operator() (Atom* lhs, Atom* rhs) const;
};

class Atom {
public:
    Atom(double _time);
    Atom(const Atom&  orig);
    virtual ~Atom();
    
    /**Returns a pointer to a new identical atom*/
    virtual Atom* Clone() = 0;
    
    /**Value from 0 to 1, representing relative location (in time) in parent container*/
    double time;
    
    /**Unique number used for sorting*/
    int ID;
    
    /**Used when dragging, to store relative position to drag beggining.*/
    double drag_offset_line,drag_offset_time, drag_offset_value;
    
    /**This is here just to force compiler to make Atom a base abstract class*/
   // virtual int bac() = 0;
private:
    static long int counter;

};

#endif	/* ATOM_H */

