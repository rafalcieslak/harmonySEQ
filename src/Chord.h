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


#ifndef CHORD_H
#define	CHORD_H
#include <vector>
#include <glibmm/ustring.h>
class Chord {
public:
    Chord();
    virtual ~Chord();



    enum {
        CUSTOM,
        TRIAD,
        GUITAR
    };

    enum {
        GUITAR_MAJOR,
        GUITAR_MINOR
    };
    enum {
        TYPE_MAJOR,
        TYPE_MINOR,
        TYPE_AUGMENTED,
        TYPE_DIMINICHED
    };
    

    int GetNote(int n);
    void SetNote(int note, int pitch);

    int GetMode();
    void SetMode(int n);

    int GetTriadRoot();
    void SetTriadRoot(int pitch);

    int GetGuitarRoot();
    void SetGuitarRoot(int pitch);

    int GetTriadMode();
    void SetTriadMode(int n);

    int GetGuitarMode();
    void SetGuitarMode(int n);

    void SetInversion(int n);
    int GetInversion();
    
    void SetOctave(int n);
    int GetOctave();

   void Set(const Chord& other);
   void SetFromVector(std::vector<int> &V);

   /**Should have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom)*/
   std::vector<int> SaveToVector();

    Glib::ustring GetName();
private:

    void RecalcNotes();


    int notes[6];

    
    int mode;

    int guitar_mode;
    int triad_mode;

    int triad_root;
    int guitar_root;

    int inversion;

    int octave;
};

#endif	/* CHORD_H */

