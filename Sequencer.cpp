/* 
 * File:   Sequencer.cpp
 * Author: cielak
 * 
 * Created on 29 sierpień 2010, 20:49
 */

#include <gtkmm/widget.h>
#include "debug.h"
#include "Sequencer.h"
#include "MainWindow.h"
extern debug *dbg;

extern MainWindow* mainwindow;
extern vector<Sequencer *> sequencers;


int spawn_sequencer(){
    int n = sequencers.size();

    //init and push to vector
    char temp[20];
    sprintf(temp,_("seq %d"),n);
    Sequencer *new_seq = new Sequencer(temp);
    sequencers.push_back(new_seq);

    //add to main window
    mainwindow->SequencerAdded(n);

    return n;
}



Sequencer::Sequencer()
    : sequence(SEQUENCE_CONST_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    name = SEQUENCER_DEFAULT_NAME;
    Init();
}

Sequencer::Sequencer(Glib::ustring _name)
    : sequence(SEQUENCE_CONST_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    name = _name;
    Init();
}


Sequencer::Sequencer(int seq[],int note[])
    :  sequence(SEQUENCE_CONST_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    for (int x = 0; x < SEQUENCE_CONST_SIZE; x++){
        sequence[x] = seq[x];
        
    }
    for (int x = 0; x < NOTES_CONST_SIZE; x++){
        notes[x] = note[x];
        
    }
    name = SEQUENCER_DEFAULT_NAME;
    Init();
}

Sequencer::Sequencer(int seq[],int note[], Glib::ustring _name)
    :  sequence(SEQUENCE_CONST_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    for (int x = 0; x < SEQUENCE_CONST_SIZE; x++){
        sequence[x] = seq[x];

    }
    for (int x = 0; x < NOTES_CONST_SIZE; x++){
        notes[x] = note[x];

    }

    
    name = _name;
    Init();
}

Sequencer::Sequencer(const Sequencer& orig) {
}

Sequencer::~Sequencer() {
}

void Sequencer::Init(){

    on = false;
    apply_mainnote = true;
    channel = 1;
    *dbg << notes[0]<<ENDL;
    *dbg << GetNotes(0);
    gui_window = new SequencerWindow(this);
    //gui_window->UpdateValues();
    //gui_window->parent = this;

}

int Sequencer::GetNotes(int n){
    
    return notes[n];

}

int Sequencer::GetSequence(int n){

    return sequence[n];
}

void Sequencer::SetOn(bool m){on = m; gui_window->tgl_mute.set_active(m);}
bool Sequencer::GetOn(){return on;}
void Sequencer::SetApplyMainNote(bool a){apply_mainnote = a;gui_window->tgl_apply_mainnote.set_active(a);}
bool Sequencer::GetApplyMainNote(){return apply_mainnote;}
void Sequencer::SetChannel(int ch){channel = ch;gui_window->channel_button.set_value((double)ch);}
int Sequencer::GetChannel(){return channel;}
void Sequencer::SetName(Glib::ustring nm){name = nm;gui_window->set_title(nm);}
Glib::ustring Sequencer::GetName(){return name;}
void Sequencer::ShowWindow(){gui_window->show();}