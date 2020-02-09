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
#include "Chord.h"
#include <glibmm/ustring.h>

/**A class representing a MIDI note sequencer.*/
class NoteSequencer : public Sequencer {
public:
    NoteSequencer();
    NoteSequencer(Glib::ustring _name0);
    NoteSequencer(const Sequencer& orig);
    virtual ~NoteSequencer();
    void Init();
    /**Virtual copy constructor.*/
    virtual Sequencer* Clone();

    /**Returns SEQ_TYPE_NOTE*/
    virtual SeqType_t GetType();

    /**The main chord*/
    Chord chord;

    /**Whether to show detailed chord settings in GUI, or hide them. This is stored here, in order to save this data to file.*/
    bool expand_chord;

    /**Returns a one note of chord of this sequencer
     *  @parram n note number*/
    int GetNoteOfChord(int n);

    void SetGatePercent(int p);
    int GetGatePercent() const;

    /* This signal can be emitted by any thread. */
    bs2::signal<void()> on_chord_change;
private:

    /** Stores sequencer gating percentage. A value of 100 represents
     *  that notes are being played at their original length; 50 means
     *  they get halved. */
    int gate_percent;
};

#endif	/* NOTESEQUENCER_H */
