/*
    Copyright (C) 2020 Rafał Cieślak

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

#ifndef NOTEDISPLAY_HPP
#define NOTEDISPLAY_HPP


#include <boost/signals2.hpp>
namespace bs2 = boost::signals2;

#include <gtkmm.h>


class NoteWidget : public Gtk::HBox {
public:
    NoteWidget();
    virtual ~NoteWidget();

    void SetValue(int value);
    void SetType(bool value_edit, bool note_name);
    void SetBaseNote(int base_note);

    int GetValue() const;
    /* Value + base note. */
    int GetCombinedValue() const;

    /* Reports just the value, that is, not accounting for the base note. */
    bs2::signal<void(int)> on_value_changed;

private:
    void UpdateNoteName();

    void OnSpinEditValueChanged();

    int base_note = 0;

    Gtk::SpinButton wSpinEdit;
    Gtk::Label wNoteName;
};

#endif // NOTEDISPLAY_HPP
