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
#include "EventGUI.h"
#include "Event.h"
#include "EventsWindow.h"
#include "messages.h"
#include "TreeModels.h"
#include "Files.h"
EventGUI::EventGUI(Event *prt){
    parent = prt;

    //setting window attributes
    set_title(_("Event"));
    set_border_width(5);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    set_resizable(0);
    set_size_request(300,-1);

    add(main_box);
    main_box.set_spacing(5);
    //preparing
    Types_combo.set_model(TreeModel_EventTypes);
    Keys_combo.set_model(TreeModel_KeyCodes);
    Keys_combo.set_wrap_width(3); //three columns
    Channels_combo.set_model(TreeModel_Channels);

    main_box.pack_start(line_type);
    main_box.pack_start(line_key);
    main_box.pack_start(line_note);
    main_box.pack_start(line_controller);
    main_box.pack_start(line_channel);
    main_box.pack_start(separator);
    main_box.pack_start(label_preview);
    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_key.pack_start(label_key,Gtk::PACK_SHRINK);
    line_note.pack_start(label_note,Gtk::PACK_SHRINK);
    line_controller.pack_start(label_controller,Gtk::PACK_SHRINK);
    line_channel.pack_start(label_channel,Gtk::PACK_SHRINK);

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_type.pack_end(capture,Gtk::PACK_SHRINK);
    line_key.pack_start(Keys_combo,Gtk::PACK_SHRINK);
    line_note.pack_start(note_spinbutton,Gtk::PACK_SHRINK);
    line_controller.pack_start(ctrl_spinbutton,Gtk::PACK_SHRINK);
    line_channel.pack_start(Channels_combo,Gtk::PACK_SHRINK);

    note_spinbutton.set_range(0.0,127.0);
    ctrl_spinbutton.set_range(0.0,127.0);
    note_spinbutton.set_increments(1.0,16.0);
    ctrl_spinbutton.set_increments(1.0,16.0);
    note_spinbutton.signal_value_changed().connect(mem_fun(*this,&EventGUI::OnNoteChanged));
    ctrl_spinbutton.signal_value_changed().connect(mem_fun(*this,&EventGUI::OnCtrlChanged));

    label_type.set_text(_("Type:"));
    label_channel.set_text(_("Channel:"));
    label_controller.set_text(_("Controller:"));
    label_key.set_text(_("Key:"));
    label_note.set_text(_("Note:"));
    capture.set_label(_("Capture"));
    capture.set_tooltip_markup(_("Cathes next event end fill this one's type and arguments to fit the one triggered.\n<i>Example usage: press this button and then the X key on your keyboard. The event will be automatically set to type: keyboard, key: X.</i>"));

    capture.signal_clicked().connect(sigc::mem_fun(*this,&EventGUI::OnCaptureClicked));

    Types_combo.pack_start(m_columns_event_types.label);
    Types_combo.set_active(parent->type);
    Types_combo.signal_changed().connect(mem_fun(*this,&EventGUI::OnTypeChanged));
    Keys_combo.pack_start(m_columns_key_codes.label);
    Keys_combo.signal_changed().connect(mem_fun(*this,&EventGUI::OnKeyChanged));
    Channels_combo.pack_start(m_columns_channels.label);
    Channels_combo.signal_changed().connect(mem_fun(*this,&EventGUI::OnChannelChanged));

    main_box.pack_start(ok_button,Gtk::PACK_SHRINK);
    ok_button.set_label(_("OK"));
    ok_button.signal_clicked().connect(mem_fun(*this,&EventGUI::OnOKClicked));

    signal_show().connect(mem_fun(*this,&EventGUI::UpdateValues));
    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect(mem_fun(*this,&EventGUI::OnKeyPress));

    label_preview.set_text(parent->GetLabel());
    show_all_children(1);

    DO_NOT_INIT_TYPE = false;
    ChangeVisibleLines(); // to hide some of widgets according to the type
    hide();
}


EventGUI::~EventGUI(){
}

