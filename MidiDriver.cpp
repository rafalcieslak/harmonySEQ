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


#include "MidiDriver.h"
#include "messages.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "global.h"
#include "Event.h"
MidiDriver::MidiDriver() {
    working = false;
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
    snd_seq_set_client_pool_output(seq_handle,2048);
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

void MidiDriver::ClearQueue(){

    *dbg << "clearing queue...\n";
    snd_seq_remove_events_t *re;
    snd_seq_remove_events_malloc(&re);
    snd_seq_remove_events_set_queue(re,queueid);
    snd_seq_remove_events_set_condition(re,SND_SEQ_REMOVE_OUTPUT|SND_SEQ_REMOVE_IGNORE_OFF);
    snd_seq_remove_events(seq_handle,re);
    snd_seq_remove_events_free(re);
}

void MidiDriver::DeleteQueue(){
    if (queueid<0) return;
    *dbg << "stopping queue";
    snd_seq_stop_queue(seq_handle,queueid,NULL);
    snd_seq_free_queue(seq_handle,queueid);
}

void MidiDriver::UpdateQueue(){
    //*dbg << "yep, updating the queue...\n";
    snd_seq_event_t ev;
    for (unsigned int n = 0; n < sequencers.size(); n++){
        if(sequencers[n] == NULL) continue;
        //*dbg << "sequencer " << n << " is ";
        //*dbg << ((sequencers[n]->GetOn())?"on":"off") << ENDL;
        if (!sequencers[n]->GetOn()) continue;
        //ok, and here we proceed all notes from one sequencer.
        //first check the length:
        if (sequencers[n]->length<=1){
            //length is smaller or equal to 1, we play the same sequence several times in a tact
            double howmanytimes = (double)1.0/(sequencers[n]->length);
            //*dbg << "howmanytimes = " << howmanytimes << ENDL;
            int duration = ((double)(TICKS_PER_NOTE / sequencers[n]->resolution))*sequencers[n]->length;
            int local_tick = tick;
            for (int i = 0; i < howmanytimes; i++){
                for (int x = 0; x < sequencers[n]->resolution; x++) {
                    snd_seq_ev_clear(&ev);
                    int note = sequencers[n]->notes[sequencers[n]->sequence[x]];
                    *dbg << note << " ";
                    if (sequencers[n]->GetApplyMainNote()) note += mainnote;
                    snd_seq_ev_set_note(&ev, sequencers[n]->GetChannel() - 1, note, sequencers[n]->GetVolume(), duration);
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, local_tick + x * duration);
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    snd_seq_event_output_direct(seq_handle, &ev);
                }
                local_tick += (double)TICKS_PER_NOTE*sequencers[n]->length;
            }

           *dbg << ENDL;

        }else{
            //length is larger than 1, we play one sequence over many tacts.
            int duration = ((double)(TICKS_PER_NOTE / sequencers[n]->resolution))*sequencers[n]->length;
            int startnote = sequencers[n]->last_played_note;
            //*dbg << "startnote = " << startnote <<ENDL;
            int x;
            for (x = 0; x < (double)sequencers[n]->resolution/sequencers[n]->length;x++){
                    //*dbg << "x = " << x << ENDL;
                    snd_seq_ev_clear(&ev);
                    int note = sequencers[n]->notes[sequencers[n]->sequence[(startnote+x)%sequencers[n]->resolution]];
                    if (sequencers[n]->GetApplyMainNote()) note += mainnote;
                    snd_seq_ev_set_note(&ev, sequencers[n]->GetChannel() - 1, note, sequencers[n]->GetVolume(), duration);
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * duration);
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    snd_seq_event_output_direct(seq_handle, &ev);

            }
            //remember last note
            sequencers[n]->last_played_note = (startnote+x)%sequencers[n]->resolution;
        }
    }
    tick+=TICKS_PER_QUARTERNOTE*4;

    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_ECHO;
    snd_seq_ev_schedule_tick(&ev,queueid,0,tick);
    snd_seq_ev_set_dest(&ev,snd_seq_client_id(seq_handle),input_port);
    snd_seq_event_output_direct(seq_handle,&ev);



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
                    mainnote = ev->data.note.note;

                    FindAndProcessEvents(Event::EVENT_TYPE_NOTE,ev->data.note.note,ev->data.note.channel+1);
                    /*freezed to implement the events system
                    gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                    mainwindow->main_note.set_value(mainnote);
                    gdk_threads_leave(); //freeing lock*/
                    
                }
                *dbg << "noteoff! (of pitch " << ev->data.note.note << ")\n";
                break;
            case SND_SEQ_EVENT_ECHO:
                *dbg << "ECHO!\n";
                gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                mainwindow->FlashTempoStart();
                gdk_threads_leave(); //freeing lock

                UpdateQueue();
                
                break;
            case SND_SEQ_EVENT_CONTROLLER:
                if (ev->data.control.param == 17){
                    *dbg << "tempo controller! " << ev->data.control.value << ENDL;
                    double tmp = 30 + ((double)ev->data.control.value/(double)127.0)*(320-30);

                    gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                    mainwindow->tempo_button.set_value(tmp);
                    gdk_threads_leave(); //freeing lock

                    SetTempo(tmp);

                }else{
                    *dbg << "controller!\n";
                }
                
                FindAndProcessEvents(Event::EVENT_TYPE_CONTROLLER,ev->data.control.param,ev->data.control.channel+1);
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