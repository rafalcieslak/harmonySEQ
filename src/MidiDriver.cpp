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

#ifdef __linux__
#include <alsa/asoundlib.h>
#endif
#include "global.h"
#include "MidiDriver.h"
#include "messages.h"
#include "MainWindow.h"
#include "NoteSequencer.h"
#include "ControlSequencer.h"
#include "Event.h"
#include "Configuration.h"
#include "SettingsWindow.h"
#include "SequencerWidget.h"
#include "NoteAtom.h"

extern int running;
extern bool diodes_disabled;

MidiDriver::MidiDriver() {
    working = false; //at the beggining the driver is not active, it isn't working
    paused = false; //and it's not paused at start

    //First, we try to open the driver: starting, connectig to system etc.
    Open();
    //And then we prepare the queue.
    InitQueue();
    //Finally, we'll set the tempo to it's current value.
    SetTempo(tempo);
    
    diode_event_id_next = 0;
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

    //If we got so far, then it seems everything gone well and we have now working sequencer ports.
    working = true;
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
    snd_seq_event_output(seq_handle,&ev);
    snd_seq_drain_output(seq_handle);

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
    snd_seq_event_output(seq_handle,&ev);
    snd_seq_drain_output(seq_handle);

}

void MidiDriver::SendNoteEvent(int channel, int pitch, int velocity, int duration_ms){
    SendNoteOnEventImmediatelly(channel,pitch,velocity);
    Glib::signal_timeout().connect_once(sigc::bind(sigc::mem_fun(*this,&MidiDriver::SendNoteOffEventImmediatelly),channel,pitch),duration_ms);
}

void MidiDriver::ScheduleNote(int channel, int tick_time, int pitch, int velocity, int length){
    //Glib::Timer T;
    //unsigned long int t;
        snd_seq_event_t ev;
        //Create a new event (clear it)...
        snd_seq_ev_clear(&ev);
        //Fill it with note data
        snd_seq_ev_set_note(&ev, channel, pitch, velocity, length);
        //Schedule it in appropriate moment in time (rather: tick, not time), putting it on a queue
        snd_seq_ev_schedule_tick(&ev, queueid, 0, tick_time);
        //Direct it ti output port, to all it's subscribers
        snd_seq_ev_set_source(&ev, output_port);
        snd_seq_ev_set_subs(&ev);
        //Output the event (but it stays at the queue.)
        snd_seq_event_output(seq_handle, &ev);
   // T.elapsed(t);if(t>1000) *err << "Warning: sending note took more than 1ms (" <<(int) t << " us)." << ENDL;
        snd_seq_free_event(&ev);
}

void MidiDriver::ScheduleCtrlEventSingle(int channel, int tick_time, int ctrl_no, int value){
    //Glib::Timer T;
    //unsigned long int t;
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);

    snd_seq_ev_set_controller(&ev, channel, ctrl_no, value);
    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick_time);
    snd_seq_ev_set_source(&ev, output_port);
    snd_seq_ev_set_subs(&ev);

    snd_seq_event_output(seq_handle, &ev);
    //T.elapsed(t);if(t>1000) *err << "Warning ctrl event took more than 1ms (" << (int)t << " us)." << ENDL;
    snd_seq_free_event(&ev);
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

void MidiDriver::ScheduleDiodeEvent(DiodeType type, seqHandle handle, int tick_time, double time, int value, int color, int max_res){
    if(diodes_disabled) return;
    DiodeMidiEvent diodeev(type,time,value,color,max_res);
    diode_events.insert(std::make_pair<int,DiodeMidiEvent>(diode_event_id_next,diodeev));
    
    snd_seq_event_t ev;
    
    snd_seq_ev_clear(&ev);
    ev.type = SND_SEQ_EVENT_USR0; //Diode ON
    ev.data.raw32.d[0] = handle; //seq handle
    ev.data.raw32.d[1] = diode_event_id_next; //diode id
    ev.data.raw32.d[2] = 0;//unused
    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick_time);
    snd_seq_ev_set_dest(&ev, snd_seq_client_id(seq_handle), input_port); //here INPUT_PORT is used, so the event will be send just to harmonySEQ itself.
    snd_seq_event_output_direct(seq_handle, &ev);
    snd_seq_ev_clear(&ev);

    snd_seq_free_event(&ev);
    
    diode_event_id_next++;
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
    snd_seq_set_client_pool_output(seq_handle,65536);//8192);
    //Clear the ticks counter.
    tick = 0;
}

