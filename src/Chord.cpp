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

int GUITAR_TABS_MAJOR[12][6] = {
    //here we go!
    {0,3,2,0,1,0}, //C
    {4,4,6,6,6,4}, //C#
    {0,0,0,2,3,2}, //D
    {6,6,8,8,8,6}, //D#
    {0,2,2,1,0,0}, //E
    {1,3,3,2,1,1}, //F
    {2,4,4,3,2,2}, //F#
    {3,2,0,0,0,3}, //G
    {4,6,6,5,4,4}, //G#
    {0,0,2,2,2,0}, //A
    {1,1,3,3,3,1}, //A#
    {2,2,4,4,4,2}  //H

};

int GUITAR_TABS_MINOR[12][6] = {
    //here we go!
    {3,3,5,5,4,3}, //C
    {4,4,6,6,5,4}, //C#
    {0,0,0,2,3,1}, //D
    {6,6,8,8,7,6}, //D#
    {0,2,2,0,0,0}, //E
    {1,3,3,1,1,1}, //F
    {2,4,4,2,2,2}, //F#
    {3,5,5,3,3,3}, //G
    {4,6,6,4,4,4}, //G#
    {0,0,2,2,1,0}, //A
    {1,1,3,3,2,1}, //A#
    {2,2,4,4,3,2}  //H

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
    *dbg<<"recalculating chord\n";
    int n1,n2,n3,base;
    switch (mode){
        case CUSTOM:

            //well, in this case nothing is to be recalculated.
            break;
        case GUITAR:
            if (mode == GUITAR_MAJOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MAJOR[root][x];
            }else if (mode == GUITAR_MINOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MINOR[root][x];
            }
            break;
        case TRIAD:
            n1 = root;
            if (type == TYPE_MAJOR || type == TYPE_AUGMENTED) n2 = n1+4;
            else n2 = n1+3;
            if(type == TYPE_MINOR || type == TYPE_AUGMENTED) n3 = n2+4;
            else n3 = n2+3;

            base = 12*octave;

            //as for now, we ignore the inversion, will be implemented later.

            notes[0] = n1;
            notes[3] = n1+12;
            notes[1] = n2;
            notes[4] = n2+12;
            notes[2] = n3;
            notes[5] = n3+12;


            break;
    }

}

int Chord::GetNote(int n){
    if (n > 5 || n < 0) return 0;
    return notes[n];

}

void Chord::SetNote(int note, int pitch){
    if (note > 5 || note < 0) return;
    mode = CUSTOM;
    notes[note] = pitch;
    //RecalcNotes(); //well, in fact: not needed;
}

void Chord::SetRoot(int pitch){
    root = pitch%12;
    RecalcNotes();
}

int Chord::GetRoot(){
    return root;
}

void Chord::SetMode(int n){
    mode = n;
    RecalcNotes();

}

int Chord::GetMode(){
    return mode;
}

void Chord::SetType(int n){
    type = n;
    RecalcNotes();
}

int Chord::GetType(){
    return type;
}

void Chord::SetGuitarMode(int n){
    guitar_mode = n;
    RecalcNotes();
}

int Chord::GetGuitarMode(){
    return guitar_mode;
}

void Chord::SetInversion(int n){
    inversion = n;
    RecalcNotes();
}

int Chord::GetInversion(){
    return inversion;
}

void Chord::SetOctave(int n){
    octave = n;
    RecalcNotes();
}

int Chord::GetOctave(){
    return octave;
}


Chord& Chord::operator =(const Chord& other){
    mode = other.mode;
    octave = other.octave;
    root = other.root;
    type = other.type;
    guitar_mode = other.guitar_mode;
    inversion = other.inversion;
    RecalcNotes();
    return *this;
}

std::vector<int> Chord::GetNotesList(){
    std::vector<int> V;
    for (int x = 0; x < 6;x++)
        V.push_back(notes[x]);
    return V;
}