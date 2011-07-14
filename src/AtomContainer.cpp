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

#include "AtomContainer.h"
#include <algorithm>
#include <vector>

bool AtomComparison(Atom * a1, Atom * a2)
{
    return (a1->time)<(a2->time);
}

AtomContainer::AtomContainer(){
}

AtomContainer::~AtomContainer(){
}

void AtomContainer::SetParent(Sequencer* _parent){
    parent = _parent;
}

int AtomContainer::GetSize(){
    return AtmVec.size();
}

void AtomContainer::Clear(){
    AtmVec.clear();
}

int AtomContainer::Add(Atom * a){
    AtmVec.push_back(a);
    Sort();
}

void AtomContainer::Remove(int n){
    AtmVec.erase(AtmVec.begin()+n);
    //No need to sort. Removing does not change order.
}

void AtomContainer::Sort(){
    std::sort(AtmVec.begin(),AtmVec.end(),AtomComparison);
}

Atom* AtomContainer::operator[](int n){
    return AtmVec[n];
}