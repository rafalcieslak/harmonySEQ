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

    muted = true;
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
