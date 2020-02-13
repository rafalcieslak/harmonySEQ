/*
    Copyright (C) 2010-2012 Rafał Cieślak

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
#include "global.hpp"
#include "MidiDriver.hpp"
#include "messages.hpp"
#include "NoteSequencer.hpp"
#include "ControlSequencer.hpp"
#include "Event.hpp"
#include "Configuration.hpp"
#include "SettingsWindow.hpp"
#include "SequencerWidget.hpp"
#include "SequencerManager.hpp"
#include "NoteAtom.hpp"


MidiDriver::MidiDriver() {
    paused = false;
    running = false;
    metronome = false;
    midi_clock_enabled = true;
    diodes_enabled = true;

    for(int i = 0; i < 30; i++)
        tap_times.push_front(0.0);

    //First, we try to open the driver: starting, connectig to system etc.
    Open();
    //And then we prepare the queue.
    InitQueue();
    //Finally, we'll set the tempo to it's current value.
    tempo = DEFAULT_TEMPO;
    SetTempo(tempo);

    diode_event_id_next = 0;
}

MidiDriver::~MidiDriver() {
}

bool MidiDriver::GetPaused(){
    return paused;
}

void MidiDriver::Run() {
    // Set running flag to true, but return if it was already set.
    if (running.exchange(true)) return;
    // Initialize queue...
    midi->StartQueue();
    // initial call, puts the first ECHO event, to make sure the loop will loop.
    midi->UpdateQueue();
    // go into infinite loop (while running = 1)
    midi->LoopWhileWaitingForInput();
    // Clean up after we are done.
    midi->ClearQueue();
    midi->AllNotesOff();
    midi->DeleteQueue();
}

void MidiDriver::Stop() {
    running = false;
}

void MidiDriver::StartQueue(){
    //Runs the queue. Warning: It's not unpausing, unpausing is implemented in ContinueQueue().
    *dbg << "The queue is starting!\n";
    snd_seq_start_queue(seq_handle,queueid,NULL);
    snd_seq_drain_output(seq_handle);
}


void MidiDriver::LoopWhileWaitingForInput(){
    int npfd;
    struct pollfd* pfd;

    npfd = snd_seq_poll_descriptors_count(seq_handle,POLLIN);
    pfd = (struct pollfd*)alloca(npfd*sizeof(struct pollfd*));
    snd_seq_poll_descriptors(seq_handle,pfd,npfd,POLLIN);

    while(running == 1)
        if (poll(pfd,npfd,1000)>0)
            ProcessInput();
}

void MidiDriver::Open(){
    //Try to open ALSA sequencer (and get a handle to it)
    int e = snd_seq_open(&seq_handle,"default",SND_SEQ_OPEN_DUPLEX, SND_SEQ_NONBLOCK);

    //catch errors
    if (e < 0){
        *err << _("Failed to open ALSA sequencer.\n");
        return;
    }

    //Set client name
    snd_seq_set_client_name(seq_handle,"harmonySEQ");
    snd_seq_set_output_buffer_size(seq_handle,(1<<22));


    //Try to create the port we use for MIDI output
    output_port = snd_seq_create_simple_port(seq_handle,"harmonySEQ output",SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,SND_SEQ_PORT_TYPE_APPLICATION);

    //catch errors
    if (output_port < 0){
        *err << _("Failed to create output port.\n");
        return;
    }

    //Try to create the port we use for MIDI input
    input_port = snd_seq_create_simple_port(seq_handle,"harmonySEQ input",SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,SND_SEQ_PORT_TYPE_APPLICATION);

    //catch errors
    if (input_port < 0){
        *err << _("Failed to create input port.\n");
        return;
    }

    //Increase pool size for greater kernel buffer
    snd_seq_set_client_pool_output(seq_handle,2000);

    *dbg << _("Alsa midi driver init successfull.\n");
}

void MidiDriver::SendNoteOnEventImmediatelly(int channel, int pitch, int velocity){
    //Create a new clear event
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    //Direct it to output port, to it's all subscribers
    snd_seq_ev_set_source(&ev,output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    //Fill it with data...
    snd_seq_ev_set_noteon(&ev,channel-1,pitch,velocity);
    //And output immediatelly - do not push into the queue.
    snd_seq_event_output_direct(seq_handle,&ev);
    //snd_seq_drain_output(seq_handle);

}


void MidiDriver::SendNoteOffEventImmediatelly(int channel, int pitch){
    //Create a new clear event
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    //Direct it to output port, to it's all subscribers
    snd_seq_ev_set_source(&ev,output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    //Fill it with data...
    snd_seq_ev_set_noteoff(&ev,channel-1,pitch,0);
    //And output immediatelly - do not push into the queue.
    snd_seq_event_output_direct(seq_handle,&ev);
    //snd_seq_drain_output(seq_handle);
}

void MidiDriver::SendStart() {
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_START;
    snd_seq_ev_set_source(&ev,output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_event_output_direct(seq_handle,&ev);
}

void MidiDriver::SendStop() {
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_STOP;
    snd_seq_ev_set_source(&ev,output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_event_output_direct(seq_handle,&ev);
}

void MidiDriver::ScheduleNote(int channel, int tick_time, int pitch, int velocity, int length){
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_note(&ev, channel, pitch, velocity, length);
    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick_time);
    snd_seq_ev_set_source(&ev, output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_event_output(seq_handle, &ev);
}

void MidiDriver::ScheduleCtrlEventSingle(int channel, int tick_time, int ctrl_no, int value){
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_controller(&ev, channel, ctrl_no, value);
    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick_time);
    snd_seq_ev_set_source(&ev, output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_event_output(seq_handle, &ev);
}

void MidiDriver::ScheduleCtrlEventLinearSlope(int channel, int ctrl_no, int start_tick_time, int start_value, int end_tick_time, int end_value){
    //*err << "s = " << start_value << ", e = " << end_value << ENDL;
    int steps = end_value - start_value;
    int time = end_tick_time - start_tick_time;
    int abs_steps = (steps>0)?steps:-steps;
    int sign = (steps>0)?1:-1;
    if(steps != 0)
        for(int x = 0; x <= abs_steps; x++){
            ScheduleCtrlEventSingle(channel,start_tick_time+sign*x*time/steps,ctrl_no,start_value+sign*x);
        }
    else
        ScheduleCtrlEventSingle(channel,start_tick_time,ctrl_no,start_value);
}

void MidiDriver::ScheduleDiodeEvent(const DiodeMidiEvent& dev, int tick_time){
    if(!diodes_enabled) return;
    diode_events.insert(std::pair<int,DiodeMidiEvent>(diode_event_id_next, dev));

    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_USR0; //Diode ON
    ev.data.raw32.d[0] = diode_event_id_next; //diode event storage ID
    ev.data.raw32.d[1] = 0; //unused
    ev.data.raw32.d[2] = 0;//unused
    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick_time);
    snd_seq_ev_set_dest(&ev, snd_seq_client_id(seq_handle), input_port); //here INPUT_PORT is used, so the event will be send just to harmonySEQ itself.
    snd_seq_event_output(seq_handle, &ev);
    snd_seq_ev_clear(&ev);

    diode_event_id_next++;
}

void MidiDriver::PassEvent(snd_seq_event_t* ev){
    snd_seq_ev_set_source(ev,output_port);
    snd_seq_ev_set_subs(ev);
    snd_seq_ev_set_direct(ev);
    snd_seq_event_output_direct(seq_handle,ev);
}

void MidiDriver::InitQueue(){
    //Asks ALSA for a new queue and get a handle to it (queueid).
    queueid = snd_seq_alloc_named_queue(seq_handle,"harmonySEQ queue");

    //Clear the ticks counter.
    tick = 0;
}

void MidiDriver::SetTempo(double bpm){
    snd_seq_queue_tempo_t* queue_tempo;
    snd_seq_queue_tempo_malloc(&queue_tempo);
    snd_seq_queue_tempo_set_tempo(queue_tempo,((double)6e7/((double)bpm*4.0)));
    snd_seq_queue_tempo_set_ppq(queue_tempo,TICKS_PER_BEAT/4);
    snd_seq_set_queue_tempo(seq_handle,queueid,queue_tempo);
    tempo = bpm;
    snd_seq_queue_tempo_free(queue_tempo);

    on_tempo_changed();
}

double MidiDriver::GetTempo(){
    return tempo;
}

void MidiDriver::TapTempo(){
    double now = GetRealTime();
    double current = now, sum = 0.0;
    std::vector<double> deltas;
    for(double q : tap_times){
        double delta = current - q;
        if(delta > 6.0)
            break;
        deltas.push_back(delta);
        current = q;
    }
    if(deltas.size() >= 5) {
        std::sort(deltas.begin(), deltas.end());
        int n1 = deltas.size() * 1 / 3, n2 = deltas.size() * 2 / 3;
        *dbg << n1 << " " << n2 << ENDL;
        for(auto it = deltas.begin() + n1; it != deltas.begin() + n2; it++){
            sum += *it;
        }
        double new_tempo = 60.0*(n2 - n1)/sum;
        *dbg << "new tap tempo: " << new_tempo << " from " << n2 - n1 << ENDL;
        SetTempo(new_tempo);
    }

    tap_times.pop_back();
    tap_times.push_front(now);
}

void MidiDriver::SetMetronome(bool enabled){
    metronome = enabled;
}

bool MidiDriver::GetMetronome(){
    return metronome;
}

void MidiDriver::SetMidiClockEnabled(bool enabled){
    midi_clock_enabled = enabled;
}

bool MidiDriver::GetMidiClockEnabled(){
    return midi_clock_enabled;
}

void MidiDriver::SetPassMidiEvents(bool enabled){
    passing_midi_events = enabled;
}

bool MidiDriver::GetPassMidiEvents(){
    return passing_midi_events;
}

void MidiDriver::SetDiodesEnabled(bool enabled){
    diodes_enabled = enabled;
}

bool MidiDriver::GetDiodesEnabled(){
    return diodes_enabled;
}

void MidiDriver::PauseImmediately(){
    //Pause the queue
    snd_seq_stop_queue(seq_handle,queueid,NULL);
    snd_seq_drain_output(seq_handle);

    if(midi_clock_enabled) SendStop();

    //Turn all notes off
    AllNotesOff();
    paused = true;

    *dbg << "Queue paused!\n";

    on_paused();
}

void MidiDriver::Sync(){
    snd_seq_drop_output(seq_handle);
    PauseImmediately();
    // Reset sequencer progress
    for (std::shared_ptr<Sequencer> seq: SequencerManager::GetAll())
        seq->play_from_here_marker = 0;
    Unpause();
    *dbg << "Sync complete.\n";
}

snd_seq_tick_time_t MidiDriver::GetTick() {
    snd_seq_queue_status_t *status;
    snd_seq_tick_time_t current_tick;
    snd_seq_queue_status_malloc(&status);
    snd_seq_get_queue_status(seq_handle, queueid, status);
    current_tick = snd_seq_queue_status_get_tick_time(status);
    snd_seq_queue_status_free(status);
    return(current_tick);
}


void MidiDriver::Unpause(){

    //Clear the queue, INCLUDING NOTEOFFS (1).
    ClearQueue(1);
    //AllNotesOff(); // here: NOT! might cause artefacts.
    //Sync the tick
    tick = GetTick();

    if(midi_clock_enabled) SendStart();

    //Update the queue
    UpdateQueue();

    //Continue the queue
    int i = snd_seq_continue_queue(seq_handle,queueid,NULL) ;
    if (i==-11) {//EAGAIN error. May happen sometimes unpredictably. Usually repeating request helps.
        *dbg <<  i<< ENDL;
    }
    snd_seq_drain_output(seq_handle);
    //Remember the state
    paused = false;
    *dbg << "Queue unpaused!\n";

    on_unpaused();
}

void MidiDriver::ClearQueue(bool remove_noteoffs){
    *dbg << "clearing queue...\n";
    snd_seq_remove_events_t *re;
    snd_seq_remove_events_malloc(&re);
    snd_seq_remove_events_set_queue(re,queueid);
    if(remove_noteoffs) snd_seq_remove_events_set_condition(re,SND_SEQ_REMOVE_OUTPUT);
        else snd_seq_remove_events_set_condition(re,SND_SEQ_REMOVE_OUTPUT|SND_SEQ_REMOVE_IGNORE_OFF);
    snd_seq_remove_events(seq_handle,re);
    snd_seq_remove_events_free(re);

    //also, clear the diode events map.
    diode_events.clear();
    *dbg << "queue cleared.\n";
}

void MidiDriver::DeleteQueue(){
    //If there was no queue, do not even try deleting it.
    if (queueid<0) return;
    *dbg << "stopping queue";
    //Stop and free the queue.
    snd_seq_stop_queue(seq_handle,queueid,NULL);
    snd_seq_free_queue(seq_handle,queueid);
}


void MidiDriver::AllNotesOff(){
    snd_seq_event_t ev;
    for (int ch = 0; ch < 16; ch++){
        snd_seq_ev_clear(&ev);
        snd_seq_ev_set_source(&ev,output_port);
        snd_seq_ev_set_subs(&ev);
        snd_seq_ev_set_direct(&ev);
        ev.type = SND_SEQ_EVENT_CONTROL14;
        ev.data.control.channel = ch;
        ev.data.control.param = MIDI_CTL_ALL_NOTES_OFF;
        ev.data.control.value = 0;
        snd_seq_event_output(seq_handle,&ev);
    }
    //Drain output, do not put these events on the queue
    snd_seq_drain_output(seq_handle);
}

double RoundTimeDouble(double marker){

            long long int temp = (long long int)((double)marker*(double)1e9);
            return ((double)(temp))/(double)(1e9);
}

void MidiDriver::UpdateQueue(){
    snd_seq_event_t ev;
    double real_time = GetRealTime();

    // send ECHO event to harmonySEQ itself, so it will be notified when the bar finishes, and new notes must be put on the queue.
    // sending the ECHO event takes place before all the notes, just in case the buffer is to small - to avoid dropping the echo event
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_ECHO;
    snd_seq_ev_schedule_tick(&ev,queueid,0,tick+TICKS_PER_BEAT);
    snd_seq_ev_set_dest(&ev,snd_seq_client_id(seq_handle),input_port); //here INPUT_PORT is used, so the event will be send just to harmonySEQ itself.
    snd_seq_event_output_direct(seq_handle,&ev);

    if(midi_clock_enabled) {
        unsigned int clock_every_n_ticks = TICKS_PER_BEAT/MIDI_PPQ;
        for(unsigned int clock_tick = tick; clock_tick < tick + TICKS_PER_BEAT; clock_tick += clock_every_n_ticks){
            snd_seq_ev_clear(&ev);
            ev.type = SND_SEQ_EVENT_CLOCK;
            snd_seq_ev_set_fixed(&ev);
            snd_seq_ev_schedule_tick(&ev, queueid, 0, clock_tick);
            snd_seq_ev_set_source(&ev, output_port);
            snd_seq_ev_set_subs(&ev);
            //Output the event (but it stays at the queue.)
            snd_seq_event_output(seq_handle, &ev);
        }
    }

    //For each sequencer...
    for (std::shared_ptr<Sequencer> seq : SequencerManager::GetAll()){
        //Update the seq's PlayOnce phase.

        //If the sequencer is about to play it's pattern once, but it's on anyway, reset the playonce phase, and use the sequencer as if it was
        //just simply turned on.
        if(seq->GetPlayOncePhase()==1 && seq->GetOn() == 1){
            seq->SetPlayOncePhase(0);
        }
        //Phase 3 means it has already been played, and it has just ended, so now we'll just turn it off, by setting phase back to initial state: 0.
        if(seq->GetPlayOncePhase() == 3) {
            seq->SetPlayOncePhase(0);
            seq->play_from_here_marker = 0.0; //reset the starting playback position.
        }
        //If phase is set to 1, this means this sequencer is about to be played once right now, so we'll change the phase to 2 (to change the colour
        //in main window corresponding to this sequencer). The last_played_note is set to 0, to make sure that when a loooong sequence (more
        //than one bar long) is played once, it's played from it's beggining
        if(seq->GetPlayOncePhase() == 1) {
            seq->SetPlayOncePhase(2);
            seq->play_from_here_marker = 0.0;
        }

        //Output the notes only if the sequencer is on, or it's in 2nd phase. Otherwise skip the note outputting routines....
        if (seq->GetOn() || seq->GetPlayOncePhase() == 2) {

            //Selecting diode color according to mode
            int diode_colour;
            if (seq->GetPlayOncePhase() == 2) diode_colour = 1;
            else diode_colour = 0;

            seq->SetPlaying(true);

            //The time (in ticks)  how long one sequence repetition will last. Note it can be larger than TICKS_PER_BEAT and that's OK.
            double sequence_time = TICKS_PER_BEAT*seq->GetLength();
            //Local tick is the tick at which this sequence repetition had began. Occasionally it may be lower than tick, for
            //example in case this repetition has already been played in 1/3.
            double local_tick = tick-seq->play_from_here_marker*TICKS_PER_BEAT*seq->GetLength();
            //Shortcut to the pattern we'll play.
            std::shared_ptr<AtomContainer> pattern = seq->GetActivePattern();
            //Number of notes in one sequence
            int size = pattern->GetSize();
            //The note and ctrlatom.
            NoteAtom* note;
            ControllerAtom* ctrl, *next_ctrl;

            double start_marker = seq->play_from_here_marker;
            double end_marker = seq->play_from_here_marker+(1.0/seq->GetLength());

            //s: the number of the first note that has time greater than the start_marker
            int s = -1;
            //e: the number of the last note that has time smaller then the end_marker
            int e = -1;
            //Note that s and e can be left as -1, this means there is no such note, and causes harmonseq not to output enything in this bar
            //It may also happen that e>size, usually in case when seq->GetLength() > 1.0.

            //ok, now find the s and e.
            if(size != 0){//ensure there are any notes
                for(int X = 0; true; X++){
                    //*err << "at note " << X << ", X/size = " << X/size << ENDL;
                    Atom* atm = ((*pattern)[X%size]);
                    if(atm->time + (X/size)*1.0 >= start_marker && s == -1) s = X ;
                    if(atm->time + (X/size)*1.0 < end_marker) e = X;
                    if(atm->time + (X/size)*1.0 >= end_marker) break;
                }
            }


            //We know which atoms to play, so lets play them.
            if(e != -1 && s != -1 && e>=s){
                //Determine whether to output notes or control messages
                if(seq->GetType() == SEQ_TYPE_NOTE){

                    std::shared_ptr<NoteSequencer> noteseq = std::dynamic_pointer_cast<NoteSequencer>(seq);

                    for(int V = s; V<=e;V++){
                        note = dynamic_cast<NoteAtom*>((*pattern)[V%size]);
                        int pitch = noteseq->GetNoteOfChord(note->pitch);
                        ScheduleNote(seq->GetChannel()-1,
                                     local_tick + (V/size)*sequence_time + note->time*TICKS_PER_BEAT*seq->GetLength(),
                                     pitch, note->velocity,
                                     note->length*TICKS_PER_BEAT*seq->GetLength()*(noteseq->GetGatePercent()/100.0));
                        ;
                        //each note shall have correspoinding diode event.
                        DiodeMidiEvent dev(seq, DIODE_TYPE_NOTE, note->time, note->pitch, diode_colour);
                        ScheduleDiodeEvent(dev, local_tick + (V / size) * sequence_time + note->time * TICKS_PER_BEAT * seq->GetLength());

                    }
                }else if(seq->GetType() == SEQ_TYPE_CONTROL){

                    std::shared_ptr<ControlSequencer> ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(seq);

                    for(int V = s; V<=e;V++){
                        ctrl = dynamic_cast<ControllerAtom*> ((*pattern)[V % size]);
                        if(ctrl->slope_type == SLOPE_TYPE_FLAT){
                            ScheduleCtrlEventSingle(seq->GetChannel()-1, local_tick + (V/size)*sequence_time + ctrl->time*TICKS_PER_BEAT*seq->GetLength(),ctrlseq->GetControllerNumber(),ctrl->value);
                        }else if(ctrl->slope_type == SLOPE_TYPE_LINEAR){


                            //TODO very important! output slopes ONLY to next bar!


                            next_ctrl = dynamic_cast<ControllerAtom*>((*pattern)[(V+1)%size]);
                            double nextctrl_time = next_ctrl->time;
                            if(V==size-1) nextctrl_time += 1.0; //if this is a last note in pattern, make sure to schedule it's slope later!
                            ScheduleCtrlEventLinearSlope(
                                seq->GetChannel()-1,
                                ctrlseq->GetControllerNumber(),
                                local_tick + (V/size)*sequence_time + ctrl->time*TICKS_PER_BEAT*seq->GetLength(),
                                ctrl->value,
                                local_tick + (V/size)*sequence_time + nextctrl_time*TICKS_PER_BEAT*seq->GetLength(),
                                next_ctrl->value
                                );

                        }else{
                            //something wrong.
                        }

                        //each note shall have correspoinding diode event.
                        DiodeMidiEvent dev(seq, DIODE_TYPE_CTRL, ctrl->time, ctrl->value, diode_colour);
                        ScheduleDiodeEvent(dev, local_tick + (V / size) * sequence_time + ctrl->time * TICKS_PER_BEAT * seq->GetLength());

                    }

                }else{
                    *err << "Sequencer is neither note nor control type. Don't bother reporting this to harmonySEQ developers. This error message will never display, so if you see it, it means you must have broken something intentionally.\n";
                }
            }

            // Store playback data so that the UI can accurately draw the playback marker.
            seq->playback_marker__start_pos = seq->play_from_here_marker;
            seq->playback_marker__start_time = real_time;
            seq->playback_marker__end_pos = end_marker;
            seq->playback_marker__end_time = real_time + 60.0/tempo;

            if(end_marker >= 1.0){
                end_marker = end_marker - (int)end_marker;
                if(seq->GetPlayOncePhase() == 2)
                    seq->SetPlayOncePhase(3);
            }
            //rounding to ensure sync...
            if(end_marker > -0.000000001 && end_marker < 0.000000001) end_marker = 0.0;
            // Save playback position
            seq->play_from_here_marker = end_marker;
        }else{
            // Sequencer is off
            seq->SetPlaying(false);
        }

        //And proceed to next sequencer.
    }

    //Also, playback the metronome notes.
    if (metronome){
        if (metronome_counter % 4 == 0){
            snd_seq_ev_clear(&ev);
            snd_seq_ev_set_note(
                &ev, Config::Metronome::Channel-1, Config::Metronome::Hit1Note,
                Config::Metronome::Hit1Velocity, TICKS_PER_BEAT);
            snd_seq_ev_schedule_tick(&ev, queueid, 0, tick);
            snd_seq_ev_set_source(&ev, output_port);
            snd_seq_ev_set_subs(&ev);
            snd_seq_event_output_direct(seq_handle, &ev);
        } else if (Config::Metronome::Hit2){
            snd_seq_ev_clear(&ev);
            snd_seq_ev_set_note(
                &ev, Config::Metronome::Channel-1, Config::Metronome::Hit2Note,
                Config::Metronome::Hit2Velocity, TICKS_PER_BEAT);
            snd_seq_ev_schedule_tick(&ev, queueid, 0, tick);
            snd_seq_ev_set_source(&ev, output_port);
            snd_seq_ev_set_subs(&ev);
            snd_seq_event_output_direct(seq_handle, &ev);

        }
        metronome_counter += 1;
        // snd_seq_free_event(&ev);
    }

    // Increment the internal tick counter
    tick+=TICKS_PER_BEAT;

    /**Note, that if there is A LOT of notes on the queue, the following call will take some time. However, it does not use CPU, and we have already unlocked gtk threads, so it can be safely called.*/
    int res = snd_seq_drain_output(seq_handle);
    if(res != 0) *err << "ERROR: ALSA sequencer interface returned an error code (" << res << ") on snd_seq_drain_output.\n";

    on_beat();
}


