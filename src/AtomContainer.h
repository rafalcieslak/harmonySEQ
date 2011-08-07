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

#ifndef ATOMCONTAINER_H
#define	ATOMCONTAINER_H
#include <set>
#include <vector>
#include "Atom.h"

class Sequencer;

//Used for sorting atoms. Compares it's time.
bool AtomComparison(Atom * a1, Atom * a2);

class AtomContainer {
public:
    AtomContainer(Sequencer* parent);
    AtomContainer();
    virtual ~AtomContainer();
    
    /**Returns the number of atoms inside*/
    int GetSize();
    
    /**Adds an atom to the container*/
    int Add(Atom * a);
    
    /**Removes an atom from container*/
    void Remove(int n);
    void RemoveList(std::set<Atom*>* V);
    
    /**Gets n-th atom*/
    Atom* operator[](int n);
    
    /**Removes all atoms from pattern.*/
    void Clear();
    
    /**Tell the container who's its owner.*/
    void SetOwner(Sequencer* owner);
    
    /**The sequencer this container belongs to, if known.*/
    Sequencer* owner;
    
    /**Finds if there is and if is then where, note with given ID. Complexity linear.*/
    int FindID(int ID);
    
    /**Call if notes were moved or added.*/
    void Sort();
private:
    std::vector<Atom *> AtmVec;
    
};

#endif	/* ATOMCONTAINER_H */

