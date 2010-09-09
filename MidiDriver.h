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

#include <alsa/asoundlib.h>

#ifndef MIDIDRIVER_H
#define	MIDIDRIVER_H
#include "global.h"

class MidiDriver {
public:
    snd_seq_t* seq_handle;
    int output_port,output_port2, input_port;
    int queueid;
    bool working;
    double tempo;
    int tick;
    MidiDriver();
    void SendNoteEvent(int pitch, int volume);
    void PassEvent(snd_seq_event_t *ev);
    void Open();
    void InitQueue();
    void SetTempo(double bpm);
    void ClearQueue();
    void DeleteQueue();
    void UpdateQueue();
    void ProcessInput();
    MidiDriver(const MidiDriver& orig);
    virtual ~MidiDriver();
private:

};

#endif	/* MIDIDRIVER_H */

