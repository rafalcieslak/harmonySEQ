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


class Sequencer {
public:
    Sequencer();
    Sequencer(const Sequencer& orig);
    Sequencer(int seq[],int notes[]);
    void Init();
    virtual ~Sequencer();
    vector<int> sequence;
    int GetSequence(int n);
    vector<int> notes;
    int GetNotes(int n);

    int channel;
    bool muted;
    int row_in_main_window;

    SequencerWindow *gui_window;
    
private:

};

#endif	/* SEQUENCER_H */

