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
extern debug *dbg;

extern MainWindow* mainwindow;
extern vector<Sequencer *> sequencers;

int resolutions[7] = RESOLUTIONS;
double lengths[7] = LENGTHS;


Gtk::TreeModel::RowReference spawn_sequencer(){
    int n = sequencers.size();

    //init and push to vector
    char temp[20];
    sprintf(temp,_("seq %d"),n);
    Sequencer *new_seq = new Sequencer(temp);
    sequencers.push_back(new_seq);

    //add to main window
    return mainwindow->AddSequencerRow(n);

    
}

Gtk::TreeModel::RowReference clone_sequencer(int orig){
    int n = sequencers.size();
    
    Sequencer *new_seq = new Sequencer(sequencers[orig]);
    new_seq->SetOn(0);
    sequencers.push_back(new_seq);
    return mainwindow->AddSequencerRow(n);

}

void ClearSequencers(){
    for(unsigned int x = 0; x < sequencers.size(); x++) delete sequencers[x];

    sequencers.clear();

}
//======begin sequencer class===============

Sequencer::Sequencer()
    : melodies(0)
{
    name = SEQUENCER_DEFAULT_NAME;
    resolution = SEQUENCE_DEFAULT_SIZE; //AddMelody() must know the resolution
    AddMelody();
    Init();
}

Sequencer::Sequencer(Glib::ustring _name)
    : melodies(0)
{
    name = _name;
    resolution = SEQUENCE_DEFAULT_SIZE; //AddMelody() must know the resolution
    AddMelody();
    Init();
}

Sequencer::Sequencer(const Sequencer *orig) {
    name = orig->name;
    on = orig->on;
    chord = orig->chord;
    resolution = orig->resolution;
    melodies = orig->melodies;
    active_melody = orig->active_melody;
    channel = orig->channel;
    apply_mainnote = orig->apply_mainnote;
    length = orig->length;
    volume = orig->volume;
    gui_window = new SequencerWindow(this);
    last_played_note = orig->last_played_note;
    play_once_phase = 0;
}

Sequencer::~Sequencer() {
}

void Sequencer::Init(){

    on = false;
    apply_mainnote = true;
    active_melody = 0;
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

        for(int s=0; s<melodies.size();s++){
            vector<vector<bool> > new_sequence(res,vector<bool>(6,0));
            assert(ratio>=1);
            int x = 0, i = 0;
            for (; x < resolution;x+=ratio){
                for(int c = 0; c < 6;c++){
                    new_sequence[i][c] = melodies[s][x][c]; //to be checked
                }
                i++;
            }
            melodies[s] = new_sequence;
        }
        resolution = res;
    } else {
        //the new resolution is larger
        int ratio = res/resolution;
        *dbg << "enlarging resolution ratio = " << ratio << ENDL;
        for(int s=0; s<melodies.size();s++){
         vector<vector<bool> > new_sequence(res,vector<bool>(6,0));
            int x = 0;
            for (int p =0; p < resolution;p++){
                for (int i = 0; i < ratio;i++){
                    for(int c = 0; c < 6; c++){
                        new_sequence[x][c]=melodies[s][p][c];
                    }
                    x++;
                }
            }
            melodies[s]=new_sequence;
        }
        resolution = res;
    }


}




int Sequencer::GetNoteOfChord(int n){return chord.GetNote(n);}
void Sequencer::SetOn(bool m){on = m;play_once_phase=0;gui_window->tgl_mute.set_active(m);}
bool Sequencer::GetOn(){return on;}
void Sequencer::SetApplyMainNote(bool a){apply_mainnote = a;}
bool Sequencer::GetApplyMainNote(){return apply_mainnote;}
void Sequencer::SetChannel(int ch){channel = ch;gui_window->channel_button.set_value((double)ch);}
int Sequencer::GetChannel(){return channel;}
void Sequencer::SetName(Glib::ustring nm){name = nm;gui_window->set_title(nm);}
Glib::ustring Sequencer::GetName(){return name;}
int Sequencer::GetVolume(){return volume;}
void Sequencer::SetVolume(int v){volume = v;gui_window->volume_button.set_value((double)v);}

void Sequencer::SetPlayOncePhase(int p){
    play_once_phase = p;
    if(row_in_main_window) mainwindow->RefreshRow(row_in_main_window);
}

int Sequencer::GetPlayOncePhase(){
    return play_once_phase;
}

void Sequencer::ShowWindow(){
    gui_window->show();
    gui_window->raise();}

void Sequencer::UpdateGui(){gui_window->UpdateValues();}
void Sequencer::UpdateGuiChord(){gui_window->UpdateChord();}


int Sequencer::AddMelody(){
    vector<bool> line(6,false);
    vector<vector<bool> > seq;
    for(int x = 0; x < resolution;x++){
        seq.push_back(line);
    }
    melodies.push_back(seq);
    *dbg<< "Added melody " << melodies.size() << ".\n";
    return melodies.size()-1;
}

bool Sequencer::RemoveMelody(int x){
    melodies.erase(melodies.begin()+x);
    if (active_melody > x) active_melody--;
    else if (active_melody == x) active_melody = 0;

    *dbg<< "Removed melody " << x << ".\n";
    return 0;
}


int Sequencer::GetMelodyNote(int melody, int n, int c){
    return melodies[melody][n][c];
}

bool Sequencer::GetActiveMelodyNote(int n, int c){
    return melodies[active_melody][n][c];
}

void Sequencer::SetMelodyNote(int melody, int n, int c, bool value){
    melodies[melody][n][c] = value;

}

void Sequencer::ChangeActiveMelody(int new_one){
    active_melody = new_one%(melodies.size());
    gui_window->UpdateMelody();
}