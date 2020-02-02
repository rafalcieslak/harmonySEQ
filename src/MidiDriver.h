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

#include <thread>

#include <alsa/asoundlib.h>

#ifndef MIDIDRIVER_H
#define	MIDIDRIVER_H
#include "global.h"
#include "DiodeMidiEvent.h"
#include "seqHandle.h"

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
    void SendNoteOnEventImmediatelly(int channel, int pitch, int velocity);

    /**Outputs immediately a noteoff*/
    void SendNoteOffEventImmediatelly(int channel, int pitch);

    /**Outputs a noteon and a noteoff after given time period.*/
    void SendNoteEvent(int channel, int pitch, int velocity, int duration);

    /**Schedules a single note on the output queue.*/
    void ScheduleNote(int channel, int tick_time, int pitch, int velocity, int length);
    /**Schedules a single MIDI control event on the output queue.*/
    void ScheduleCtrlEventSingle(int channel, int tick_time, int ctrl_no, int value);
    /**Schedules a set of MIDI controller events on the output queue, so that to emulate a linear slope.*/
    void ScheduleCtrlEventLinearSlope(int channel, int ctrl_no, int start_tick_time, int start_value, int end_tick_time, int end_value);

    void ScheduleDiodeEvent(DiodeType type, seqHandle handle, int tick_time, double time, int value, int color, int max_res = 0);

    /** Immediatelly send MIDI start/stop messages. **/
    void SendStop();
    void SendStart();

    /**Sets tempo*/
    void SetTempo(double bpm);

    /** Enables MIDI Clock output **/
    void SetMidiClockEnabled(bool enabled);

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


    int diode_event_id_next;
    /**Diode data is kept here*/
    std::mutex diode_events_mutex;
    std::map<int, DiodeMidiEvent> diode_events;


    /**Returns queue's current tick*/
    snd_seq_tick_time_t GetTick();


    /**Called every time there are MIDI events on input, this procedure processes them; no need to call it from anywhere else*/
    void ProcessInput();

};

#endif	/* MIDIDRIVER_H */
