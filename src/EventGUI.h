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

#ifndef EVENTGUI_H
#define	EVENTGUI_H
#include "global.h"
#include <gtkmm.h>

class Event;


class EventGUI :public Gtk::Window{
public:
    EventGUI(Event *prt);
    virtual ~EventGUI();

    /**Sets all widgets data to the data from parent event.*/
    void UpdateValues();

    /**Event this GUI is associated with, used for two-way communication between Event and EventGUI.*/
    Event *parent;

private:
    /**Flag disabling signal that is called when type is changed, is true during type manual set.*/
    bool DO_NOT_INIT_TYPE;

    /**Hides and shows lines appropieate to the parent event type.*/
    void ChangeVisibleLines();
    /**Sets all default data for given type, used to avoid having events with strange arguments.*/
    void InitType();

    void OnTypeChanged();
    void OnKeyChanged();
    void OnNoteChanged();
    void OnCtrlChanged();
    void OnOSCPortChanged();
    void OnChannelChanged();
    bool OnKeyPress(GdkEventKey* event);
    void OnOKClicked();
    void OnCaptureClicked();

    Gtk::VBox main_box;
    Gtk::HBox line_type;
    Gtk::Label label_type;
    Gtk::ToggleButton capture;

    Gtk::HBox line_key;
    Gtk::Label label_key;
    Gtk::HBox line_note;
    Gtk::Label label_note;
    Gtk::HBox line_channel;
    Gtk::Label label_channel;
    Gtk::HBox line_controller;
    Gtk::Label label_controller;
    Gtk::Button ok_button;
    Gtk::HBox line_osc_tag;
    Gtk::Label label_osc_tag;
    Gtk::SpinButton osc_tag;

    Gtk::Label osc_note;
    Gtk::Label label_preview;
    Gtk::HSeparator separator;
    Gtk::ComboBox Types_combo;
    Gtk::ComboBox Keys_combo;
    Gtk::SpinButton note_spinbutton;
    Gtk::SpinButton ctrl_spinbutton;
    Gtk::ComboBox Channels_combo;

};

#endif	/* EVENTGUI_H */

