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


    /**Chord types*/
    enum {
        CHORD_TYPE_CUSTOM,
        CHORD_TYPE_TRIAD,
        CHORD_TYPE_GUITAR
    };

    /**Guitar chord modes*/
    enum {
        CHORD_GUITAR_MODE_MAJOR,
        CHORD_GUITAR_MODE_MINOR,
        CHORD_GUITAR_MODE_MAJ7,
        CHORD_GUITAR_MODE_M7
    };

    /**Triad chord modes*/
    enum {
        CHORD_TRIAD_MODE_MAJOR,
        CHORD_TRIAD_MODE_MINOR,
        CHORD_TRIAD_MODE_AUGMENTED,
        CHORD_TRIAD_MODE_DIMINISCHED
    };
    
    //The following have obvious names, they does not require to be commented

    int GetNotePlusBasenote(int n);
    /*Returns a note ignoring*/
    int GetNote(int n);
    void SetNote(int note, int pitch);

    int GetType();
    void SetType(int n);

    int GetRoot();
    void SetRoot(int pitch);

    int GetTriadMode();
    void SetTriadMode(int n);

    int GetGuitarMode();
    void SetGuitarMode(int n);

    void SetInversion(int n);
    int GetInversion();
    
    void SetBaseOctave(int n);
    int GetBaseOctave();

    void SetBaseNote(int n);
    int GetBaseNote();

    void SetBase(int n);
    int GetBase();

    void SetBaseUse(bool use);
    bool GetBaseUse();
    
   /**Copies all data from another chord (fe. from an action)*/
   void Set(const Chord& other, bool apply_octave=1);


   /**Load chord settings from vector. Should have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom).*/
   void SetFromVector(std::vector<int> &V);
   /**Load chord settings from vector. Should have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom).*/
   void SetFromVector_OLD_FILE_PRE_0_14(std::vector<int> &V);

   /**Saves all chord settings to a vector. Should have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom).*/
   std::vector<int> SaveToVector();

   /**Generates a name for the chord, like "Guitar, E-moll"*/
    Glib::ustring GetName(bool do_not_use_octave=0);
    
   /**Generates a summarized name for the chord, like "Triad, a-moll, i:1, o:2"*/
    Glib::ustring GetSummary(bool do_not_use_octave=0);

    /**Recalcs Base to Note and Octave*/
    void BaseToOctaveAndNote();
    /**Recalcs Note and Octave to Base*/
    void NoteAndOctaveToBase();
private:

    /**Recalculates notes (given the modes, roots etc) and stores them to notes array.*/
    void RecalcNotes();
    /**Notes are stored here so that they do not need to be recalculated every time.*/
    int notes[6];

    int type;

    int base;
    int base_octave;
    int base_note;
    //states whether pitches are given relatively to the base note.
    bool base_use;

    int root;

    int mode_guitar;
    int mode_triad;

    int inversion;

};

#endif	/* CHORD_H */

