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

AtomContainer::AtomContainer(){
    owner = NULL;
}

AtomContainer::AtomContainer(Sequencer* _owner){
    owner = _owner;
}

//Needed by std::vector
AtomContainer::AtomContainer(const AtomContainer& orig){
    owner = orig.owner;
    for(int x = 0; x < (int)orig.AtmVec.size(); x++){
        Atom* atm = orig.AtmVec[x];
        Atom* newatm = atm->Clone();
        AtmVec.push_back(newatm);
    }
}

AtomContainer::~AtomContainer(){
    Clear();
}

void AtomContainer::SetOwner(Sequencer* _owner){
    owner = _owner;
}

int AtomContainer::GetSize(){
    return AtmVec.size();
}

void AtomContainer::Clear(){
    for(int x = 0; x < (int)AtmVec.size(); x++) delete AtmVec[x];
    AtmVec.clear();
}

int AtomContainer::Add(Atom * a){
    AtmVec.push_back(a);
    Sort();
    return AtmVec.size()-1;
}

void AtomContainer::Remove(int n){
    delete AtmVec[n];
    AtmVec.erase(AtmVec.begin()+n);
    //No need to sort. Removing does not change order.
}

void AtomContainer::RemoveList(std::set<Atom*,AtomComparingClass>* V){
    std::vector<Atom*> result(1000);
    std::set<Atom*, AtomComparingClass> atoms = *V;
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


int AtomContainer::FindID(int ID){
    for(int x = 0; x < (int)AtmVec.size();x++)
        if(AtmVec[x]->ID == ID)
            return x;
    return -1;
}

Atom* AtomContainer::operator[](int n){
    return AtmVec[n];
}


AtomContainer & AtomContainer::operator= (const AtomContainer & other){
    if(this != &other){
        owner = other.owner;
        Clear();
        for(int x = 0; x < (int)other.AtmVec.size();x++){
            Atom* atm = other.AtmVec[x];
            Atom* newatm = atm->Clone();
            AtmVec.push_back(newatm);
        }
        
    }
    return *this;
}