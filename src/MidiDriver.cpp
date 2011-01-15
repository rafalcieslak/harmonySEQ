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

#ifdef __linux__
#include <alsa/asoundlib.h>
#endif
#include "MidiDriver.h"
#include "messages.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "Event.h"
#include "Configuration.h"

extern int running;

MidiDriver::MidiDriver() {
    working = false; //at the beggining the driver is not active, it isn't working
    paused = false; //and it's not paused at start

    //First, we try to open the driver: starting, connectig to system etc.
    Open();
    //And then we prepare the queue.
    InitQueue();
    //Finally, we'll set the tempo to it's current value.
    SetTempo(tempo);
}

MidiDriver::~MidiDriver() {

}

bool MidiDriver::GetPaused(){
    return paused;
}

#ifdef __linux__                                            /*Linux-specific implementation below.*/

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

    while(running == 1){
    if (poll(pfd,npfd,1000)>0)
        //*dbg << "w00t! an event got!\n";
        ProcessInput();
    };    
    
}

void MidiDriver::Open(){
    //Try to open ALSA sequencer (and get a handle to it)
    int e = snd_seq_open(&seq_handle,"default",SND_SEQ_OPEN_DUPLEX,0);

    //catch errors
    if (e < 0){
        *err << _("Failed to open ALSA sequencer.\n");
        return;
    }
    
    //Set client name
    snd_seq_set_client_name(seq_handle,"harmonySEQ");

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

    //If we got so far, then it seems everything gone well and we have now working sequencer ports.
    working = true;
    *dbg << _("Alsa midi driver init successfull.\n");
}

