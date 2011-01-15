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


};



#endif	/* SETTINGSWINDOW_H */

