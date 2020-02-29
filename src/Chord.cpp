/*
    Copyright (C) 2010, 2020 Rafał Cieślak

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


#include "Chord.hpp"

#include <iostream>

#include "shared.hpp"

std::map<int, std::string> Chord::notemap = {
    {0, "C"},
    {1, "C#"},
    {2, "D"},
    {3, "D#"},
    {4, "E"},
    {5, "F"},
    {6, "F#"},
    {7, "G"},
    {8, "G#"},
    {9, "A"},
    {10, "A#"},
    {11, "H"},
};

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

int GUITAR_TABS_MAJ7[12][6] = {
    {0,3,2,0,0,0}, //C
    {0,5,4,2,2,2}, //C#
    {0,4,3,1,1,1}, //D
    {0,0,1,3,3,3}, //D#
    {0,2,1,1,0,0}, //E
    {1,0,2,2,1,0}, //F
    {0,0,4,3,2,1}, //F#
    {3,2,0,0,0,2}, //G
    {0,0,1,1,1,3}, //G#
    {0,0,2,1,2,0}, //A
    {0,1,3,2,3,1}, //A#
    {2,2,1,3,0,0}  //H
};
int GUITAR_TABS_M7[12][6] = {
    {0,0,1,3,1,3}, //C
    {0,4,2,1,0,0}, //C#
    {0,0,0,2,1,1}, //D
    {0,0,1,3,2,2}, //D#
    {0,2,2,0,3,0}, //E
    {1,3,3,1,4,1}, //F
    {2,0,2,2,2,0}, //F#
    {0,1,3,0,3,0}, //G
    {1,3,1,1,4,1}, //G#
    {0,0,2,0,1,0}, //A
    {0,1,3,1,2,1}, //A#
    {0,2,0,2,0,2}  //H
};

Chord::Chord(){
    type = CHORD_TYPE_TRIAD;
    inversion = 0;
    root = 0;
    mode_guitar = CHORD_GUITAR_MODE_MAJOR;
    mode_triad = CHORD_TRIAD_MODE_MAJOR;
    base_octave = 0;
    base_note=0;
    base_use = 1;
    NoteAndOctaveToBase();
    RecalcNotes();
}

Chord::~Chord(){
}


void Chord::CopyInto(Chord& c) const{
    c.type = type;
    c.inversion = inversion;
    c.root = root;
    c.mode_guitar = mode_guitar;
    c.mode_triad = mode_triad;
    c.base_octave = base_octave;
    c.base_note = base_note;
    c.base_use = base_use;
    for(int i = 0; i < 6; i++)
        c.notes[i] = notes[i];
    c.NoteAndOctaveToBase();
    c.RecalcNotes();
}


void Chord::RecalcNotes(){
    int n1,n2,n3;
    switch (type){
        case CHORD_TYPE_CUSTOM:

            //well, in this case nothing is to be recalculated.
            break;
        case CHORD_TYPE_GUITAR:
            if (mode_guitar == CHORD_GUITAR_MODE_MAJOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MAJOR[root][x];
            }else if (mode_guitar == CHORD_GUITAR_MODE_MINOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MINOR[root][x];
            }else if (mode_guitar == CHORD_GUITAR_MODE_MAJ7){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MAJ7[root][x];
            }else if (mode_guitar == CHORD_GUITAR_MODE_M7){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_M7[root][x];
            }
            break;
        case CHORD_TYPE_TRIAD:
            n1 =root;
            if (mode_triad == CHORD_TRIAD_MODE_MAJOR || mode_triad == CHORD_TRIAD_MODE_AUGMENTED) n2 = n1+4;
            else n2 = n1+3;
            if(mode_triad == CHORD_TRIAD_MODE_MINOR || mode_triad == CHORD_TRIAD_MODE_AUGMENTED) n3 = n2+4;
            else n3 = n2+3;



            if (inversion == 0) {
                notes[0] = n1;
                notes[1] = n2;
                notes[2] = n3;
                notes[3] = n1 + 12;
                notes[4] = n2 + 12;
                notes[5] = n3 + 12;
            }else if (inversion == 1){
                notes[0] = n3 - 12;
                notes[1] = n1;
                notes[2] = n2;
                notes[3] = n3;
                notes[4] = n1 + 12;
                notes[5] = n2 + 12;
            }else if (inversion == 2){
                notes[0] = n2 - 12;
                notes[1] = n3 - 12;
                notes[2] = n1;
                notes[3] = n2;
                notes[4] = n3;
                notes[5] = n1 + 12;
            }

            break;
    }

    on_change();
    on_base_changed(GetBase());
}

int Chord::GetNotePlusBasenote(int n){
    if (n > 5 || n < 0) return 0;
    if (base_use)
        return notes[n] + base;
    else
        return notes[n];
}

int Chord::GetNote(int n){
    if (n > 5 || n < 0) return 0;
    return notes[n];
}

void Chord::SetNote(int note, int pitch){
    if (note > 5 || note < 0) return;
    type = CHORD_TYPE_CUSTOM;
    notes[note] = pitch;
    //RecalcNotes(); //well, in fact: not needed;
    on_change();
}

void Chord::SetRoot(int pitch){
    root = pitch%12;
    RecalcNotes();
}

int Chord::GetRoot(){
    return root;
}


void Chord::SetType(int n){
    type = n;
    if(type == CHORD_TYPE_GUITAR || type == CHORD_TYPE_TRIAD) base_use =1;
    if(type == CHORD_TYPE_GUITAR) base_note = 4;
    if(type == CHORD_TYPE_TRIAD) base_note = 0;
    NoteAndOctaveToBase();
    RecalcNotes();

}

int Chord::GetType(){
    return type;
}

void Chord::SetTriadMode(int n){
    mode_triad = n;
    RecalcNotes();
}

int Chord::GetTriadMode(){
    return mode_triad;
}

void Chord::SetGuitarMode(int n){
    mode_guitar = n;
    RecalcNotes();
}

int Chord::GetGuitarMode(){
    return mode_guitar;
}

void Chord::SetInversion(int n){
    inversion = n;
    RecalcNotes();
}

int Chord::GetInversion(){
    return inversion;
}

void Chord::SetBaseOctave(int n){
    base_octave = n;
    if (base_octave > 5) base_octave =5;
    if (base_octave < -5) base_octave = -5;
    NoteAndOctaveToBase();
    RecalcNotes();
    on_base_changed(GetBase());
}

int Chord::GetBaseOctave(){
    return base_octave;
}

void Chord::SetBaseNote(int n){
    base_note = n;
    NoteAndOctaveToBase();
    RecalcNotes();
    on_base_changed(GetBase());
}
int Chord::GetBaseNote(){
    return base_note;
}

void Chord::SetBase(int n){
    base = n;
    BaseToOctaveAndNote();
    RecalcNotes();
    on_base_changed(GetBase());
}

int Chord::GetBase(){
    if(!base_use) return 0;
    return base;
}

void Chord::SetBaseUse(bool use){
    base_use = use;
    NoteAndOctaveToBase();
    on_base_changed(GetBase());
}

bool Chord::GetBaseUse(){
    return base_use;
}

void Chord::BaseToOctaveAndNote(){
    int oct = base/12;
    base_octave = oct-5;
    base_note = base - (base_octave+5)*12;
}

void Chord::NoteAndOctaveToBase(){
    base = (base_octave+5)*12+base_note;
}

void Chord::Set(const Chord& other,bool apply_octave){
    type = other.type;
    if (type == CHORD_TYPE_CUSTOM) for (int x = 0 ; x < 6; x++) notes[x] = other.notes[x];
    if (apply_octave) base_octave = other.base_octave;
    base_note = other.base_note;
    base_use = other.base_use;
    NoteAndOctaveToBase();
    mode_triad = other.mode_triad;
    mode_guitar = other.mode_guitar;
    root = other.root;
    inversion = other.inversion;
    RecalcNotes();
}


Glib::ustring Chord::GetName(bool do_not_use_octave){
    char temp[80];
    char temp2[102];
    char octave[20];
    bool use_octave = !do_not_use_octave;
    if(use_octave) sprintf(octave,_("octave: %d"),base_octave);
    Glib::ustring a;
    switch (type){
        case CHORD_TYPE_CUSTOM:
            if (base_use)
                sprintf(temp2,_("Custom: %d, %d, %d, %d, %d, %d (base: %d)"),notes[0],notes[1],notes[2],notes[3],notes[4],notes[5],base);
            else
                sprintf(temp2,_("Custom: %d, %d, %d, %d, %d, %d"),notes[0],notes[1],notes[2],notes[3],notes[4],notes[5]);
            break;
        case CHORD_TYPE_GUITAR:
            if (mode_guitar == CHORD_GUITAR_MODE_MAJOR){
                sprintf(temp,_("Guitar: %s maj"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_MINOR){
                sprintf(temp,_("Guitar: %s m"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_MAJ7){
                sprintf(temp,_("Guitar: %s maj7"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_M7){
                sprintf(temp,_("Guitar: %s m7"),notemap.find(root)->second.c_str());
            }
            if(use_octave)
                sprintf(temp2,"%s, %s",temp,octave);
            else
                sprintf(temp2,"%s",temp);
            break;
        case CHORD_TYPE_TRIAD:
            if (mode_triad ==  CHORD_TRIAD_MODE_MAJOR){
                if (inversion == 0)
                    sprintf(temp,_("Triad: %s major"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad: %s major, inversion: %d"),notemap.find(root)->second.c_str(),inversion);
            } else
            if (mode_triad == CHORD_TRIAD_MODE_MINOR){
                if (inversion == 0)
                    sprintf(temp,_("Triad: %s minor"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad: %s minor, inversion: %d"),notemap.find(root)->second.c_str(),inversion);
            } else
            if (mode_triad ==  CHORD_TRIAD_MODE_AUGMENTED){
                if (inversion == 0)
                    sprintf(temp,_("Triad: %s augmented"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad: %s augmented, inversion: %d"),notemap.find(root)->second.c_str(),inversion);
            } else
            if (mode_triad ==  CHORD_TRIAD_MODE_DIMINISCHED){
                if (inversion == 0)
                    sprintf(temp,_("Triad: %s diminisched"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad: %s diminisched, inversion: %d"),notemap.find(root)->second.c_str(),inversion);
            }
            if(use_octave)
                sprintf(temp2,"%s, %s",temp,octave);
            else
                sprintf(temp2,"%s",temp);
            break;
    }

    a = temp2;
    return a;

}


Glib::ustring Chord::GetSummary(bool do_not_use_octave){
    char temp[80];
    char temp2[102];
    char octave[20];
    bool use_octave = !do_not_use_octave;
    if(use_octave) sprintf(octave,_("o:%d"),base_octave);
    Glib::ustring a;
    switch (type){
        case CHORD_TYPE_CUSTOM:
            if (base_use)
                sprintf(temp2,_("Custom, base %d"),base);
            else
                sprintf(temp2,_("Custom"));
            break;
        case CHORD_TYPE_GUITAR:
            if (mode_guitar == CHORD_GUITAR_MODE_MAJOR){
                sprintf(temp,_("Guitar %s maj"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_MINOR){
                sprintf(temp,_("Guitar %s m"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_MAJ7){
                sprintf(temp,_("Guitar %s maj7"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_M7){
                sprintf(temp,_("Guitar %s m7"),notemap.find(root)->second.c_str());
            }
            if(use_octave)
                sprintf(temp2,"%s, %s",temp,octave);
            else
                sprintf(temp2,"%s",temp);
            break;
        case CHORD_TYPE_TRIAD:
            if (mode_triad ==  CHORD_TRIAD_MODE_MAJOR){
                if (inversion == 0)
                    sprintf(temp,_("Triad %s maj"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad %s maj, i:%d"),notemap.find(root)->second.c_str(),inversion);
            } else
            if (mode_triad == CHORD_TRIAD_MODE_MINOR){
                if (inversion == 0)
                    sprintf(temp,_("Triad %s min"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad %s min, i:%d"),notemap.find(root)->second.c_str(),inversion);
            } else
            if (mode_triad ==  CHORD_TRIAD_MODE_AUGMENTED){
                if (inversion == 0)
                    sprintf(temp,_("Triad %s aug"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad %s aug, i:%d"),notemap.find(root)->second.c_str(),inversion);
            } else
            if (mode_triad ==  CHORD_TRIAD_MODE_DIMINISCHED){
                if (inversion == 0)
                    sprintf(temp,_("Triad %s dim"),notemap.find(root)->second.c_str());
                else
                    sprintf(temp,_("Triad %s dim, i:%d"),notemap.find(root)->second.c_str(),inversion);
            }
            if(use_octave)
                sprintf(temp2,"%s, %s",temp,octave);
            else
                sprintf(temp2,"%s",temp);
            break;
    }

    a = temp2;
    return a;

}


std::vector<int> Chord::SaveToVector(){
   /**Should have following format: type, root, guitar_mode, triad_mode,inversion,base,use_base, notes(6)(if custom)*/
    std::vector<int> V;
    V.push_back(type);
    V.push_back(root);
    V.push_back(mode_guitar);
    V.push_back(mode_triad);
    V.push_back(inversion);
    V.push_back(base);
    V.push_back(base_use);
    if (type == CHORD_TYPE_CUSTOM) for (int x = 0; x < 6; x++) V.push_back(notes[x]);
    return V;
}

void Chord::SetFromVector(std::vector<int>& V){
    if (V.size() < 7) {
        std::cerr << "ERROR: chord vector too small\n";
        return;
    }
    type = V[0];
    root = V[1];
    mode_guitar = V[2];
    mode_triad = V[3];
    inversion = V[4];
    base = V[5];
    base_use = V[6];
    BaseToOctaveAndNote();
    if(type == CHORD_TYPE_CUSTOM)for (int x = 0; x < 6; x++) notes[x] = V[7+x];
    RecalcNotes();
}


   void Chord::SetFromVector_OLD_FILE_PRE_0_14(std::vector<int> &V){
   /**Old files have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom)*/
    if (V.size() < 7) {
        std::cerr << "ERROR: chord vector too small\n";
        return;
    }
    type = V[0];
    if (type == CHORD_TYPE_GUITAR) root = V[1];
    else root = V[3];

    mode_guitar = V[2];
    mode_triad = V[4];
    base_octave = V[5];
    NoteAndOctaveToBase();
    inversion = V[6];
    if(type == CHORD_TYPE_CUSTOM)for (int x = 0; x < 6; x++) notes[x] = V[7+x];
    else RecalcNotes();
   }
