/* 
 * File:   MidiDriver.cpp
 * Author: cielak
 * 
 * Created on 18 sierpień 2010, 17:14
 */

#include "MidiDriver.h"
#include "error.h"
#include "debug.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "global.h"
extern error* err;
extern debug* dbg;
extern MainWindow *mainwindow;
extern int mainnote, tempo;
extern vector<Sequencer *> sequencers;
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

    //determine event type
    if(volume == 0) snd_seq_ev_set_noteon(&ev,1,pitch,volume);
    else snd_seq_ev_set_noteon(&ev,1,pitch,volume);

    snd_seq_event_output(seq_handle,&ev);
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::InitQueue(){
    queueid = snd_seq_alloc_named_queue(seq_handle,"harmonySEQ queue");
    snd_seq_set_client_pool_output(seq_handle,64);
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
    *dbg << "yep, updating the queue...\n";
    snd_seq_event_t ev;
    for (unsigned int n = 0; n < sequencers.size(); n++){
        if(sequencers[n] == NULL) continue;
        //*dbg << "sequencer " << n << " is ";
        //*dbg << ((sequencers[n]->muted)?"muted":"not muted") << ENDL;
        if (!sequencers[n]->GetOn()) continue;
        for (unsigned int x = 0; x < sequencers[n]->sequence.size();x++){
            snd_seq_ev_clear(&ev);
            int note = sequencers[n]->notes[sequencers[n]->sequence[x]];
            if(sequencers[n]->GetApplyMainNote()) note += mainnote;
            snd_seq_ev_set_note(&ev,sequencers[n]->GetChannel()-1,note,100,TICKS_PER_QUARTERNOTE/2);
            snd_seq_ev_schedule_tick(&ev,queueid,0,tick+x*TICKS_PER_QUARTERNOTE/2);
            snd_seq_ev_set_source(&ev,output_port);
            snd_seq_ev_set_subs(&ev);
            snd_seq_event_output_direct(seq_handle,&ev);
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
        *dbg << "We got an event!!!! And the type is....";
        switch (ev->type){
            case SND_SEQ_EVENT_NOTEON:
                if (ev->data.note.velocity != 0) {
                *dbg << "noteon! (of pitch " << ev->data.note.note << ")\n";
                    mainnote = ev->data.note.note;

                    gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                    mainwindow->SetMainNote(mainnote);
                    gdk_threads_leave(); //freeing lock
                }
                *dbg << "noteoff! (of pitch " << ev->data.note.note << ")\n";
                break;
            case SND_SEQ_EVENT_ECHO:
                *dbg << "ECHO!\n";
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