void MidiDriver::SetTempo(double bpm){
    //Create a new tempo-class (strange usage, but that's how ALSA want to receive the tempo)
    snd_seq_queue_tempo_t* queue_tempo;
    snd_seq_queue_tempo_malloc(&queue_tempo);
    //Calculate the real tempo value from the BPM
    snd_seq_queue_tempo_set_tempo(queue_tempo,((double)6e7/((double)bpm*4.0)));
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
    //All diodes off...
    //mainwindow->seqWidget.Diodes_AllOff();
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
    //All diodes off...
    mainwindow->seqWidget.DeacivateAllDiodes();
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
    
    //also, clear the map.
    diode_events.clear();
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

double RoundTimeDouble(double marker){
    
            long long int temp = (long long int)((double)marker*(double)1e9);
            return ((double)(temp))/(double)(1e9);
}

double Wrap(double x){
   return x - (int) x;
}

void MidiDriver::UpdateQueue(bool do_not_lock_threads){
    
    if(!do_not_lock_threads) gdk_threads_enter(); //for safety. any calls to GUI will be thread-protected
    snd_seq_event_t ev;
    Sequencer* seq;

    //For each sequencer...
    for (unsigned int n = 0; n < seqVector.size(); n++){
        if(seqVector[n] == NULL) continue; //seems this sequencer was removed, so proceed to next one.

        //Shortcut pointer to the sequencer we are currently dealing with.
        seq = seqVector[n];

        bool needs_to_have_its_row_refreshed = false;

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
            needs_to_have_its_row_refreshed = true;
        }
        //If phase is set to 1, this means this sequencer is about to be played once right now, so we'll change the phase to 2 (to change the colour
        //in main window corresponding to this sequencer). The last_played_note is set to 0, to make sure that when a loooong sequence (more
        //than one bar long) is played once, it's played from it's beggining
        if(seq->GetPlayOncePhase() == 1) {
            seq->SetPlayOncePhase(2);
            seq->play_from_here_marker = 0.0;
            needs_to_have_its_row_refreshed = true;
        }

        //Output the notes only if the sequencer is on, or it's in 2nd phase. Otherwise skip the note outputting routines....
        if (seq->GetOn() || seq->GetPlayOncePhase() == 2){

            //Selecting diode color according to mode
            int diode_colour;
            if (seq->GetPlayOncePhase() == 2) diode_colour = 1;
            else diode_colour = 0;
            
            
            //The time (in ticks)  how long one sequence repetition will last. Note it can be larger than TICKS_PER_NOTE and that's OK.
            double sequence_time = TICKS_PER_NOTE*seq->GetLength();
            //Local tick is the tick at which this sequence repetition had began. Occasionally it may be lower than tick, for
            //example in case this repetition has already been played in 1/3.
            double local_tick = tick-seq->play_from_here_marker*TICKS_PER_NOTE*seq->GetLength();
            //Shortcut to the pattern we'll play.
            AtomContainer* pattern = seq->GetActivePattern();
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
                int X = -1;
                while(1){
                    X++;
                    if(seq->GetPlayOncePhase() == 2 && X == size) {seq->SetPlayOncePhase(3); break;}
                    //*err << "at note " << X << ", X/size = " << X/size << ENDL; 
                    Atom* atm = ((*pattern)[X%size]);
                    if(atm->time + (X/size)*1.0 >= start_marker && s == -1) s = X ;
                    if(atm->time + (X/size)*1.0 < end_marker) e = X;
                    if(atm->time + (X/size)*1.0 >= end_marker) break;
                }
            }else{
                //if empty thing was played once...
                if(seq->GetPlayOncePhase() == 2) seq->SetPlayOncePhase(3);
            }
            
            //We know which atoms to play, so lets play them.
            if(e != -1 && s != -1 && e>=s){
                      //Determine whether to output notes or control messages
                      if(seq->GetType() == SEQ_TYPE_NOTE){
                          
                                 NoteSequencer* noteseq = dynamic_cast<NoteSequencer*>(seq);
                                 
                                  for(int V = s; V<=e;V++){
                                        note = dynamic_cast<NoteAtom*>((*pattern)[V%size]);
                                        int pitch = noteseq->GetNoteOfChord(note->pitch);
                                        ScheduleNote(seq->GetChannel()-1,local_tick + (V/size)*sequence_time + note->time*TICKS_PER_NOTE*seq->GetLength(),pitch,note->velocity,note->length*TICKS_PER_NOTE*seq->GetLength());
                                        
                                        //each note shall have correspoinding diode event.
                                        ScheduleDiodeEvent(DIODE_TYPE_NOTE, seq->MyHandle, local_tick + (V / size) * sequence_time + note->time * TICKS_PER_NOTE * seq->GetLength(), note->time, note->pitch, diode_colour);
                        
                                  } 
                      }else if(seq->GetType() == SEQ_TYPE_CONTROL){
                          
                                 ControlSequencer* ctrlseq = dynamic_cast<ControlSequencer*>(seq);
                          
                                  for(int V = s; V<=e;V++){
                                        ctrl = dynamic_cast<ControllerAtom*> ((*pattern)[V % size]);
                                        if(ctrl->slope_type == SLOPE_TYPE_FLAT){
                                            ScheduleCtrlEventSingle(seq->GetChannel()-1, local_tick + (V/size)*sequence_time + ctrl->time*TICKS_PER_NOTE*seq->GetLength(),ctrlseq->controller_number,ctrl->value);
                                        }else if(ctrl->slope_type == SLOPE_TYPE_LINEAR){
                                            
                                            
                                            //TODO very important! output slopes ONLY to next bar!
                                            
                                            
                                            next_ctrl = dynamic_cast<ControllerAtom*>((*pattern)[(V+1)%size]);
                                            double nextctrl_time = next_ctrl->time;
                                            if(V==size-1) nextctrl_time += 1.0; //if this is a last note in pattern, make sure to schedule it's slope later!
                                            ScheduleCtrlEventLinearSlope(
                                                                         seq->GetChannel()-1,
                                                                         ctrlseq->controller_number,
                                                                         local_tick + (V/size)*sequence_time + ctrl->time*TICKS_PER_NOTE*seq->GetLength(),
                                                                         ctrl->value,
                                                                         local_tick + (V/size)*sequence_time + nextctrl_time*TICKS_PER_NOTE*seq->GetLength(),
                                                                         next_ctrl->value
                                            );
                                            
                                        }else{
                                            //something wrong.
                                        }
                                        
                                        //each note shall have correspoinding diode event.
                                        ScheduleDiodeEvent(DIODE_TYPE_CTRL, seq->MyHandle, local_tick + (V / size) * sequence_time + ctrl->time * TICKS_PER_NOTE * seq->GetLength(), ctrl->time, ctrl->value, diode_colour);
                        
                                  }
                              
                      }else{
                              *err << "Sequencer is neither note nor control type. Don't bother reporting this to harmonySEQ developers. This error message will never display, so if you see it, it means you must have broken something intentionally.\n";
                      }
            }
            double play_from_here_marker = Wrap(end_marker);
            //rounding to ensure sync...
            if(play_from_here_marker > -0.000000001 && play_from_here_marker < 0.000000001) play_from_here_marker = 0.0;
            seq->play_from_here_marker = play_from_here_marker;
        } //[If seq is on or in 2nd phase]

        //Finally, no matter whether the sequencer was on or not...
        
        //Refreshing the sequencer's row in main window, if needed
        if (needs_to_have_its_row_refreshed){
            if ( seq->my_row) mainwindow->RefreshRow(seq->my_row);
            if (mainwindow->seqWidget.selectedSeq == seq->MyHandle) mainwindow->seqWidget.UpdateOnOffColour();
        }
             

        //And proceed to next sequencer.
    }

    //Also, playback the metronome notes.
    if (metronome){
        for (int x =0; x < 4;x++){
            if (x == 0){
                    //Create a new event (clear it)...
                    snd_seq_ev_clear(&ev);
                    //Fill it with note data
                    snd_seq_ev_set_note(&ev, Config::Metronome::Channel-1, Config::Metronome::Hit1Note, Config::Metronome::Hit1Velocity, TICKS_PER_QUARTERNOTE);
                    //Schedule it in appropriate momment in time (rather: tick, not time), putting it on a queue
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * TICKS_PER_QUARTERNOTE);
                    //Direct it ti output port, to all it's subscribers
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    //Output the event (but it stays at the queue.)
                    snd_seq_event_output_direct(seq_handle, &ev);
             } else if (Config::Metronome::Hit2){
                    //Create a new event (clear it)...
                    snd_seq_ev_clear(&ev);
                    //Fill it with note data
                    snd_seq_ev_set_note(&ev, Config::Metronome::Channel-1, Config::Metronome::Hit2Note, Config::Metronome::Hit2Velocity, TICKS_PER_QUARTERNOTE);
                    //Schedule it in appropriate momment in time (rather: tick, not time), putting it on a queue
                    snd_seq_ev_schedule_tick(&ev, queueid, 0, tick + x * TICKS_PER_QUARTERNOTE);
                    //Direct it ti output port, to all it's subscribers
                    snd_seq_ev_set_source(&ev, output_port);
                    snd_seq_ev_set_subs(&ev);
                    //Output the event (but it stays at the queue.)
                    snd_seq_event_output_direct(seq_handle, &ev);

            }
            snd_seq_free_event(&ev);
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
    snd_seq_free_event(&ev);

    //T.elapsed(t);
    //*err <<"end :" << (int)t << ENDL;
    
    if(!do_not_lock_threads)  gdk_threads_leave(); //see note on this functions beggining.

    //*dbg << "harmonySEQ will now try to drain ALSA midi output. If it hangs right after outputting this message, this means ALSA has done something wrong.\n";
    /**Note, that if there is A LOT of notes on the queue, the following call will take some time. However, it does not use CPU, and we have already unlocked gtk threads, so it can be safely called.*/
    snd_seq_drain_output(seq_handle);
    //*dbg << "Output succesfully drained.\n";
    //T.elapsed(t);
    //*err <<"end + drain :" << (int)t << ENDL;
    
}


