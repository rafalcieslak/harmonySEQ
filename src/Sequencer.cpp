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

void clear_sequencers(){
    for(unsigned int x = 0; x < sequencers.size(); x++) delete sequencers[x];

    sequencers.clear();

}
//======begin sequencer class===============

Sequencer::Sequencer()
    : sequence(SEQUENCE_DEFAULT_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    name = SEQUENCER_DEFAULT_NAME;
    Init();
}

Sequencer::Sequencer(Glib::ustring _name)
    : sequence(SEQUENCE_DEFAULT_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    name = _name;
    Init();
}


Sequencer::Sequencer(int seq[],int note[])
    :  sequence(SEQUENCE_DEFAULT_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    for (int x = 0; x < SEQUENCE_DEFAULT_SIZE; x++){
        sequence[x] = seq[x];
        
    }
    for (int x = 0; x < NOTES_CONST_SIZE; x++){
        notes[x] = note[x];
        
    }
    name = SEQUENCER_DEFAULT_NAME;
    Init();
}

Sequencer::Sequencer(int seq[],int note[], Glib::ustring _name)
    :  sequence(SEQUENCE_DEFAULT_SIZE,0), notes(NOTES_CONST_SIZE,0)
{
    for (int x = 0; x < SEQUENCE_DEFAULT_SIZE; x++){
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
    resolution = orig->resolution;
    sequence = orig->sequence;
    channel = orig->channel;
    apply_mainnote = orig->apply_mainnote;
    length = orig->length;
    volume = orig->volume;
    gui_window = new SequencerWindow(this);
    last_played_note = orig->last_played_note;
    played_once = 0;
}

Sequencer::~Sequencer() {
}

void Sequencer::Init(){

    on = false;
    apply_mainnote = true;
    channel = 1;
    length = 1;
    volume = DEFAULT_VOLUME;
    last_played_note = 0;
    played_once = 0;
    resolution = SEQUENCE_DEFAULT_SIZE;
    *dbg << notes[0]<<ENDL;
    *dbg << GetNotes(0);
    gui_window = new SequencerWindow(this);
    //gui_window->UpdateValues();
    //gui_window->parent = this;

}

void Sequencer::SetResolution(int res){
    if (res == resolution) return;
    
    if (res < resolution){
        //the new resolution is smaller
        vector<int> new_sequence(res,0);
        int ratio = resolution/res;
        *dbg << "ensmalling resolution ratio = " << ratio << ENDL;
        assert(ratio>=1);
        int x = 0, i = 0;
        for (; x < resolution;x+=ratio){
            new_sequence[i++] = sequence[x];
        }

        sequence = new_sequence;
        resolution = res;
        
    } else {
        //the new resolution is larger
        vector<int> new_sequence(res,0);
        int ratio = res/resolution;
        *dbg << "enlarging resolution ratio = " << ratio << ENDL;
        int x = 0;
        for (int p =0; p < resolution;p++){
            for (int i = 0; i < ratio;i++){
                new_sequence[x++]=sequence[p];
            }
        }
        sequence=new_sequence;
        resolution = res;
    }


}




int Sequencer::GetNotes(int n){return notes[n];}
int Sequencer::GetSequence(int n){return sequence[n];}
void Sequencer::SetOn(bool m){on = m; gui_window->tgl_mute.set_active(m);}
bool Sequencer::GetOn(){return on;}
void Sequencer::SetApplyMainNote(bool a){apply_mainnote = a;gui_window->tgl_apply_mainnote.set_active(a);}
bool Sequencer::GetApplyMainNote(){return apply_mainnote;}
void Sequencer::SetChannel(int ch){channel = ch;gui_window->channel_button.set_value((double)ch);}
int Sequencer::GetChannel(){return channel;}
void Sequencer::SetName(Glib::ustring nm){name = nm;gui_window->set_title(nm);}
Glib::ustring Sequencer::GetName(){return name;}
int Sequencer::GetVolume(){return volume;}
void Sequencer::SetVolume(int v){volume = v;gui_window->volume_button.set_value((double)v);}

void Sequencer::ShowWindow(){gui_window->show();}
void Sequencer::UpdateGui(){gui_window->UpdateValues();}
void Sequencer::UpdateGuiNotes(){gui_window->UpdateNotes();}
void Sequencer::PlayOnce(){on = true; played_once = true;}
bool Sequencer::GetPlayedOnce(){return played_once;}
void Sequencer::GotPlayedOnce(){on = false; played_once = false;}