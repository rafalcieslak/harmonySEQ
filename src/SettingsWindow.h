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


    Gtk::VBox main_vbox;
    Gtk::HBox lower_hbox;

    Gtk::Button ok_button;
    Gtk::Button cancel_button;

};



#endif	/* SETTINGSWINDOW_H */