void MidiDriver::ProcessInput(){
    
    snd_seq_event_t * ev;
    int h, id;
    std::map<int,DiodeMidiEvent>::iterator it;
    DiodeMidiEvent diodev(DIODE_TYPE_NOTE,0,0,0);
    
        //Do while there is anything in the input
        do {
        //Obtain the event from input
        snd_seq_event_input(seq_handle,&ev);
        //If we are in passing_midi mode, do pass the event (Well,  unless it's the ECHO, which MUST be caught).
        if(passing_midi&&ev->type!=SND_SEQ_EVENT_ECHO&&ev->type!=SND_SEQ_EVENT_USR0) {PassEvent(ev);continue;}

        //int i, c;
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
            case SND_SEQ_EVENT_USR0:
                h = ev->data.raw32.d[0];
                id = ev->data.raw32.d[1];
                if (mainwindow->seqWidget.selectedSeq == h){
                    it = diode_events.find(id);
                    diodev = (*it).second;
                    if (it == diode_events.end()) break; //in case such event was not registered, avoid crashes
                    if (diodev.type == DIODE_TYPE_NOTE && mainwindow->seqWidget.selectedSeqType == SEQ_TYPE_CONTROL) break; //mismatched type
                    if (diodev.type == DIODE_TYPE_CTRL && mainwindow->seqWidget.selectedSeqType == SEQ_TYPE_NOTE) break;    //mismatched type
                    gdk_threads_enter(); //to interact with gui thread we MUST lock it's thread
                        mainwindow->seqWidget.ActivateDiode(diodev);
                    gdk_threads_leave(); //freeing lock
                    diode_events.erase(it);
                }
                break;
            default:
                //Some unmatched event.
                *err << "unknown event recieved:(\n";
                break;

        }
        //Get rid of the event.
        snd_seq_free_event(ev);

        //Repeat it while there is any event on input.
    }while (snd_seq_event_input_pending(seq_handle,0)>0);

}
#else
#ifdef __WIN32 /*both 32 and 64-bit enviroments*/







    /* The midi driver for Windows shall go
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
