/* 
 * File:   SequencerGUI.h
 * Author: cielak
 *
 * Created on 29 sierpień 2010, 22:19
 */

#ifndef SEQUENCERGUI_H
#define	SEQUENCERGUI_H
#include "global.h"
//#include "Sequencer.h"
class Sequencer;

class SequencerWindow : public Gtk::Window {
public:
    SequencerWindow(Sequencer* prt);
    virtual ~SequencerWindow();
    void UpdateValues();
private:
    void OnNotesChanged(int note);
    void OnSequenceChanged(int seq);
    Gtk::VBox box_of_sliders;
    Gtk::HBox box_of_notes;
    Gtk::HScale *sequence_scales[8];
    Gtk::SpinButton* note_buttons[6];
public:
    Sequencer *parent;

};

#endif	/* SEQUENCERGUI_H */

