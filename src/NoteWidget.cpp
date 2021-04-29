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

#include "NoteWidget.hpp"

#include <algorithm>
#include <functional>
#include <string>

#include "Chord.hpp"


/* TODO: This function should be moved to a shared module. */
std::string MIDINoteName(int x){
    int oct = x / 12 - 1;
    int note = x % 12;
    char t[10];
    snprintf(t, 10, "%s%d", Chord::notemap[note].c_str(), oct);
    return t;
}


NoteWidget::NoteWidget()
{
    set_name("note-display");

    pack_start(wSpinEdit, Gtk::PACK_SHRINK);
    pack_end(wNoteName, Gtk::PACK_SHRINK);

    wSpinEdit.set_increments(1, 12);
    wSpinEdit.set_range(-128, 128);
    wSpinEdit.set_width_chars(3);
    wSpinEdit.signal_value_changed().connect(std::bind(&NoteWidget::OnSpinEditValueChanged, this));

    wNoteName.set_halign(Gtk::ALIGN_END);
    wNoteName.set_xalign(0.9);
    wNoteName.set_justify(Gtk::JUSTIFY_RIGHT);
    wNoteName.set_size_request(28, -1);

    UpdateNoteName();
}

NoteWidget::~NoteWidget()
{
}

void NoteWidget::SetValue(int value){
    wSpinEdit.set_value(value);
}

void NoteWidget::SetType(bool value_edit, bool note_name){
    if(value_edit) wSpinEdit.show();
    else wSpinEdit.hide();
    if(note_name) wNoteName.show();
    else wNoteName.hide();
}

void NoteWidget::SetBaseNote(int n){
    base_note = n;
    UpdateNoteName();
}

int NoteWidget::GetValue() const{
    return wSpinEdit.get_value();
}

int NoteWidget::GetCombinedValue() const{
    return std::max(0, std::min(128, base_note + GetValue()));
}

void NoteWidget::UpdateNoteName(){
    wNoteName.set_markup("<b>" + MIDINoteName(GetCombinedValue()) + "</b>");
}

void NoteWidget::OnSpinEditValueChanged(){
    UpdateNoteName();
    on_value_changed(GetValue());
}
