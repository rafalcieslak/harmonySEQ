/* 
 * File:   SequencerGUI.cpp
 * Author: cielak
 * 
 * Created on 29 sierpień 2010, 22:19
 */

#include "SequencerGUI.h"
#include "debug.h"
#include "Sequencer.h"
extern debug *dbg;

SequencerWindow::SequencerWindow(Sequencer* prt){
    *dbg << "constructing new SequencerWindow\n";

    parent = prt;

    set_title("Sequencer");
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);
    
    box_of_sliders.pack_start(box_of_notes);
    for (int x = 0; x < 6; x++){
        note_buttons[x] = new Gtk::SpinButton;
        note_buttons[x]->set_range(-24,24);
        note_buttons[x]->set_increments(1,12);
        //*dbg << parent->GetNotes(0);
        note_buttons[x]->set_value(parent->GetNotes(x));
        note_buttons[x]->signal_value_changed().connect(sigc::bind<int>(sigc::mem_fun(*this,&SequencerWindow::OnNotesChanged),x));
        box_of_notes.pack_start(*note_buttons[x]);
    }


//TODO: this is fixed to 8, let it be changeable
    for (int x= 0; x < 8; x++){
        sequence_scales[x] = new Gtk::HScale(0,6,1);
        sequence_scales[x]->set_value(parent->GetSequence(x));
        sequence_scales[x]->set_increments(1.0,1.0);
        //sequence_scales[x]->set_draw_value(0);  //<- hehe, here is a bug in gtkmm ^^
        sequence_scales[x]->set_value_pos(Gtk::POS_RIGHT); //<- temporary workaround
        sequence_scales[x]->signal_value_changed().connect(sigc::bind(sigc::mem_fun(*this,&SequencerWindow::OnSequenceChanged),x));
        box_of_sliders.pack_start(*sequence_scales[x]);
    }
    low_hbox.pack_start(channellabel,Gtk::PACK_SHRINK);
    low_hbox.pack_start(channel_button, Gtk::PACK_SHRINK);
    channellabel.set_text("MIDI channel:");
    channel_button.set_value(1);
    channel_button.set_range(1,16);
    channel_button.set_increments(1,1);
    channel_button.signal_value_changed().connect(sigc::mem_fun(*this,&SequencerWindow::OnChannelChanged));
    box_of_sliders.pack_start(low_hbox);
    add(box_of_sliders);
    
    show_all_children(1);
    show();
    //hide(); //hide at start, but let the children be shown
}
SequencerWindow::~SequencerWindow(){

    
}

void SequencerWindow::OnNotesChanged(int note){
    parent->notes[note] = note_buttons[note]->get_value();

}

void SequencerWindow::OnSequenceChanged(int seq){
    parent->sequence[seq] = sequence_scales[seq]->get_value();

}

void SequencerWindow::UpdateValues(){
    for (int x = 0; x < SEQUENCE_CONST_SIZE; x++){
        sequence_scales[x]->set_value(parent->sequence[x]);
    }
    for (int x = 0; x < NOTES_CONST_SIZE; x++) {
        note_buttons[x]->set_value(parent->GetNotes(x));
    }
}

void SequencerWindow::OnChannelChanged(){

    parent->channel = channel_button.get_value();
}