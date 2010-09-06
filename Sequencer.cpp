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


#include <gtkmm/widget.h>
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
extern debug *dbg;

extern MainWindow* mainwindow;
extern vector<Sequencer *> sequencers;


Gtk::TreeModel::iterator spawn_sequencer(){
    int n = sequencers.size();

    //init and push to vector
    char temp[20];
    sprintf(temp,_("seq %d"),n);
    Sequencer *new_seq = new Sequencer(temp);
    sequencers.push_back(new_seq);

    //add to main window
    return mainwindow->AddSequencerRow(n);

    
}

Gtk::TreeModel::iterator clone_sequencer(int orig){
    int n = sequencers.size();
    
    Sequencer *new_seq = new Sequencer(sequencers[orig]);
    sequencers.push_back(new_seq);

    return mainwindow->AddSequencerRow(n);

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

Sequencer::Sequencer(const Sequencer *orig) {
    name = orig->name;
    on = orig->on;
    notes = orig->notes;
    sequence = orig->sequence;
    channel = orig->channel;
    apply_mainnote = orig->apply_mainnote;
    gui_window = new SequencerWindow(this);



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
void Sequencer::UpdateGui(){gui_window->UpdateValues();}