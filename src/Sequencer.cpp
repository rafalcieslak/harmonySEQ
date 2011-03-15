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
#include <assert.h>
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "seqHandle.h"
extern debug *dbg;

extern MainWindow* mainwindow;
extern vector<Sequencer *> seqVector;

int resolutions[7] = RESOLUTIONS;
double lengths[7] = LENGTHS;


Gtk::TreeModel::Row spawn_sequencer(){
    int n = seqVector.size();

    //init and push to vector
    char temp[20];
    sprintf(temp,_("seq %d"),n+1);
    Sequencer *new_seq = new Sequencer(temp);
    seqVector.push_back(new_seq);
    new_seq->MyHandle = RequestNewSeqHandle(n+1);

    //add to main window
    return mainwindow->AddSequencerRow(n);
}

Gtk::TreeModel::Row clone_sequencer(int orig){
    int n = seqVector.size();
    
    Sequencer *new_seq = new Sequencer(seqVector[orig]);
    new_seq->SetOn(0);
    seqVector.push_back(new_seq);
    new_seq->MyHandle = RequestNewSeqHandle(n+1);
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
    volume = orig->volume;
    gui_window = new SequencerWindow(this);
    last_played_note = orig->last_played_note;
    play_once_phase = 0;
}

Sequencer::~Sequencer() {
    gui_window->hide();
    delete gui_window;
}

void Sequencer::Init(){

    on = false;
    active_pattern = 0;
    channel = 1;
    length = 1;
    volume = DEFAULT_VOLUME;
    last_played_note = 0;
    play_once_phase = 0;
    resolution = SEQUENCE_DEFAULT_SIZE;
    gui_window = new SequencerWindow(this);
    //gui_window->UpdateValues();
    //gui_window->parent = this;

}

void Sequencer::SetResolution(int res){
    if (res == resolution) return;
    
    if (res < resolution){
        //the new resolution is smaller
        int ratio = resolution/res;
        *dbg << "ensmalling resolution ratio = " << ratio << ENDL;

        for(int s=0; s<patterns.size();s++){
            vector<vector<bool> > new_sequence(res,vector<bool>(6,0));
            assert(ratio>=1);
            int x = 0, i = 0;
            for (; x < resolution;x+=ratio){
                for(int c = 0; c < 6;c++){
                    new_sequence[i][c] = patterns[s][x][c]; //to be checked
                }
                i++;
            }
            patterns[s] = new_sequence;
        }
        resolution = res;
    } else {
        //the new resolution is larger
        int ratio = res/resolution;
        *dbg << "enlarging resolution ratio = " << ratio << ENDL;
        for(int s=0; s<patterns.size();s++){
         vector<vector<bool> > new_sequence(res,vector<bool>(6,0));
            int x = 0;
            for (int p =0; p < resolution;p++){
                for (int i = 0; i < ratio;i++){
                    for(int c = 0; c < 6; c++){
                        new_sequence[x][c]=patterns[s][p][c];
                    }
                    x++;
                }
            }
            patterns[s]=new_sequence;
        }
        resolution = res;
    }


}




int Sequencer::GetNoteOfChord(int n){return chord.GetNotePlusBasenote(n);}
void Sequencer::SetOn(bool m){on = m;play_once_phase=0;gui_window->wMuteToggle.set_active(m);}
bool Sequencer::GetOn(){return on;}
void Sequencer::SetChannel(int ch){channel = ch;gui_window->wChannelButton.set_value((double)ch);}
int Sequencer::GetChannel(){return channel;}
void Sequencer::SetName(Glib::ustring nm){name = nm;gui_window->set_title(nm);}
Glib::ustring Sequencer::GetName(){return name;}
int Sequencer::GetVolume(){return volume;}
void Sequencer::SetVolume(int v){volume = v;gui_window->wVolumeButton.set_value((double)v);}

void Sequencer::SetPlayOncePhase(int p){
    play_once_phase = p;
    if(my_row) mainwindow->RefreshRow(my_row);
}

int Sequencer::GetPlayOncePhase(){
    return play_once_phase;
}

void Sequencer::ShowWindow(){
    gui_window->present();}

void Sequencer::UpdateGui(){gui_window->UpdateValues();}
void Sequencer::UpdateGuiChord(){gui_window->UpdateChord();}


int Sequencer::AddPattern(){
    vector<bool> line(6,false);
    vector<vector<bool> > seq;
    for(int x = 0; x < resolution;x++){
        seq.push_back(line);
    }
    patterns.push_back(seq);
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


int Sequencer::GetPatternNote(int pattern, int n, int c){
    return patterns[pattern][n][c];
}

bool Sequencer::GetActivePatternNote(int n, int c){
    return patterns[active_pattern][n][c];
}

void Sequencer::SetPatternNote(int pattern, int n, int c, bool value){
    patterns[pattern][n][c] = value;

}

void Sequencer::ChangeActivePattern(int new_one){
    active_pattern = new_one%(patterns.size());
    gui_window->UpdatePattern();
}