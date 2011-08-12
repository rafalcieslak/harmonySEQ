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

#ifndef NOTESEQUENCER_H
#define	NOTESEQUENCER_H
#include "Sequencer.h"
#include <glibmm/ustring.h>

class NoteSequencer : public Sequencer {
public:
    NoteSequencer();
    NoteSequencer(Glib::ustring _name0);
    NoteSequencer(const NoteSequencer& orig);
    virtual ~NoteSequencer();
    virtual Sequencer* Clone(); //virtual copy constructor
    
    virtual SeqType_t GetType();
    
    /**The main chord*/
   Chord chord;
   
    bool expand_chord;
    
    /**Returns a one note of chord of this sequencer
     *  @parram n note number*/
    int GetNoteOfChord(int n);
    
private:

};

#endif	/* NOTESEQUENCER_H */

