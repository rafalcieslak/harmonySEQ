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
#include "MidiDriver.h"
#include "messages.h"
#include "MainWindow.h"
#include "Sequencer.h"
//#include "global.h"
#include "Event.h"
MidiDriver::MidiDriver() {
    working = false;
    paused = false;
    to_be_paused = false;
    Open();
    InitQueue();
    SetTempo(tempo);
}

MidiDriver::MidiDriver(const MidiDriver& orig) {
    
}

MidiDriver::~MidiDriver() {

}

void MidiDriver::Open(){
    //try to open sequencer port
    int e = snd_seq_open(&seq_handle,"default",SND_SEQ_OPEN_DUPLEX,0);

    //catch for errors
    if (e < 0){
        *err << _("Failed to open ALSA sequencer.\n");
        return;
    }
    
    //set client name
    snd_seq_set_client_name(seq_handle,"harmonySEQ");

    //try to create the port we use for midi output
    output_port = snd_seq_create_simple_port(seq_handle,"harmonySEQ output",SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,SND_SEQ_PORT_TYPE_APPLICATION);

    //catch errors
    if (output_port < 0){
        *err << _("Failed to create output port.\n");
        return;
    }

    //try to create the port we use for midi output
    input_port = snd_seq_create_simple_port(seq_handle,"harmonySEQ input",SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,SND_SEQ_PORT_TYPE_APPLICATION);

    //catch errors
    if (input_port < 0){
        *err << _("Failed to create input port.\n");
        return;
    }

    //if we get so far, seems everything gone well and we have now a working sequencer port.
    working = true;
    *dbg << _("Alsa midi driver init successfull.\n");
}