void MidiDriver::SendNoteEvent(int pitch, int volume){
    //Create a new clear event
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    //Direct it to output port, to it's all subsctibers
    snd_seq_ev_set_source(&ev,output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    //Fill it with data...
    snd_seq_ev_set_noteon(&ev,1,pitch,volume);
    //And output immidiatelly - do not push into the queue.
    snd_seq_event_output(seq_handle,&ev);
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::PassEvent(snd_seq_event_t* ev){
    *dbg << "passing an event...";
    //Direct the event to output port, to it's all subsctibers
    snd_seq_ev_set_source(ev,output_port);
    snd_seq_ev_set_subs(ev);
    snd_seq_ev_set_direct(ev);
    //And output immidiatelly - do not push into the queue.
    snd_seq_event_output(seq_handle,ev);
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::InitQueue(){
    //Asks ALSA for a new queue and get a handle to it (queueid).
    queueid = snd_seq_alloc_named_queue(seq_handle,"harmonySEQ queue");
    //Set maximum number of event on the queue to 8192 - should be enough.
    snd_seq_set_client_pool_output(seq_handle,8192);
    //Clear the ticks counter.
    tick = 0;
}

void MidiDriver::SetTempo(double bpm){
    //Create a new tempo-class (strange usage, but that's how ALSA want to receive the tempo)
    snd_seq_queue_tempo_t* queue_tempo;
    snd_seq_queue_tempo_malloc(&queue_tempo);
    //Calculate the real tempo value from the BPM
    snd_seq_queue_tempo_set_tempo(queue_tempo,((double)6e7/(double)bpm));
    //Set number of ticks per quarternote
    snd_seq_queue_tempo_set_ppq(queue_tempo,TICKS_PER_QUARTERNOTE);
    //Apply the tempo to the queue
    snd_seq_set_queue_tempo(seq_handle,queueid,queue_tempo);
    //Set the tempo variable
    tempo = bpm;
    //Get rid of the tempo-class
    snd_seq_queue_tempo_free(queue_tempo);
}

void MidiDriver::PauseQueueImmediately(){
    //Pause the queue
    snd_seq_stop_queue(seq_handle,queueid,NULL);
    snd_seq_drain_output(seq_handle);
    //Turn all notes off
    AllNotesOff();
    //Remember the state
    paused = true;
    *dbg << "Queue paused!\n";
    //Choose an icon/label for the toggle in the main window
    mainwindow->UpdatePlayPauseTool();
}

void MidiDriver::PauseOnNextBar(){
    *err << "Pausing on next bar is not yet implemented.\n";
}

void MidiDriver::Sync(){
    *dbg << "Syncing.\n";
    //Clear the queue, INCLUDING NOTEOFFS (1).
    ClearQueue(1);
    //Turn off all notes immidiatelly
    AllNotesOff();
    //Sync the tick
    tick = GetTick();
    tick++;
    //Update the queue
    UpdateQueue(1);
    //Indicate graphically a starting bar
    mainwindow->FlashTempoStart();
}

snd_seq_tick_time_t MidiDriver::GetTick() {
    //Not much to explain, that's just a constant formule to get the current tick on the queue.
  snd_seq_queue_status_t *status;
  snd_seq_tick_time_t current_tick;
  snd_seq_queue_status_malloc(&status);
  snd_seq_get_queue_status(seq_handle, queueid, status);
  current_tick = snd_seq_queue_status_get_tick_time(status);
  snd_seq_queue_status_free(status);
  return(current_tick);
}


void MidiDriver::ContinueQueue(){

    //Clear the queue, INCLUDING NOTEOFFS (1).
    ClearQueue(1);
    //AllNotesOff(); // here: NOT! might cause artefacts.
    //Sync the tick
    tick = GetTick();
    //Update the queue
    UpdateQueue(1);
    //Indicate graphically a starting bar
    mainwindow->FlashTempoStart();
    //Continue the queue, draining it before and after..
    snd_seq_drain_output(seq_handle);
    int i = snd_seq_continue_queue(seq_handle,queueid,NULL) ;
   if (i==-11) {//EAGAIN error. May happen sometimes unpredictably. Usually repeating request helps.
        *dbg <<  i<< ENDL;
   }
    snd_seq_drain_output(seq_handle);
    //Remember the state
    paused = false;
    *dbg << "Queue unpaused!\n";
    
    //Choose an icon/label for the toggle in the main window
    mainwindow->UpdatePlayPauseTool();
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

    //Wel, most sequencers use an AllNotesOff control event and then automatically end all notes. That would be more elegant way to achieve
    //that. But as for now I didn't manage to find out how to sent such event, the solutions is less elegant, but much more obvious.
    snd_seq_event_t ev;
    
    for (int ch = 0; ch < 16; ch++)
    for (int x = 0; x < 128; x++){
        //Send a noteoff on channel ch for note x
        snd_seq_ev_clear(&ev);
        snd_seq_ev_set_source(&ev,output_port);
        snd_seq_ev_set_subs(&ev);
        snd_seq_ev_set_direct(&ev);
        snd_seq_ev_set_noteoff(&ev,ch,x,0);
        snd_seq_event_output(seq_handle,&ev);
    }
    //Drain output, do not put these events on the queue
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::UpdateQueue(bool do_not_lock_threads){
    if(!do_not_lock_threads) gdk_threads_enter(); //for safety. any calls to GUI will be thread-protected
    snd_seq_event_t ev;
    Sequencer* seq;

    //For each sequencer...
    for (unsigned int n = 0; n < sequencers.size(); n++){
        if(sequencers[n] == NULL) continue; //seems this sequencer was removed, so proceed to next one.

        //Shortcut pointer to the sequencer we are currently dealing with.
        seq = sequencers[n];
        
        //Update the seq's PlayOnce phase.
        //Phase 3 means it has already been played, and it has just ended, so now we'll just turn it off, by setting phase back to initial state: 0.
        if(seq->GetPlayOncePhase() == 3) seq->SetPlayOncePhase(0);
        //If phase is set to 1, this means this sequencer is about to be played once right now, so we'll change the phase to 2 (to change the colour
        //in main window corresponding to this sequencer). The last_palyed_note is set to 0, to make sure that when a loooong sequence (more
        //than one bar long) is played once, it's played from it's beggining
        if(seq->GetPlayOncePhase() == 1) {seq->SetPlayOncePhase(2); seq->last_played_note = 0;}

        //If neither the sequencer is turned on nor it's in 2nd phase, proceed to next sequencer, because there is nothing to do with this one.
        if (!(seq->GetOn() || seq->GetPlayOncePhase() == 2)) continue; 

        //OK, and now we proceed all notes from one sequencer.
        //First check the length:
        if (seq->length<=1){
            //Length is smaller or equal to 1, so we play the same sequence several times in a bar

            //Calculate how many times we play this sequence in one bar
            double howmanytimes = (double)1.0/(seq->length);
            //Calculate duration of a singular note.
            int duration = ((double)(TICKS_PER_NOTE / seq->resolution))*seq->length;
            //Local tick is used for calculation in one repetition, at beggining copy it from the main tick.
            int local_tick = tick;
            //Repeat this as many times as we'll play the melody in one bar:
            for (int i = 0; i < howmanytimes; i++){
                //Take every note from this sequencer
                for (int x = 0; x < sequencers[n]->resolution; x++) {
                    for(int C = 0; C < 6; C++){
                        //If the note is inactive, take next note.
                        if(!(seq->GetActivePatternNote(x,C))) continue;
                        //Get the pitch of that note.
                        int pitch = seq->GetNoteOfChord(C);
                        *dbg << "outputting note " << pitch << ",\n";
                        //Create a new event (clear it)...
                        snd_seq_ev_clear(&ev);
                        //Fill it with note data
                        snd_seq_ev_set_note(&ev, seq->GetChannel() - 1, pitch, seq->GetVolume(), duration);
                        //Schedule it in appropriate momment in time (rather: tick, not time), putting it on a queue
                        snd_seq_ev_schedule_tick(&ev, queueid, 0, local_tick + x * duration);
                        //Direct it ti output port, to all it's subscribers
                        snd_seq_ev_set_source(&ev, output_port);
                        snd_seq_ev_set_subs(&ev);
                        //Output the event (but it stays at the queue.)
                        snd_seq_event_output_direct(seq_handle, &ev);
                    }
                }
                //If the sequencer had to play it's melody ONCE, then mark it as it has already played, and break the loop, because we won't repeat the melody (it had to be played ONCE!)
                if(seq->GetPlayOncePhase() == 2) {seq->SetPlayOncePhase(3);break;} 

                //Prepare the local_tick for next repetition.
                local_tick += (double)TICKS_PER_NOTE*seq->length;
            }

        }else{
            //Length is larger than 1, we play one sequence over many bars.
            //TODO: rewrite this, so that it saves a double representing a progress of this sequence (i.e. where to start from next time)


            //Calculate duration of a singular note.
            int duration = ((double)(TICKS_PER_NOTE / seq->resolution))*seq->length;
            //Get the note we'll start playback from.
            int startnote = seq->last_played_note;
            //currnote is used for iteration through consequent notes. x, however, represents the number (required for scheduling) of the note in this sequence IN this BAR.
            int x, currnote = startnote;
            //In this bar we'll play [seq->resolution/seq->length] notes, so repeat the loop (incrementing the iterator) this many times.
            for (x = 0; x < (double)seq->resolution/seq->length;x++){
                //Iterate through all notes using currnote as iterator.
                for(int C = 0; C < 6; C++){
                    //If the note is inactive, take next note.
                    if (!(seq->GetActivePatternNote(currnote, C))) continue;
                    //Get the pitch of that note.
                    int note = seq->GetNoteOfChord(C);
                    //Create a new event (clear it)...
                    snd_seq_ev_clear(&ev);
                    //Fill it with note data
                    snd_seq_ev_set_note(&ev, seq->GetChannel() - 1, note, seq->GetVolume(), duration);
                    //Schedule it in appropriate momment in time (rather: tick, not time), putting it on a queue
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * duration);
                    //Direct it ti output port, to all it's subscribers
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    //Output the event (but it stays at the queue.)
                    snd_seq_event_output_direct(seq_handle, &ev);
                }
                //Increment the iterator - currnote.
                currnote++;
                //If currnote is out of range AND we had to play the sequence once, mark it as played.
                if (currnote >= seq->resolution && seq->GetPlayOncePhase() == 2) seq->SetPlayOncePhase(3);
                //If currnote is out of range, wrap it.
                currnote = currnote % seq->resolution;

            }
            //Remember which note was last played, so we'll continue from next one.
            seq->last_played_note =currnote;
        }

    }

    //Also, playback the metronome notes.
    if (metronome){
        for (int x =0; x < 4;x++){
            if (x == 0){
                    *dbg<<"bar"<<ENDL;
                    //Create a new event (clear it)...
                    snd_seq_ev_clear(&ev);
                    //Fill it with note data
                    snd_seq_ev_set_note(&ev, Config::MetronomeChannel-1, Config::MetronomeHit1Note, Config::MetronomeHit1Velocity, TICKS_PER_QUARTERNOTE);
                    //Schedule it in appropriate momment in time (rather: tick, not time), putting it on a queue
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * TICKS_PER_QUARTERNOTE);
                    //Direct it ti output port, to all it's subscribers
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    //Output the event (but it stays at the queue.)
                    snd_seq_event_output_direct(seq_handle, &ev);
             } else if (Config::MetronomeHit2){
                    *dbg<<"-"<<ENDL;
                    //Create a new event (clear it)...
                    snd_seq_ev_clear(&ev);
                    //Fill it with note data
                    snd_seq_ev_set_note(&ev, Config::MetronomeChannel-1, Config::MetronomeHit2Note, Config::MetronomeHit2Velocity, TICKS_PER_QUARTERNOTE);
                    //Schedule it in appropriate momment in time (rather: tick, not time), putting it on a queue
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * TICKS_PER_QUARTERNOTE);
                    //Direct it ti output port, to all it's subscribers
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    //Output the event (but it stays at the queue.)
                    snd_seq_event_output_direct(seq_handle, &ev);

            }
        }
    }

    //Increment the tick counter (TICKS_PER_NOTE = TICKS_PER_BAR).
    tick+=TICKS_PER_NOTE;

   //send ECHO event to harmonySEQ itself, so it will be notified when the bar finishes, and new notes must be put on the queue.
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_ECHO;
    snd_seq_ev_schedule_tick(&ev,queueid,0,tick);
    snd_seq_ev_set_dest(&ev,snd_seq_client_id(seq_handle),input_port); //here INPUT_PORT is used, so the event will be send just to harmonySEQ itself.
    snd_seq_event_output_direct(seq_handle,&ev);


    if(!do_not_lock_threads)  gdk_threads_leave(); //see note on this functions beggining.

}


void MidiDriver::ProcessInput(){
    
    snd_seq_event_t * ev;

        //Do while there is anything in the input
        do {
        //Obtain the event from input
        snd_seq_event_input(seq_handle,&ev);
        //If we are in passing_midi mode, do pass the event (Well,  unless it's the ECHO, which MUST be caught).
        if(passing_midi&&ev->type!=SND_SEQ_EVENT_ECHO) {PassEvent(ev);continue;}

        //Switch, according to the type.
        switch (ev->type){
            case SND_SEQ_EVENT_NOTEON:
                if (ev->data.note.velocity != 0) {
                    //That's a note-on. It might have triggered events, so let's check for them.
                     gdk_threads_enter(); //just in case.
                    FindAndProcessEvents(Event::NOTE,ev->data.note.note,ev->data.note.channel+1);
                    gdk_threads_leave(); //freeing lock.
                } else {
                    //That's a note-off. We ignore it as for now.
                }
                break;
            case SND_SEQ_EVENT_ECHO:
                *dbg << "ECHO!\n";
                gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                //Indicate graphically that a new bar is starting
                mainwindow->FlashTempoStart();
                gdk_threads_leave(); //freeing lock
                
                //As we got the ECHO event, this means we must prepare the next bar, that is starting right now.
                UpdateQueue();
                
                break;
            case SND_SEQ_EVENT_CONTROLLER:
                //This is a controller event. t might have triggered events, so let's check for them.
                gdk_threads_enter(); //just in case.
                FindAndProcessEvents(Event::CONTROLLER,ev->data.control.param,ev->data.control.channel+1);
                gdk_threads_leave(); //freeing lock
                break;
            case SND_SEQ_EVENT_PITCHBEND:
                //Pithbend event. Pass it through.
                snd_seq_ev_set_source(ev,output_port);
                snd_seq_ev_set_subs(ev);
                snd_seq_ev_set_direct(ev);
                snd_seq_event_output_direct(seq_handle,ev);
                break;
            case SND_SEQ_EVENT_PGMCHANGE:
                //Program change event. Pass it through
                snd_seq_ev_set_source(ev,output_port);
                snd_seq_ev_set_subs(ev);
                snd_seq_ev_set_direct(ev);
                snd_seq_event_output_direct(seq_handle,ev);
                break;
            default:
                //Some unmatched event.
                *dbg << "unknown:(\n";
                break;

        }
        //Get rid of the event.
        snd_seq_free_event(ev);

        //Repeat it while there is any event on input.
    }while (snd_seq_event_input_pending(seq_handle,0)>0);

}
#else
#ifdef __WIN32 /*both 32 and 64-bit enviroments*/







    / * The midi driver for Windows shall go
        *
        *    H
        *    E
        *    R
        *    E
        *
        */






#else

#error Neither Linux nor Windows, sorry.

#endif
#endif
