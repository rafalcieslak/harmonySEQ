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
#include <set>
#include "messages.h"
#include "global.h"

bool AtomComparison(Atom * a1, Atom * a2)
{
    if( (a1->time)==(a2->time)){
        return (a1->ID)<(a2->ID);
    }else //just:
    return (a1->time)<(a2->time);
}

AtomContainer::AtomContainer(){
    owner = NULL;
}

AtomContainer::AtomContainer(Sequencer* _owner){
    owner = _owner;
}

AtomContainer::~AtomContainer(){
}

void AtomContainer::SetOwner(Sequencer* _owner){
    owner = _owner;
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

void AtomContainer::RemoveList(std::vector<Atom*>* V){
    int n = V->size();
    std::vector<Atom*> result(1000,NULL);
    std::vector<Atom*> atoms = *V;
    std::sort(atoms.begin(),atoms.end(),AtomComparison);
    std::set_difference(AtmVec.begin(),AtmVec.end(),atoms.begin(),atoms.end(),result.begin(),AtomComparison);
    AtmVec.clear();
    int x = 0;
    while(result[x] != NULL){
        AtmVec.push_back(result[x]);
        x++;
    }

    //No need to sort. Removing does not change order.
}

void AtomContainer::Sort(){
    std::sort(AtmVec.begin(),AtmVec.end(),AtomComparison);
}

Atom* AtomContainer::operator[](int n){
    return AtmVec[n];
}