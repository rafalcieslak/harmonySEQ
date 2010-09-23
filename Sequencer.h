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


#ifndef SEQUENCER_H
#define	SEQUENCER_H
#include "global.h"
#include "SequencerGUI.h"
using namespace std;


Gtk::TreeModel::RowReference spawn_sequencer();
Gtk::TreeModel::RowReference clone_sequencer(int orig);
void clear_sequencers();

class Sequencer {
public:
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer* orig);
    Sequencer(int seq[],int notes[]);
    Sequencer(int seq[],int notes[], Glib::ustring _name);
    virtual ~Sequencer();
    
    void Init();


    void SetResolution(int res);
    double length;
    int resolution;
    vector<int> sequence;
    int GetSequence(int n);
    vector<int> notes;
    int GetNotes(int n);
    void SetOn(bool m);
    bool GetOn();
    void SetApplyMainNote(bool a);
    bool GetApplyMainNote();
    void SetChannel(int ch);
    int GetChannel();
    void SetName(Glib::ustring n);
    Glib::ustring GetName();
    void ShowWindow();
    void UpdateGui();
    int GetVolume();
    void SetVolume(int v);
    int last_played_note;

    Gtk::TreeRowReference row_in_main_window;
    friend class SequencerWindow;
protected:
    int channel;
    bool apply_mainnote;
    bool on;
    int volume;
    Glib::ustring name;
    SequencerWindow *gui_window;
    
private:

};

#endif	/* SEQUENCER_H */

