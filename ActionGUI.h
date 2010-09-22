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

#ifndef ACTIONGUI_H
#define	ACTIONGUI_H
#include "global.h"
class Action;

class ActionGUI : public Gtk::Window{
public:
    ActionGUI(Action *prt);
    ActionGUI(const ActionGUI& orig);
    virtual ~ActionGUI();
    void OnShow();
    void UpdateValues();
    void TypeChanged();
    void OnOKClicked();
    void OnTypeChanged();


    Gtk::VBox main_box;
    Gtk::HBox line_type;
    Gtk::Label label_type;
    Gtk::HBox line_note;
    Gtk::Label label_note;
    Gtk::Button ok_button;

    Gtk::Label label_preview;
    Gtk::HSeparator separator;
    
    Action *parent;

private:

};

#endif	/* ACTIONGUI_H */

