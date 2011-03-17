/*
    Copyright (C) 2010, 2011 Rafał Cieślak

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
    /**Constructor, called only from main()*/
    MidiDriver();

    virtual ~MidiDriver();



    /**Infinite loop, exiting when"running" variable is set to 0. Waits for any MIDI events on input, and calls ProcessInput() when something is on the input.*/
    void LoopWhileWaitingForInput();

    /**Runs the queue. Called on startup.  Warning: It's not unpausing, unpausing is implemented in ContinueQueue().*/
    void StartQueue();


    /**Stops playback - pauses the queue*/
    void PauseQueueImmediately(); 

    /**Stops playback on next bar. NOT YET IMPLEMENTED*/
    void PauseOnNextBar();

    /*Tell whether the queue is paused*/
    bool GetPaused();
    
    /**Unpauses the queue*/
    void ContinueQueue();


    /**This routine gets called every time the ECHO event is received. It puts next set of notes (one bar long) into the queue, and puts next ECHO event to harmonySEQ itself, so it gets called consequently. */
    void UpdateQueue(bool do_not_lock_threads = 0);

    /**Stops all notes and updates queue IMMIDIATELLY*/
    void Sync();


    /**Outputs immediately a noteon*/
    void SendNoteOnEvent(int channel, int pitch, int volume);

    /**Outputs immediately a noteoff*/
    void SendNoteOffEvent(int channel, int pitch);

    /**Outputs a noteon and a noteoff after given time period.*/
    void SendNoteEvent(int channel, int pitch, int volume, int duration);

    
    /**Sets tempo*/
    void SetTempo(double bpm);


    /**Ends all note events (send noteoffs)*/
    void AllNotesOff();

    /**Removes all events from queue, except noteoffs*/
    void ClearQueue(bool remove_noteoffs = 0);

    /**Closes and removes queue*/
    void DeleteQueue();


private:
    /**ALSA MIDI sequencer's handle*/
    snd_seq_t* seq_handle;

    /**ALSA MIDI ports handles*/
    int output_port,input_port;

    /**ALSA MIDI queue handle*/
    int queueid;

    /**True if the driver is working*/
    bool working;

    /**Current tempo on the queue*/
    double tempo;
    int tick;

    /**States whether queue is paused or not*/
    bool paused;
    

    /**Outputs event immediately.
     * @parram ev an event to pass*/
    void PassEvent(snd_seq_event_t *ev);

    /**Opens the sequencer MIDI ports, activates the driver.*/
    void Open();

    /**Inits Queue*/
    void InitQueue();



    
    /**Returns queue's current tick*/
    snd_seq_tick_time_t GetTick();


    /**Called every time there are MIDI events on input, this procedure processes them; no need to call it from anywhere else*/
    void ProcessInput();

};

#endif	/* MIDIDRIVER_H */

