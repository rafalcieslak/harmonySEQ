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


#include <gtkmm/widget.h>
#include <assert.h>
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "seqHandle.h"
#include "AtomContainer.h"
#include "NoteAtom.h"
extern debug *dbg;

extern MainWindow* mainwindow;
extern vector<Sequencer *> seqVector;

int resolutions[7] = RESOLUTIONS;
double lengths[7] = LENGTHS;


Gtk::TreeModel::Row spawn_sequencer(){
    int n = seqVector.size();

    //init and push to vector
    char temp[20];
    seqHandle h = RequestNewSeqHandle(n);
    sprintf(temp,_("seq %d"),h);
    Sequencer *new_seq = new Sequencer(temp);
    seqVector.push_back(new_seq);
    new_seq->MyHandle = h;

    //add to main window
    return mainwindow->AddSequencerRow(n);
}

Gtk::TreeModel::Row clone_sequencer(int orig){
    int n = seqVector.size();
    
    Sequencer *new_seq = new Sequencer(seqVector[orig]);
    new_seq->SetOn(0);
    seqVector.push_back(new_seq);
    seqHandle h = RequestNewSeqHandle(n);
    new_seq->MyHandle = h;
    return mainwindow->AddSequencerRow(n);

}

void ClearSequencers(){
    for(unsigned int x = 0; x < seqVector.size(); x++) delete seqVector[x];

    seqVector.clear();

}
//======begin sequencer class===============

Sequencer::Sequencer()
    : patterns(0)
{
    name = SEQUENCER_DEFAULT_NAME;
    resolution = SEQUENCE_DEFAULT_SIZE; //AddPattern() must know the resolution
    AddPattern();
    Init();
}

Sequencer::Sequencer(Glib::ustring _name)
    : patterns(0)
{
    name = _name;
    resolution = SEQUENCE_DEFAULT_SIZE; //AddPattern() must know the resolution
    AddPattern();
    Init();
}

Sequencer::Sequencer(const Sequencer *orig) {
    name = orig->name;
    on = orig->on;
    chord = orig->chord;
    resolution = orig->resolution;
    patterns = orig->patterns;
    active_pattern = orig->active_pattern;
    channel = orig->channel;
    length = orig->length;
    last_played_note = orig->last_played_note;
    play_once_phase = 0;
}

Sequencer::~Sequencer() {
}

void Sequencer::Init(){

    on = false;
    active_pattern = 0;
    channel = 1;
    length = 1;
    last_played_note = 0;
    play_once_phase = 0;
    resolution = SEQUENCE_DEFAULT_SIZE;
    chord.SetType(Chord::CHORD_TYPE_TRIAD);
    chord.SetRoot(0);
    chord.SetTriadMode(Chord::CHORD_TRIAD_MODE_MAJOR);
    chord.SetBaseOctave(0);
    chord.SetInversion(0);

    //FIXME: following is temporary. Just for testing purposes!
    patterns[0].Add(new NoteAtom(0,0.125,0));
    patterns[0].Add(new NoteAtom(0.25,0.125,0));
    patterns[0].Add(new NoteAtom(0,0.5,1));
    patterns[0].Add(new NoteAtom(0,0.125,2));
    patterns[0].Add(new NoteAtom(0.5,0.25,3));
    patterns[0].Add(new NoteAtom(0.75,0.25,4));
    
}

void Sequencer::SetResolution(int res){
    resolution = res;
}

void Sequencer::SetLength(double len){
    length  = len;
    
}

double Sequencer::GetLength(){
    return length;
}

void Sequencer::SetActivePattern(int a){
    active_pattern  = a;
    
}

int Sequencer::GetActivePattern(){
    return active_pattern;
}

int Sequencer::GetNoteOfChord(int n){return chord.GetNotePlusBasenote(n);}
void Sequencer::SetOn(bool m){on = m;play_once_phase=0;}
bool Sequencer::GetOn(){return on;}
void Sequencer::SetChannel(int ch){channel = ch;}
int Sequencer::GetChannel(){return channel;}
void Sequencer::SetName(Glib::ustring nm){name = nm;}
Glib::ustring Sequencer::GetName(){return name;}

void Sequencer::SetPlayOncePhase(int p){
    play_once_phase = p;
    
}

int Sequencer::GetPlayOncePhase(){
    return play_once_phase;
}



int Sequencer::AddPattern(){
    AtomContainer notecont;
    patterns.push_back(notecont);
    *dbg<< "Added pattern " << patterns.size() << ".\n";
    return patterns.size()-1;
}

bool Sequencer::RemovePattern(int x){
    patterns.erase(patterns.begin()+x);
    if (active_pattern > x) active_pattern--;
    else if (active_pattern == x) active_pattern = 0;

    *dbg<< "Removed pattern " << x << ".\n";
    return 0;
}

void Sequencer::ChangeActivePattern(int new_one){
    active_pattern = new_one%(patterns.size());
}

void Sequencer::ClearPattern(int p){
    if (p >= patterns.size()) return;
    patterns[p].Clear();

}