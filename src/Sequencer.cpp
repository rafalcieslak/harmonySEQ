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

#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "seqHandle.h"
#include "AtomContainer.h"
#include "NoteAtom.h"
#include "NoteSequencer.h"
#include "ControlSequencer.h"
extern debug *dbg;

extern MainWindow* mainwindow;

// TODO: This structure desparately needs a manager class, with a
// mutex guard, and access methods.
std::vector<Sequencer *> seqVector;
bs2::signal<void()> on_sequencer_list_changed;


void ClearSequencers(){
    for(unsigned int x = 0; x < seqVector.size(); x++) delete seqVector[x];

    seqVector.clear();

}

//======begin sequencer class===============

int Sequencer::id_counter = 1;

Sequencer::Sequencer()
    : patterns(0)
{
    id = id_counter++;

    char temp[20];
    snprintf(temp, 20, _("seq %d"), id);
    name = temp;

    resolution = SEQUENCE_DEFAULT_SIZE; //AddPattern() must know the resolution
    Init();
}

Sequencer::Sequencer(Glib::ustring _name)
    : patterns(0)
{
    id = id_counter++;

    name = _name;
    resolution = SEQUENCE_DEFAULT_SIZE; //AddPattern() must know the resolution
    Init();
}

Sequencer::Sequencer(const Sequencer& orig) {
    id = id_counter++;

    name = orig.name;
    on = orig.on;
    resolution = orig.resolution;
    patterns = orig.patterns;
    active_pattern = orig.active_pattern;
    channel = orig.channel;
    length_numerator = orig.length_numerator;
    length_denominator = orig.length_denominator;
    play_from_here_marker = orig.play_from_here_marker;
    play_once_phase = 0;
    //resetting pattern's owner:
    for(unsigned int x = 0; x < patterns.size();x++) patterns[x].SetOwner(this);
}

Sequencer::~Sequencer() {
  *dbg << "Sequencer deleted...\n";
  for(unsigned int x = 0; x < patterns.size(); x++){
    *dbg << "Clearing the owner attribute of pattern " << x << "\n";
    patterns[x].owner = NULL; //Hello kids, daddy's dead!
  }
}

void Sequencer::Init(){

    on = false;
    playing = false;
    active_pattern = 0;
    channel = 1;
    length_numerator = 4;
    length_denominator = 1;
    play_from_here_marker = 0.0;
    play_once_phase = 0;
    resolution = SEQUENCE_DEFAULT_SIZE;

    AddPattern();
}

void Sequencer::SetResolution(int res){
    resolution = res;
    on_parameter_change();
}
int Sequencer::GetResolution(){
    return resolution;
}

void Sequencer::SetLength(int numerator, int denominator){
    length_numerator = numerator;
    length_denominator = denominator;
    on_parameter_change();
}

double Sequencer::GetLength(){
    if (length_denominator == 0) return 1.0;
    return (double)length_numerator/(double)length_denominator;
}

int Sequencer::GetLengthNumerator(){
    return length_numerator;
}

int Sequencer::GetLengthDenominator(){
    return length_denominator;
}

void Sequencer::SetActivePatternNumber(int a){
    if(patterns.size() != 0)
        active_pattern = a%(patterns.size());
    else
        active_pattern = 0;
    on_activepattern_change();
}

int Sequencer::GetActivePatternNumber(){
    return active_pattern;
}

AtomContainer* Sequencer::GetActivePattern(){
    return &patterns[active_pattern];
}

void Sequencer::SetOn(bool m){
    on = m;
    play_once_phase=0;
    on_playstate_change();
}

bool Sequencer::GetOn(){
    return on;
}

bool Sequencer::IsPlaying(){
    return playing;
}

void Sequencer::SetPlaying(bool p){
    playing = p;
}

void Sequencer::SetChannel(int ch){
    channel = ch;
    on_parameter_change();
}

int Sequencer::GetChannel(){
    return channel;
}

void Sequencer::SetName(Glib::ustring nm){
    name = nm;
    on_parameter_change();
}

Glib::ustring Sequencer::GetName(){
    return name;
}

void Sequencer::SetPlayOncePhase(int p){
    play_once_phase = p;
    on_playstate_change();
}

int Sequencer::GetPlayOncePhase(){
    return play_once_phase;
}



int Sequencer::AddPattern(){
    AtomContainer atomcont(this);
    patterns.push_back(atomcont);
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

void Sequencer::ClearPattern(int p){
    if (p >= (int)patterns.size()) return;
    patterns[p].Clear();

}