void MidiDriver::ProcessInput(){
    // While there is anything in the input
    do {
        //Obtain the event from input
        snd_seq_event_t * ev;
        snd_seq_event_input(seq_handle,&ev);
        // If we are in MIDI passthrough mode, pass the event (Well, unless it's the ECHO, which MUST be caught).
        if(passing_midi_events && ev->type!=SND_SEQ_EVENT_ECHO && ev->type!=SND_SEQ_EVENT_USR0) {
            PassEvent(ev);
            continue;
        }

        //Switch, according to the type.
        if(ev->type == SND_SEQ_EVENT_NOTEON){
            if (ev->data.note.velocity != 0) {
                //That's a note-on. It might have triggered events, so let's check for them.
                // For now events are processed by the UI thread; this will change once we implement the events thread.
                DeferWorkToUIThread(
                    [=](){
                        FindAndProcessEvents(Event::NOTE,ev->data.note.note,ev->data.note.channel+1);
                        return false;
                    });
            } else {
                //That's a note-off. We ignore it as for now.
            }
        } else if (ev->type == SND_SEQ_EVENT_ECHO) {
            *dbg << "ECHO!\n";
            //As we got the ECHO event, this means we must prepare the next bar, that is starting right now.
            UpdateQueue();
        } else if (ev->type == SND_SEQ_EVENT_CONTROLLER) {
            //This is a controller event. It might have triggered events, so let's check for them.
            DeferWorkToUIThread(
                [=](){
                    FindAndProcessEvents(Event::CONTROLLER,ev->data.control.param,ev->data.control.channel+1);
                    return false;
                });
        } else if (ev->type == SND_SEQ_EVENT_PITCHBEND) {
            //Pithbend event. Pass it through.
            snd_seq_ev_set_source(ev,output_port);
            snd_seq_ev_set_subs(ev);
            snd_seq_ev_set_direct(ev);
            snd_seq_event_output_direct(seq_handle,ev);
        } else if (ev->type == SND_SEQ_EVENT_PGMCHANGE) {
            //Program change event. Pass it through
            snd_seq_ev_set_source(ev,output_port);
            snd_seq_ev_set_subs(ev);
            snd_seq_ev_set_direct(ev);
            snd_seq_event_output_direct(seq_handle,ev);
        } else if (ev->type == SND_SEQ_EVENT_USR0) {
            int id = ev->data.raw32.d[0];

            std::map<int,DiodeMidiEvent>::iterator it = diode_events.find(id);
            // If the event wasn't found, ignore this message.
            if(it == diode_events.end())
                break;
            DiodeMidiEvent diodev = (*it).second;
            diode_events.erase(it);

            // In case such event was not registered, ignore it.
            if (it == diode_events.end()) break;
            on_diode(diodev);
        } else {
            //Some unmatched event.

        }
        //Get rid of the event.
        snd_seq_free_event(ev);

        //Repeat it while there is any event on input.
    }while (snd_seq_event_input_pending(seq_handle,0)>0);

}
