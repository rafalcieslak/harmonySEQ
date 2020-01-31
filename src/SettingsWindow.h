/*
    Copyright (C) 2010-2012 Rafał Cieślak

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

#ifndef SETTINGSWINDOW_H
#define	SETTINGSWINDOW_H
#include "global.h"
#include <gtkmm.h>

class SettingsWindow : public Gtk::Window {
public:
    SettingsWindow();
    virtual ~SettingsWindow();

    /**Fills in the GUI with values */
    void LoadDataFromConfig();

    /**Saves the values to config variables*/
    void StoreDataToConfig();

private:
    void OnShowed();
    void OnCancelClicked();
    void OnOKClicked();
    void OnRestoreDefaults();
    /**Needs to update some widget's sensitivity*/
    void OnUse14BarToggled();
    Gtk::VBox main_vbox;
    Gtk::HBox lower_hbox;

    Gtk::Button ok_button;
    Gtk::Button cancel_button;
    Gtk::Button restore_defaults_button;

    Gtk::Notebook notebook;

    Gtk::VBox page_main;
    Gtk::VBox page_midi;
    Gtk::VBox page_OSC;

        Gtk::VBox disable_diodes_vbox;
        Gtk::CheckButton disable_diodes;
        Gtk::HBox disable_diodes_label_hbox;
        Gtk::Label disable_diodes_label;

        Gtk::HBox columns_label_hbox;
        Gtk::Label colums_label;
        Gtk::VBox colums_vbox;
        Gtk::CheckButton colums_id;
        Gtk::CheckButton colums_name;
        Gtk::CheckButton colums_onoff;
        Gtk::CheckButton colums_channel;
        Gtk::CheckButton colums_pattern;
        Gtk::CheckButton colums_resolution;
        Gtk::CheckButton colums_length;
        Gtk::CheckButton colums_chord;

        Gtk::HSeparator sep1;

        Gtk::HBox metronome_label_hbox;
        Gtk::Table metronome_table;
        Gtk::Label metronome_label;
        Gtk::Label metronome_channel_label;
        Gtk::HBox metronome_channel_hbox;
        Gtk::SpinButton metronome_channel;
        Gtk::Label metronome_note_label;
        Gtk::Label metronome_velocity_label;
        Gtk::Label metronome_bar_label;
        Gtk::CheckButton metronome_1_4;
        Gtk::SpinButton metronome_bar_note;
        Gtk::SpinButton metronome_1_4_note;
        Gtk::SpinButton metronome_bar_velocity;
        Gtk::SpinButton metronome_1_4_velocity;

        Gtk::HSeparator sep2;
        
        Gtk::HBox osc_port_label_hbox;
        Gtk::Label osc_port_label;
        Gtk::SpinButton osc_port;
        Gtk::Label osc_note_restart;
        Gtk::HSeparator osc_sep;
        Gtk::Label osc_description;
        Gtk::HBox osc_description_hbox;
        
        Gtk::HSeparator sepFPS;
        Gtk::VBox boxFPS;
        Gtk::HBox boxFPSlabel;
        Gtk::HBox boxFPSbutton_label;
        Gtk::SpinButton FPSbutton;
        Gtk::Label FPSbutton_label;
        Gtk::Label FPSlabel;

};



#endif	/* SETTINGSWINDOW_H */