void EventGUI::ChangeVisibleLines(){
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    line_key.hide();
    line_note.hide();
    line_controller.hide();
    line_channel.hide();

    switch (type){
        case Event::NONE:

            break;
        case Event::CONTROLLER:
            line_controller.show();
            line_channel.show();
            break;
        case Event::NOTE:
            line_note.show();
            line_channel.show();
            break;
        case Event::KEYBOARD:
            line_key.show();
            break;
        default:
            break;
    }
    resize(2,2);
}

void EventGUI::InitType(){
    switch (parent->type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            Keys_combo.set_active(0);
            break;
        case Event::NOTE:
            note_spinbutton.set_value(0.0);
            parent->arg1=0;
            Channels_combo.set_active(0);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(0.0);
            parent->arg1=0;
            Channels_combo.set_active(0);
            break;
        default:
            break;

    }

}

void EventGUI::OnTypeChanged(){
    if(!Types_combo.get_active()) return;
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    parent->type = type;
    ChangeVisibleLines();
    if (!DO_NOT_INIT_TYPE) InitType();
    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
    Files::SetFileModified(1);
}



void EventGUI::OnChannelChanged(){
    if(parent->type == Event::CONTROLLER || parent->type == Event::NOTE){

            parent->arg2 = (*(Channels_combo.get_active()))[m_columns_channels.ch];
        
    }else *err << _("Error: channel has changed, while event is not MIDI-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
    Files::SetFileModified(1);
}

void EventGUI::OnKeyChanged(){
    if(parent->type == Event::KEYBOARD){
            parent->arg1 = (*(Keys_combo.get_active()))[m_columns_key_codes.keycode];
    }else *err << _("Error: key has changed, while event is not key-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
    Files::SetFileModified(1);
}

void EventGUI::OnCtrlChanged(){

    if(parent->type == Event::CONTROLLER){
        parent->arg1 = ctrl_spinbutton.get_value();
    }else *err << _("Error: controller has changed, while event is not ctrl-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
    Files::SetFileModified(1);

}

void EventGUI::OnNoteChanged(){
    if(parent->type == Event::NOTE){
        parent->arg1 = note_spinbutton.get_value();
    }else *err << _("Error: note has changed, while event is not note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    if(parent->row_in_event_window) eventswindow->UpdateRow(parent->row_in_event_window);
    Files::SetFileModified(1);

}
void EventGUI::OnOKClicked(){
    event_capturing_mode = 0;
    event_to_capture_to = NULL; //not nessesary, but just for cleaning up
    hide();
}

void EventGUI::UpdateValues(){
    set_transient_for(*eventswindow);
    Gtk::TreeModel::iterator it = TreeModel_EventTypes->get_iter("0");
    Gtk::TreeModel::Row row;
    DO_NOT_INIT_TYPE = true; //causes the Types_combo.signal_changed reciver know he shouldnt clear event args with zeros;

    for (;it;it++){
        row = *it;
        if (row[m_columns_event_types.type] == parent->type){
            Types_combo.set_active(it);
            break;
        }
    }
    DO_NOT_INIT_TYPE = false;
    switch (parent->type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            it = TreeModel_KeyCodes->get_iter("0");

            for (; it; it++) {
                row = *it;
                if(row[m_columns_key_codes.keycode]==parent->arg1){
                    Keys_combo.set_active(it);
                    break;
                }
            }

            break;
        case Event::NOTE:
            note_spinbutton.set_value(parent->arg1);
            Channels_combo.set_active(parent->arg2);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(parent->arg1);
            Channels_combo.set_active(parent->arg2);
            break;
        default:
            break;

    }

    label_preview.set_text(parent->GetLabel());

    if(event_capturing_mode == 1 && event_to_capture_to == parent) capture.set_active(1);
    else capture.set_active(0);

}

bool EventGUI::OnKeyPress(GdkEventKey* event){
    FindAndProcessEvents(Event::KEYBOARD, event->keyval);

    return 1;
}

void EventGUI::OnCaptureClicked(){
    if (capture.get_active() == 1){
        event_capturing_mode = 1;
        event_to_capture_to = parent;
        label_preview.set_text(_("(Waiting for an event...)"));
    }else{
        event_capturing_mode = 0;
        event_to_capture_to = NULL; //not nessesary, but just for cleaning up
        UpdateValues();
    }

}