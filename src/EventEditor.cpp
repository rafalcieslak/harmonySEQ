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

#include "EventEditor.hpp"

#include "Configuration.hpp"
#include "Event.hpp"
#include "Files.hpp"
#include "TreeModels.hpp"
#include "shared.hpp"


EventEditor::EventEditor(){
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
    note_spinbutton.signal_value_changed().connect(std::bind(&EventEditor::OnNoteChanged, this));
    ctrl_spinbutton.signal_value_changed().connect(std::bind(&EventEditor::OnCtrlChanged, this));
    osc_tag.signal_value_changed().connect(std::bind(&EventEditor::OnOSCPortChanged, this));

    label_type.set_text(_("Type:"));
    label_channel.set_text(_("Channel:"));
    label_controller.set_text(_("Controller:"));
    label_key.set_text(_("Key:"));
    label_note.set_text(_("Note:"));
    label_osc_tag.set_text(_("OSC message tag:"));
    capture.set_label(_("Capture"));
    capture.set_tooltip_markup(_("Cathes next event end fill this one's type and arguments to fit the one triggered.\n<i>Example usage: press this button and then the X key on your keyboard. The event will be automatically set to type: keyboard, key: X.</i>"));

    capture.signal_clicked().connect(std::bind(&EventEditor::OnCaptureClicked, this));

    Types_combo.pack_start(m_columns_event_types.label);
    Types_combo.signal_changed().connect(std::bind(&EventEditor::OnTypeChanged, this));
    Keys_combo.pack_start(m_columns_key_codes.label);
    Keys_combo.signal_changed().connect(std::bind(&EventEditor::OnKeyChanged, this));
    Channels_combo.pack_start(m_columns_channels.label);
    Channels_combo.signal_changed().connect(std::bind(&EventEditor::OnChannelChanged, this));

    main_box.pack_start(ok_button,Gtk::PACK_SHRINK);
    ok_button.set_label(_("OK"));
    ok_button.signal_clicked().connect(std::bind(&EventEditor::OnOKClicked, this));

    signal_show().connect(std::bind(&EventEditor::UpdateEverything, this));
    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect([=](GdkEventKey *e){return OnKeyPress(e);});
    show_all_children(1);

    hide();
}


EventEditor::~EventEditor(){
}

void EventEditor::Edit(const Event& initial_value){
    initial_value.CopyInto(event);

    UpdateEverything();
    show();
    raise();
}

void EventEditor::OnOKClicked(){
    capture_connection.disconnect();

    hide();
    on_edit_completed(event);
}

void EventEditor::UpdateEverything(){
    UpdateVisibleLines();

    /* Prevent type initialization while we're initializing GUI.*/
    inhibit_type_initialization = true;
    for (Gtk::TreeModel::iterator iter = TreeModel_EventTypes->get_iter("0"); iter; iter++){
        if ((*iter)[m_columns_event_types.type] == event.type){
            Types_combo.set_active(iter);
            break;
        }
    }
    inhibit_type_initialization = false;

    switch (event.type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            for (Gtk::TreeModel::iterator iter = TreeModel_KeyCodes->get_iter("0"); iter; iter++) {
                if((*iter)[m_columns_key_codes.keycode]==event.arg1){
                    Keys_combo.set_active(iter);
                    break;
                }
            }

            break;
        case Event::NOTE:
            note_spinbutton.set_value(event.arg1);
            Channels_combo.set_active(event.arg2);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(event.arg1);
            Channels_combo.set_active(event.arg2);
            break;
        case Event::OSC:
            osc_tag.set_value(event.arg1);
            break;
        default:
            break;

    }

    label_preview.set_text(event.GetLabel());
}

void EventEditor::UpdateVisibleLines(){
    int type = event.type;
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

void EventEditor::InitType(){
    if(inhibit_type_initialization) return;

    /* Only prepare event values - when we're done, we call UpdateEverything which copies values from event into UI. */
    /* TODO: This should be handled by Event class */
    switch (event.type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            event.arg1 = 0;
            break;
        case Event::NOTE:
            event.arg1 = 0;
            event.arg2 = 0;
            break;
        case Event::CONTROLLER:
            event.arg1 = 0;
            event.arg2 = 0;
            break;
        case Event::OSC:
            event.arg1 = 0;
            break;
        default:
            break;

    }

    UpdateEverything();
}

void EventEditor::OnTypeChanged(){
    if(!Types_combo.get_active()) return;
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    event.type = type;
    UpdateVisibleLines();
    InitType();
    label_preview.set_text(event.GetLabel());
}



void EventEditor::OnChannelChanged(){
    if(event.type != Event::CONTROLLER &&
       event.type != Event::NOTE)
        return;

    event.arg2 = (*(Channels_combo.get_active()))[m_columns_channels.ch];

    label_preview.set_text(event.GetLabel());
}

void EventEditor::OnKeyChanged(){
    if(event.type != Event::KEYBOARD)
        return;

    event.arg1 = (*(Keys_combo.get_active()))[m_columns_key_codes.keycode];

    label_preview.set_text(event.GetLabel());
}

void EventEditor::OnCtrlChanged(){
    if(event.type != Event::CONTROLLER)
        return;
    event.arg1 = ctrl_spinbutton.get_value();

    label_preview.set_text(event.GetLabel());

}

void EventEditor::OnNoteChanged(){
    if(event.type != Event::NOTE)
        return;
    event.arg1 = note_spinbutton.get_value();

    label_preview.set_text(event.GetLabel());

}

void EventEditor::OnOSCPortChanged(){
    if(event.type != Event::OSC)
        return;
    event.arg1 = osc_tag.get_value();

    label_preview.set_text(event.GetLabel());
}

bool EventEditor::OnKeyPress(GdkEventKey* event){
    FindAndProcessEvents(Event::KEYBOARD, event->keyval);

    return 1;
}

void EventEditor::OnCaptureClicked(){
    if (capture.get_active() == 1){
        capture_connection = on_event_received.connect(
        [=](Event::EventTypes t, int a1, int a2){ DeferWorkToUIThread(
            [=](){
                capture_connection.disconnect();
                capture.set_active(0);
                event.type = t;
                event.arg1 = a1;
                event.arg2 = a2;
                UpdateEverything();
            });});
        label_preview.set_text(_("(Waiting for an event...)"));
    }else{
        capture_connection.disconnect();
        /* Restore preview text. */
        label_preview.set_text(event.GetLabel());
    }

}
