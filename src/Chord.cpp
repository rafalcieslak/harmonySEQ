/*
    Copyright (C) 2010 Rafał Cieślak

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


#include "Chord.h"

int STANDARD_GUITAR_TUNING[6] =
                                    {0,5,10,15,19,24};

int GUITAR_CHORDS_MAJOR[12][6] = {
    //here we go!
    {0,0,0,0,0,0}, //C
    {0,0,0,0,0,0}, //C#
    {0,0,0,0,0,0}, //D
    {0,0,0,0,0,0}, //D#
    {0,0,0,0,0,0}, //E
    {0,0,0,0,0,0}, //F
    {0,0,0,0,0,0}, //F#
    {0,0,0,0,0,0}, //G
    {0,0,0,0,0,0}, //G#
    {0,0,0,0,0,0}, //A
    {0,0,0,0,0,0}, //A#
    {0,0,0,0,0,0}  //H

};

int GUITAR_CHORDS_MINOR[12][6] = {
    //here we go!
    {0,0,0,0,0,0}, //C
    {0,0,0,0,0,0}, //C#
    {0,0,0,0,0,0}, //D
    {0,0,0,0,0,0}, //D#
    {0,0,0,0,0,0}, //E
    {0,0,0,0,0,0}, //F
    {0,0,0,0,0,0}, //F#
    {0,0,0,0,0,0}, //G
    {0,0,0,0,0,0}, //G#
    {0,0,0,0,0,0}, //A
    {0,0,0,0,0,0}, //A#
    {0,0,0,0,0,0}  //H

};

Chord::Chord(){
    mode = TRIAD;
    inversion = 0;
    root = 0;
    guitar_mode = GUITAR_MAJOR;
    type = TYPE_MAJOR;
    octave = 0;

}

Chord::~Chord(){
}

void Chord::RecalcNotes(){
    switch (mode){
        case CUSTOM:

            //well, in this case nothing is to be recalculated.
            break;
        case GUITAR:

            break;
        case TRIAD:

            break;
    }

}

int Chord::GetNote(int n){
    if (n > 5 || n < 0) return;
    return notes[n];

}

void Chord::SetNote(int note, int pitch){
    if (note > 5 || note < 0) return;
    mode = CUSTOM;
    notes[note] = pitch;

}