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

#include <valarray>

#include "NoteSequencer.h"

NoteSequencer::NoteSequencer()
                                                : Sequencer()
{
    Init();
}

NoteSequencer::NoteSequencer(const Sequencer& s)
                                                : Sequencer(s)
{
    Init();
}


NoteSequencer::NoteSequencer(Glib::ustring _name0)
                                                :Sequencer(_name0)
{
    Init();
}

void NoteSequencer::Init()
{
    expand_chord = 1;
    chord.SetType(Chord::CHORD_TYPE_TRIAD);
    chord.SetRoot(0);
    chord.SetTriadMode(Chord::CHORD_TRIAD_MODE_MAJOR);
    chord.SetBaseOctave(0);
    chord.SetInversion(0);

    chord.on_change.connect(
        [=](){ on_chord_change(); });
}

/*NoteSequencer::NoteSequencer(const NoteSequencer& orig)
    : Sequencer(orig)
{
    chord = orig.chord.copy();
    expand_chord = orig.expand_chord;
    }*/


NoteSequencer::~NoteSequencer(){
}

Sequencer* NoteSequencer::Clone(){
    NoteSequencer* seq = new NoteSequencer(*(Sequencer*)this);
    chord.CopyInto(seq->chord);
    return seq;
}

SeqType_t NoteSequencer::GetType(){
    return SEQ_TYPE_NOTE;
}

int NoteSequencer::GetNoteOfChord(int n){
    return chord.GetNotePlusBasenote(n);
}
