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
#include <vector>
#include "SequencerGUI.h"

using namespace std;

#define SEQUENCE_CONST_SIZE 8
#define NOTES_CONST_SIZE 6

//the following may not work with gettext. careful!
#define SEQUENCER_DEFAULT_NAME _("sequencer")






Gtk::TreeModel::iterator spawn_sequencer();
Gtk::TreeModel::iterator clone_sequencer(int orig);

class Sequencer {
public:
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer* orig);
    Sequencer(int seq[],int notes[]);
    Sequencer(int seq[],int notes[], Glib::ustring _name);
    void Init();
    virtual ~Sequencer();
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



    //Gtk::TreeModel::iterator row_in_main_window;
    Gtk::TreeRowReference row_in_main_window;
    friend class SequencerWindow;
protected:
    int channel;
    bool apply_mainnote;
    bool on;
    Glib::ustring name;
    SequencerWindow *gui_window;

    
private:

};

#endif	/* SEQUENCER_H */

