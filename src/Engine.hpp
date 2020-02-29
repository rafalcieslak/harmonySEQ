/*
    Copyright (C) 2010-2011, 2020 Rafał Cieślak

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



#ifndef MIDIDRIVER_H
#define	MIDIDRIVER_H

#include <boost/signals2.hpp>
namespace bs2 = boost::signals2;

#include <alsa/asoundlib.h>

class DiodeMidiEvent;


class Engine {
public:
    /**Constructor, called only from main()*/
    Engine();

    virtual ~Engine();

    /** Enters the main event loop. */
    void Run();

    /** Requests the call to Run() to return */
    void Stop();

    /**Stops playback - pauses the queue*/
    void PauseImmediately();
    /**Unpauses the queue*/
    void Unpause();
    /** Tell whether the queue is paused*/
    bool GetPaused();

    /**Stops all notes and updates queue IMMIDIATELLY*/
    void Sync();

    /**Outputs immediately a noteon*/
    void SendNoteOnEventImmediatelly(int channel, int pitch, int velocity);

    /**Outputs immediately a noteoff*/
    void SendNoteOffEventImmediatelly(int channel, int pitch);

    /** Immediatelly send MIDI start/stop messages. **/
    void SendStop();
    void SendStart();

    /**Sets tempo*/
    void SetTempo(double bpm);
    double GetTempo();

    void TapTempo();

    /** Enables or disables metronome output. */
    /** These methods can be safely called from other threads. */
    void SetMetronome(bool enabled);
    bool GetMetronome();

    /** Enables or disables MIDI clock output. */
    /** These methods can be safely called from other threads. */
    void SetMidiClockEnabled(bool enabled);
    bool GetMidiClockEnabled();

    /** Enables or disables MIDI event passing through harmonySEQ. */
    /** These methods can be safely called from other threads. */
    void SetPassMidiEvents(bool enabled);
    bool GetPassMidiEvents();

    /** Enables or disables diode event emitting. */
    /** These methods can be safely called from other threads. */
    void SetDiodesEnabled(bool disabled);
    bool GetDiodesEnabled();

    /** These signals may be emitted by any thread - the
     * subscriber must relay work to a different thread if
     * necessary. */
    bs2::signal<void()> on_paused;
    bs2::signal<void()> on_unpaused;
    bs2::signal<void()> on_beat;
    bs2::signal<void()> on_tempo_changed;

    /* TODO: Each sequencer should have their own diode signal, so
     * that sequencerwidgets could subscribe to events on that one
     * specific seq. */
    bs2::signal<void(DiodeMidiEvent)> on_diode;

private:
    /**Infinite loop, exiting when"running" variable is set to 0. Waits for any MIDI events on input, and calls ProcessInput() when something is on the input.*/
    void LoopWhileWaitingForInput();

    /**Called every time there are MIDI events on input, this procedure processes them; no need to call it from anywhere else*/
    void ProcessInput();

    /**Runs the queue. Called on startup.  Warning: It's not unpausing, unpausing is implemented in ContinueQueue().*/
    void StartQueue();

    /**This routine gets called every time the ECHO event is received. It puts next set of notes (one bar long) into the queue, and puts next ECHO event to harmonySEQ itself, so it gets called consequently. */
    void UpdateQueue();

    /**Removes all events from queue, except noteoffs*/
    void ClearQueue(bool remove_noteoffs = 0);

    /**Closes and removes queue*/
    void DeleteQueue();

    /**Ends all note events (send noteoffs)*/
    void AllNotesOff();

    /**Opens the sequencer MIDI ports, activates the driver.*/
    void Open();

    /**Inits Queue*/
    void InitQueue();

    /**Schedules a single note on the output queue.*/
    void ScheduleNote(int channel, int tick_time, int pitch, int velocity, int length);
    /**Schedules a single MIDI control event on the output queue.*/
    void ScheduleCtrlEventSingle(int channel, int tick_time, int ctrl_no, int value);
    /**Schedules a set of MIDI controller events on the output queue, so that to emulate a linear slope.*/
    void ScheduleCtrlEventLinearSlope(int channel, int ctrl_no, int start_tick_time, int start_value, int end_tick_time, int end_value);
    /** Schedules a Diode callback event. */
    void ScheduleDiodeEvent(const DiodeMidiEvent& dev, int tick_time);

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

    std::deque<double> tap_times;

    /** Used to notify the engine thread to quit */
    std::atomic<bool> running;

    /**Outputs event immediately.
     * @parram ev an event to pass*/
    void PassEvent(snd_seq_event_t *ev);

    /** Controls metronome event output. */
    std::atomic<bool> metronome;
    int metronome_counter = 0;

    /** Controls MIDI clock output. */
    std::atomic<bool> midi_clock_enabled;

    /** Controls MIDI event passthrough. */
    std::atomic<bool> passing_midi_events;

    /** Used to optionally disables diode event processing. */
    std::atomic<bool> diodes_enabled;


    int diode_event_id_next;
    /**Diode data is kept here*/
    std::map<int, DiodeMidiEvent> diode_events;


    /**Returns queue's current tick*/
    snd_seq_tick_time_t GetTick();

};

#endif	/* MIDIDRIVER_H */
