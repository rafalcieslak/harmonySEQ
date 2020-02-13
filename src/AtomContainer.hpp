/*
    Copyright (C) 2011, 2012 Rafał Cieślak

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
#include "Atom.hpp"

class Sequencer;

class AtomContainer {
public:
    AtomContainer();
    AtomContainer(const AtomContainer& orig);
    virtual ~AtomContainer();

    /**Returns the number of atoms inside*/
    int GetSize();

    /**Adds an atom to the container*/
    int Add(Atom * a);

    /**Removes an atom from container*/
    void Remove(int n);
    void Remove(Atom* a);
    void RemoveList(std::set<Atom*,AtomComparingClass>* V);

    /**Gets n-th atom*/
    Atom* get(int n);
    Atom* operator[](int n);

    /**Removes all atoms from pattern.*/
    void Clear();

    /**Finds if there is and if is then where, note with given ID. Complexity linear.*/
    int FindID(int ID);

    /**Call if notes were moved or added.*/
    void Sort();

    AtomContainer & operator= (const AtomContainer & other);
private:
    std::vector<Atom *> AtmVec;

};

#endif	/* ATOMCONTAINER_H */
