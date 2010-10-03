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
    int GetMode();
    void SetMode(int n);
    void SetNote(int note, int pitch);
    void SetRoot(int pitch);
    int GetRoot();
    void SetType(int n);
    int GetType();
    void SetGuitarMode(int n);
private:

    int notes[6];

    
    int mode;

    int guitar_mode;

    int root;

    int type;
};

#endif	/* CHORD_H */

