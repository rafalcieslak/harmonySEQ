/* 
 * File:   Sequencer.h
 * Author: cielak
 *
 * Created on 29 sierpień 2010, 20:49
 */

#ifndef SEQUENCER_H
#define	SEQUENCER_H
#include <vector>
#include "SequencerGUI.h"

using namespace std;

#define SEQUENCE_CONST_SIZE 8
#define NOTES_CONST_SIZE 8

//the following may not work with gettext. careful!
#define SEQUENCER_DEFAULT_NAME _("sequencer")
int spawn_sequencer();

class Sequencer {
public:
    Sequencer();
    Sequencer(Glib::ustring _name0);
    Sequencer(const Sequencer& orig);
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



    Gtk::TreeModel::iterator row_in_main_window;
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

