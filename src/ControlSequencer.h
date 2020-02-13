/*
    Copyright (C) 2011 Rafał Cieślak

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
#include "Sequencer.h"
#include <glibmm/ustring.h>

#ifndef CONTROLSEQUENCER_H
#define	CONTROLSEQUENCER_H

/**A control sequencer, outputting MIDI control events*/
class ControlSequencer : public Sequencer{
public:
    ControlSequencer();
    ControlSequencer(Glib::ustring _name0);
    virtual ~ControlSequencer();
    /**Virtual copy constructor.*/
    virtual std::shared_ptr<Sequencer> Clone();

    virtual SeqType_t GetType();

    void SetControllerNumber(int number);
    int GetControllerNumber();
private:

    /**Stores the controller number (ie. 11 -> expression)*/
    int controller_number;

    /* Only used internally - use Clone() instead. */
    ControlSequencer(const ControlSequencer& orig);
};

#endif	/* CONTROLSEQUENCER_H */
