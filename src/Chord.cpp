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

#include "messages.h"
extern debug *dbg;
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
    triad_root = 0;
    guitar_root = 0;
    guitar_mode = GUITAR_MAJOR;
    triad_mode = TYPE_MAJOR;
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
            if (guitar_mode == GUITAR_MAJOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MAJOR[guitar_root][x];
            }else if (guitar_mode == GUITAR_MINOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MINOR[guitar_root][x];
            }
            break;
        case TRIAD:
            base = 12*octave;
            n1 = base+triad_root;
            *dbg << "    base+triad_root  = " << n1 << ENDL;
            if (triad_mode == TYPE_MAJOR || triad_mode == TYPE_AUGMENTED) n2 = n1+4;
            else n2 = n1+3;
            if(triad_mode == TYPE_MINOR || triad_mode == TYPE_AUGMENTED) n3 = n2+4;
            else n3 = n2+3;


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

void Chord::SetTriadRoot(int pitch){
    triad_root = pitch%12;
    RecalcNotes();
}

int Chord::GetTriadRoot(){
    return triad_root;
}

void Chord::SetGuitarRoot(int pitch){
    guitar_root = pitch%12;
    RecalcNotes();
}

int Chord::GetGuitarRoot(){
    return guitar_root;
}

void Chord::SetMode(int n){
    mode = n;
    RecalcNotes();

}

int Chord::GetMode(){
    return mode;
}

void Chord::SetTriadMode(int n){
    triad_mode = n;
    RecalcNotes();
}

int Chord::GetTriadMode(){
    return triad_mode;
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

void Chord::Set(const Chord& other){
    *dbg << "copying chord." << ENDL;
    mode = other.mode;
    if (mode == CUSTOM) for (int x = 0 ; x < 6; x++) notes[x] = other.notes[x];
    octave = other.octave;
    triad_root = other.triad_root;
    triad_mode = other.triad_mode;
    guitar_mode = other.guitar_mode;
    guitar_root = other.guitar_root;
    inversion = other.inversion;
    RecalcNotes();
}


Glib::ustring Chord::GetName(){
    char temp[100];
    Glib::ustring a;
    switch (mode){
        case CUSTOM:
            sprintf(temp,_("Custom: %d, %d, %d, %d, %d, %d"),notes[0],notes[1],notes[2],notes[3],notes[4],notes[5]);
            break;
        case GUITAR:
            if (guitar_mode == GUITAR_MAJOR){
                sprintf(temp,_("Guitar: %s major"),notemap.find(guitar_root)->second.c_str());
            }else if (guitar_mode == GUITAR_MINOR){
                sprintf(temp,_("Guitar: %s minor"),notemap.find(guitar_root)->second.c_str());
            }
            break;
        case TRIAD:
            if (triad_mode ==  TYPE_MAJOR){
                sprintf(temp,_("Triad: %s major"),notemap.find(triad_root)->second.c_str());
            } else
            if (triad_mode == TYPE_MINOR){
                sprintf(temp,_("Triad: %s minor"),notemap.find(triad_root)->second.c_str());
            } else
            if (triad_mode ==  TYPE_AUGMENTED){
                sprintf(temp,_("Triad: %s augumented"),notemap.find(triad_root)->second.c_str());
            } else
            if (triad_mode ==  TYPE_DIMINICHED){
                sprintf(temp,_("Triad: %s diminished"),notemap.find(triad_root)->second.c_str());
            }
            break;
    }

    a = temp;
    return a;

}

std::vector<int> Chord::SaveToVector(){
   /**Should have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom)*/
    std::vector<int> V;
    V.push_back(mode);
    V.push_back(guitar_root);
    V.push_back(guitar_mode);
    V.push_back(triad_root);
    V.push_back(triad_mode);
    V.push_back(octave);
    V.push_back(inversion);
    if (mode == CUSTOM) for (int x = 0; x < 6; x++) V.push_back(notes[x]);
    return V;
}

void Chord::SetFromVector(std::vector<int>& V){
    if (V.size() < 7) {*err << "ERROR: chord vector too small\n"; return;}
    mode = V[0];
    guitar_root = V[1];
    guitar_mode = V[2];
    triad_root = V[3];
    *dbg << "setting chord settings from vector: triad root is " << V[3] << ENDL;
    triad_mode = V[4];
    octave = V[5];
    inversion = V[6];
    if(mode == CUSTOM)for (int x = 0; x < 6; x++) notes[x] = V[7+x];
}