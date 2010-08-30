/* 
 * File:   Sequencer.cpp
 * Author: cielak
 * 
 * Created on 29 sierpień 2010, 20:49
 */

#include <gtkmm/widget.h>
#include "debug.h"
#include "Sequencer.h"
extern debug *dbg;

Sequencer::Sequencer()
    : sequence(SEQUENCE_CONST_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
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
    Init();
}

Sequencer::Sequencer(const Sequencer& orig) {
}

Sequencer::~Sequencer() {
}

void Sequencer::Init(){

    muted = false;
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