/*
    Copyright (C) 2010, 2020 Rafał Cieślak

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

#include "EventGUI.hpp"

#include "Configuration.hpp"
#include "Event.hpp"
#include "Files.hpp"
#include "TreeModels.hpp"
#include "shared.hpp"


EventGUI::EventGUI(){
    set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    set_modal(true);

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
    main_box.pack_start(line_osc_tag);
    main_box.pack_start(separator);
    main_box.pack_start(osc_note);
    main_box.pack_start(label_preview);

    char temp[200];
#ifndef DISABLE_OSC
    sprintf(temp,_("<span size='small'>harmonySEQ listens for OSC messages at port %d</span>"),Config::OSC::Port);
#else
    sprintf(temp,_("<span size='small'><b>harmonySEQ was compiled without OSC support!</b>\nThis means this event cannot be triggered.</span>"));
#endif

    osc_note.set_markup(temp);

    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_key.pack_start(label_key,Gtk::PACK_SHRINK);
    line_note.pack_start(label_note,Gtk::PACK_SHRINK);
    line_controller.pack_start(label_controller,Gtk::PACK_SHRINK);
    line_channel.pack_start(label_channel,Gtk::PACK_SHRINK);
    line_osc_tag.pack_start(label_osc_tag,Gtk::PACK_SHRINK);

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_type.pack_end(capture,Gtk::PACK_SHRINK);
    line_key.pack_start(Keys_combo,Gtk::PACK_SHRINK);
    line_note.pack_start(note_spinbutton,Gtk::PACK_SHRINK);
    line_controller.pack_start(ctrl_spinbutton,Gtk::PACK_SHRINK);
    line_channel.pack_start(Channels_combo,Gtk::PACK_SHRINK);
    line_osc_tag.pack_start(osc_tag,Gtk::PACK_SHRINK);

    note_spinbutton.set_range(0.0,127.0);
    ctrl_spinbutton.set_range(0.0,127.0);
    osc_tag.set_range(0.0,127.0);
    note_spinbutton.set_increments(1.0,16.0);
    ctrl_spinbutton.set_increments(1.0,16.0);
    osc_tag.set_increments(1.0,16.0);
    note_spinbutton.signal_value_changed().connect(std::bind(&EventGUI::OnNoteChanged, this));
    ctrl_spinbutton.signal_value_changed().connect(std::bind(&EventGUI::OnCtrlChanged, this));
    osc_tag.signal_value_changed().connect(std::bind(&EventGUI::OnOSCPortChanged, this));

    label_type.set_text(_("Type:"));
    label_channel.set_text(_("Channel:"));
    label_controller.set_text(_("Controller:"));
    label_key.set_text(_("Key:"));
    label_note.set_text(_("Note:"));
    label_osc_tag.set_text(_("OSC message tag:"));
    capture.set_label(_("Capture"));
    capture.set_tooltip_markup(_("Cathes next event end fill this one's type and arguments to fit the one triggered.\n<i>Example usage: press this button and then the X key on your keyboard. The event will be automatically set to type: keyboard, key: X.</i>"));

    capture.signal_clicked().connect(std::bind(&EventGUI::OnCaptureClicked, this));

    Types_combo.pack_start(m_columns_event_types.label);
    Types_combo.signal_changed().connect(std::bind(&EventGUI::OnTypeChanged, this));
    Keys_combo.pack_start(m_columns_key_codes.label);
    Keys_combo.signal_changed().connect(std::bind(&EventGUI::OnKeyChanged, this));
    Channels_combo.pack_start(m_columns_channels.label);
    Channels_combo.signal_changed().connect(std::bind(&EventGUI::OnChannelChanged, this));

    main_box.pack_start(ok_button,Gtk::PACK_SHRINK);
    ok_button.set_label(_("OK"));
    ok_button.signal_clicked().connect(std::bind(&EventGUI::OnOKClicked, this));

    signal_show().connect(std::bind(&EventGUI::UpdateEverything, this));
    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect([=](GdkEventKey *e){return OnKeyPress(e);});
    show_all_children(1);

    DO_NOT_INIT_TYPE = false;
    hide();
}


EventGUI::~EventGUI(){
}

void EventGUI::SwitchTarget(Event* t){
    target = t;

    UpdateEverything();
}

void EventGUI::UpdateVisibleLines(){
    int type = target->type;
    line_key.hide();
    line_note.hide();
    line_controller.hide();
    line_channel.hide();
    line_osc_tag.hide();
    osc_note.hide();

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
        case Event::OSC:
            line_osc_tag.show();
            osc_note.show();
            break;
        default:
            break;
    }
    resize(2,2);
}

void EventGUI::UpdateEverything(){
    Gtk::TreeModel::iterator it = TreeModel_EventTypes->get_iter("0");
    Gtk::TreeModel::Row row;

    UpdateVisibleLines();

    DO_NOT_INIT_TYPE = true; //causes the Types_combo.signal_changed reciver know he shouldnt clear event args with zeros;
    for (;it;it++){
        row = *it;
        if (row[m_columns_event_types.type] == target->type){
            Types_combo.set_active(it);
            break;
        }
    }
    DO_NOT_INIT_TYPE = false;
    switch (target->type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            it = TreeModel_KeyCodes->get_iter("0");

            for (; it; it++) {
                row = *it;
                if(row[m_columns_key_codes.keycode]==target->arg1){
                    Keys_combo.set_active(it);
                    break;
                }
            }

            break;
        case Event::NOTE:
            note_spinbutton.set_value(target->arg1);
            Channels_combo.set_active(target->arg2);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(target->arg1);
            Channels_combo.set_active(target->arg2);
            break;
        case Event::OSC:
            osc_tag.set_value(target->arg1);
            break;
        default:
            break;

    }

    label_preview.set_text(target->GetLabel());

    if(event_capturing_mode == true && event_to_capture_to == target)
        capture.set_active(1);
    else
        capture.set_active(0);
}

void EventGUI::InitType(){
    switch (target->type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            Keys_combo.set_active(0);
            break;
        case Event::NOTE:
            note_spinbutton.set_value(0.0);
            target->arg1=0;
            Channels_combo.set_active(0);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(0.0);
            target->arg1=0;
            Channels_combo.set_active(0);
            break;
        case Event::OSC:
            osc_tag.set_value(0.0);
            target->arg1=0;
            break;
        default:
            break;

    }

}

void EventGUI::OnTypeChanged(){
    if(!Types_combo.get_active()) return;
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    target->type = type;
    UpdateVisibleLines();
    if (!DO_NOT_INIT_TYPE) InitType();
    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);
}



void EventGUI::OnChannelChanged(){
    if(target->type != Event::CONTROLLER &&
       target->type != Event::NOTE)
        return;

    target->arg2 = (*(Channels_combo.get_active()))[m_columns_channels.ch];

    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);
}

void EventGUI::OnKeyChanged(){
    if(target->type != Event::KEYBOARD)
        return;

    target->arg1 = (*(Keys_combo.get_active()))[m_columns_key_codes.keycode];

    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);
}

void EventGUI::OnCtrlChanged(){
    if(target->type != Event::CONTROLLER)
        return;
    target->arg1 = ctrl_spinbutton.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);

}

void EventGUI::OnNoteChanged(){
    if(target->type != Event::NOTE)
        return;
    target->arg1 = note_spinbutton.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);

}

void EventGUI::OnOSCPortChanged(){
    if(target->type != Event::OSC)
        return;
    target->arg1 = osc_tag.get_value();

    label_preview.set_text(target->GetLabel());
    target->on_changed();
    Files::SetFileModified(1);
}

void EventGUI::OnOKClicked(){
    event_capturing_mode = 0;
    event_to_capture_to = NULL; //not nessesary, but just for cleaning up
    capture_connection.disconnect();
    hide();
}

bool EventGUI::OnKeyPress(GdkEventKey* event){
    FindAndProcessEvents(Event::KEYBOARD, event->keyval);

    return 1;
}

void EventGUI::OnCaptureClicked(){
    if (capture.get_active() == 1){
        event_capturing_mode = true;
        event_to_capture_to = target;
        capture_connection = target->on_changed.connect(std::bind(&EventGUI::UpdateEverything, this));
        label_preview.set_text(_("(Waiting for an event...)"));
    }else{
        event_capturing_mode = false;
        event_to_capture_to = NULL; //not nessesary, but just for cleaning up
        capture_connection.disconnect();
    }

}