void MidiDriver::SendNoteEvent(int pitch, int volume){
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);
    snd_seq_ev_set_source(&ev,output_port);
    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_ev_set_noteon(&ev,1,pitch,volume);

    snd_seq_event_output(seq_handle,&ev);
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::PassEvent(snd_seq_event_t* ev){
    *dbg << "passing an event...";
    snd_seq_ev_set_source(ev,output_port);
    snd_seq_ev_set_subs(ev);
    snd_seq_ev_set_direct(ev);
    snd_seq_event_output(seq_handle,ev);
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::InitQueue(){
    queueid = snd_seq_alloc_named_queue(seq_handle,"harmonySEQ queue");
    snd_seq_set_client_pool_output(seq_handle,8192);
    tick = 0;
}

void MidiDriver::SetTempo(double bpm){
    snd_seq_queue_tempo_t* queue_tempo;
    snd_seq_queue_tempo_malloc(&queue_tempo);
    snd_seq_queue_tempo_set_tempo(queue_tempo,((double)6e7/(double)bpm));
    snd_seq_queue_tempo_set_ppq(queue_tempo,TICKS_PER_QUARTERNOTE);
    snd_seq_set_queue_tempo(seq_handle,queueid,queue_tempo);
    tempo = bpm;
    snd_seq_queue_tempo_free(queue_tempo);
}

void MidiDriver::PauseQueueImmediately(){
    //snd_seq_event_t ev;
   //snd_seq_ev_clear(&ev);
    //ev.type = SND_SEQ_EVENT_RESET; //may result in a all-note-off
    //PassEvent(&ev);
    
    snd_seq_stop_queue(seq_handle,queueid,NULL);
    snd_seq_drain_output(seq_handle);

    AllNotesOff();

    paused = true;
    *dbg << "Queue paused!\n";
    mainwindow->UpdatePlayPauseButton();
}

void MidiDriver::PauseOnNextTact(){
    *err << "Pausing on next track is not yet implemented.\n";
}

void MidiDriver::Sync(){
    *dbg << "Syncing.\n";
    ClearQueue(1); //remove also noteoffs!
    AllNotesOff(); //
    tick = GetTick();
    tick++;
    UpdateQueue(1);
    mainwindow->FlashTempoStart();//to indicate a starting tact
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


void MidiDriver::ContinueQueue(){

    *dbg << "Syncing.\n";
    ClearQueue(1); //remove also noteoffs!
    //AllNotesOff(); // here: NOT!
    tick = GetTick();
    UpdateQueue(1);
    mainwindow->FlashTempoStart();//to indicate a starting tact
    
    snd_seq_drain_output(seq_handle);
    int i = snd_seq_continue_queue(seq_handle,queueid,NULL) ;
   *dbg <<  i<< ENDL;
   if (i==-11) {//eagain
       i = snd_seq_continue_queue(seq_handle,queueid,NULL) ;
        *dbg <<  i<< ENDL;
   }
    snd_seq_drain_output(seq_handle);

    paused = false;
    *dbg << "Queue unpaused!\n";

    mainwindow->UpdatePlayPauseButton();
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
    if (queueid<0) return;
    *dbg << "stopping queue";
    snd_seq_stop_queue(seq_handle,queueid,NULL);
    snd_seq_free_queue(seq_handle,queueid);
}


void MidiDriver::AllNotesOff(){
    snd_seq_event_t ev;
    
    for (int ch = 0; ch < 16; ch++)
    for (int x = 0; x < 128; x++){
        snd_seq_ev_clear(&ev);
        snd_seq_ev_set_source(&ev,output_port);
        snd_seq_ev_set_subs(&ev);
        snd_seq_ev_set_direct(&ev);
        snd_seq_ev_set_noteoff(&ev,ch,x,0);
        snd_seq_event_output(seq_handle,&ev);
    }
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::UpdateQueue(bool do_not_lock_threads){
    if(!do_not_lock_threads) gdk_threads_enter(); //for safety. any calls to GUI will be thread - protected
    snd_seq_event_t ev;
    Sequencer* seq;

    for (unsigned int n = 0; n < sequencers.size(); n++){
        if(sequencers[n] == NULL) continue; //seems this sequencer was removed

        seq = sequencers[n];
        if(seq->GetPlayOncePhase() == 3) seq->SetPlayOncePhase(0);
        if(seq->GetPlayOncePhase() == 1) seq->SetPlayOncePhase(2);
        if (!(seq->GetOn() || seq->GetPlayOncePhase() == 2)) continue; //if it's not turned on, take next sequencer

        //ok, and here we proceed all notes from one sequencer.
        //first check the length:
        if (seq->length<=1){
            //length is smaller or equal to 1, we play the same sequence several times in a tact

            double howmanytimes = (double)1.0/(seq->length); //how many times we play this sequence in one tact
            int duration = ((double)(TICKS_PER_NOTE / seq->resolution))*seq->length;
            int local_tick = tick;
            for (int i = 0; i < howmanytimes; i++){
                for (int x = 0; x < sequencers[n]->resolution; x++) {
                    snd_seq_ev_clear(&ev);
                    int note = seq->GetNoteOfChord(seq->sequence[x]);
                    if (seq->GetApplyMainNote()) note += mainnote;
                    snd_seq_ev_set_note(&ev, seq->GetChannel() - 1, note, seq->GetVolume(), duration);
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, local_tick + x * duration);
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    snd_seq_event_output_direct(seq_handle, &ev);

                }
                if(seq->GetPlayOncePhase() == 2) {seq->SetPlayOncePhase(3);break;} //had to be played ONCE, so we do not repeat the loop
                
                local_tick += (double)TICKS_PER_NOTE*seq->length;
            }

        }else{
            //length is larger than 1, we play one sequence over many tacts.
            //TODO: rewrite this, so that it saves a double representing a progress of this sequence (i.e. where to start from next time)

             
            int duration = ((double)(TICKS_PER_NOTE / seq->resolution))*seq->length;
            int startnote = seq->last_played_note;
            //*dbg << "startnote = " << startnote <<ENDL;
            int x, currnote = startnote;
            for (x = 0; x < (double)seq->resolution/seq->length;x++){
                    //*dbg << "x = " << x << ENDL;
                    snd_seq_ev_clear(&ev);
                    int note = seq->GetNoteOfChord(seq->sequence[(currnote)]);
                    if (seq->GetApplyMainNote()) note += mainnote;
                    snd_seq_ev_set_note(&ev, seq->GetChannel() - 1, note, seq->GetVolume(), duration);
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * duration);
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    snd_seq_event_output_direct(seq_handle, &ev);

                    currnote++;
                     if(currnote>=seq->resolution&&seq->GetPlayOncePhase() == 2) seq->SetPlayOncePhase( 3);
                    currnote = currnote%seq->resolution;

            }
            //remember last note
            seq->last_played_note =currnote;
        }

    }

    tick+=TICKS_PER_NOTE;

   //send ECHO event
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_ECHO;
    snd_seq_ev_schedule_tick(&ev,queueid,0,tick);
    snd_seq_ev_set_dest(&ev,snd_seq_client_id(seq_handle),input_port);
    snd_seq_event_output_direct(seq_handle,&ev);


    if(!do_not_lock_threads)  gdk_threads_leave(); //see note on above

}


void MidiDriver::ProcessInput(){
    
    snd_seq_event_t * ev;

        do {
        snd_seq_event_input(seq_handle,&ev);
        if(passing_midi&&ev->type!=SND_SEQ_EVENT_ECHO) {PassEvent(ev);continue;}
        //*dbg << "We got an event!!!! And the type is....";
        switch (ev->type){
            case SND_SEQ_EVENT_NOTEON:
                if (ev->data.note.velocity != 0) {
                *dbg << "noteon! (of pitch " << ev->data.note.note << ")\n";

                     gdk_threads_enter(); //just in case.
                    FindAndProcessEvents(Event::NOTE,ev->data.note.note,ev->data.note.channel+1);
                    gdk_threads_leave(); //freeing lock
                    
                } else {
                    *dbg << "noteoff! (of pitch " << ev->data.note.note << ")\n";
                }
                break;
            case SND_SEQ_EVENT_ECHO:
                *dbg << "ECHO!\n";
                gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                mainwindow->FlashTempoStart();
                gdk_threads_leave(); //freeing lock

                UpdateQueue();
                
                break;
            case SND_SEQ_EVENT_CONTROLLER:
                 *dbg << "controller!\n";

                gdk_threads_enter(); //just in case.
                FindAndProcessEvents(Event::CONTROLLER,ev->data.control.param,ev->data.control.channel+1);
                gdk_threads_leave(); //freeing lock
                break;
            case SND_SEQ_EVENT_PITCHBEND:
                //pass it through
                *dbg << "pitchbend! passing through\n";
                snd_seq_ev_set_source(ev,output_port);
                snd_seq_ev_set_subs(ev);
                snd_seq_ev_set_direct(ev);
                snd_seq_event_output_direct(seq_handle,ev);
                break;
            case SND_SEQ_EVENT_PGMCHANGE:
                //pass it through
                *dbg << "program change! passing through\n";
                snd_seq_ev_set_source(ev,output_port);
                snd_seq_ev_set_subs(ev);
                snd_seq_ev_set_direct(ev);
                snd_seq_event_output_direct(seq_handle,ev);
                break;
            default:
                //some unmatched event
                *dbg << "unknown:(\n";
                break;

        }
        snd_seq_free_event(ev);

    }while (snd_seq_event_input_pending(seq_handle,0)>0);

}