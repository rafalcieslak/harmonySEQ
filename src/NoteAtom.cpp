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

#include "NoteAtom.hpp"

NoteAtom::NoteAtom(double _time, double _length, int _pitch)
                                        :  Atom(_time)
{
    length = _length;
    pitch = _pitch;
    velocity = 100;
}

NoteAtom::NoteAtom(const NoteAtom& orig)
                                        :Atom(orig.time){
    length = orig.length;
    pitch = orig.pitch;
    velocity = orig.velocity;
}

NoteAtom* NoteAtom::Clone(){
    return new NoteAtom(*this);

}


NoteAtom::~NoteAtom(){
}
