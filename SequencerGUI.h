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
    Gtk::VBox box_of_sliders;
    Gtk::HBox box_of_notes;
    Gtk::HScale *sequence_scales[8];
    Gtk::SpinButton* note_buttons[6];
    Gtk::SpinButton channel_button;
    Gtk::Label channellabel;
    Gtk::HBox low_hbox;
    Gtk::VBox toggle_vbox;
    Gtk::CheckButton tgl_apply_mainnote, tgl_mute;
private:
    void OnNotesChanged(int note);
    void OnSequenceChanged(int seq);
    void OnChannelChanged();
    void OnToggleMuteToggled();
    void OnToggleApplyMainNoteToggled();
public:
    Sequencer *parent;

};

#endif	/* SEQUENCERGUI_H */

