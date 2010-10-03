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

#ifndef CHORDWIDGET_H
#define	CHORDWIDGET_H
#include <gtkmm.h>

class ChordWidget : Gtk::VBox {
public:
    ChordWidget();
    virtual ~ChordWidget();

    Gtk::VBox MainBox;

    Gtk::HBox line_guitar;
    Gtk::HBox line_chord;
    Gtk::HBox line_custom;
    


    Gtk::RadioButtonGroup group;
    Gtk::RadioButton guitar;
    Gtk::RadioButton chord;
    Gtk::RadioButton custom;
private:

};

#endif	/* CHORDWIDGET_H */

