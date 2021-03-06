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

#include "Atom.hpp"

bool AtomComparison(Atom * a1, Atom * a2)
{
    if( (a1->time)==(a2->time)){
        return (a1->ID)<(a2->ID);
    }else //just:
    return (a1->time)<(a2->time);
}

//Needed by std::set
bool AtomComparingClass::operator ()(Atom* lhs, Atom* rhs) const{
    return AtomComparison(lhs, rhs);
}


long int Atom::counter = 0;

Atom::Atom(double _time){
    time = _time;
    drag_offset_line = 0;
    drag_offset_time = 0;
    drag_offset_value = 0;
    ID = counter;
    counter++;
}

Atom::Atom(const Atom&  orig){
    time = orig.time;
    drag_offset_line = 0;
    drag_offset_time = 0;
    drag_offset_value = 0;
    ID = counter;
    counter++;
}

Atom::~Atom(){

}
