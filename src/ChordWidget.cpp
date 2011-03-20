/*
    Copyright (C) 2010, 2011 Rafał Cieślak

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

#include "ChordWidget.h"
#include "TreeModels.h"
#include "messages.h"


ChordWidget::ChordWidget(){

    AnythingSelected = 0;
    chord = NULL;

    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    pack_start(MainBox);

    MainBox.pack_start(LeftHBox);
    MainBox.pack_start(vsep);
    MainBox.pack_end(NotesVBox,Gtk::PACK_SHRINK);

    LeftHBox.pack_start(caption,Gtk::PACK_SHRINK);
    LeftHBox.pack_start(line1,Gtk::PACK_SHRINK);
    LeftHBox.pack_start(line2,Gtk::PACK_SHRINK);
    LeftHBox.pack_start(line3,Gtk::PACK_SHRINK);
    LeftHBox.pack_start(line4,Gtk::PACK_SHRINK);
    LeftHBox.pack_start(line5,Gtk::PACK_SHRINK);

    caption.set_markup(_("<b>Chord</b>"));
    inv_label.set_text(_("Inversion: "));
    type_label.set_text(_("Type: "));

    combo_type.set_model(TreeModel_ChordTypes);
    combo_root.set_model(TreeModel_Notes);
    combo_note.set_model(TreeModel_Notes);
    combo_guitar_mode.set_model(TreeModel_ChordGuitarModes);
    combo_triad_mode.set_model(TreeModel_TriadModes);
    combo_type.pack_start(m_columns_IdAndName.name);
    combo_root.pack_start(m_columns_notes.name);
    combo_note.pack_start(m_columns_notes.name);
    combo_guitar_mode.pack_start(m_columns_IdAndName.name);
    combo_triad_mode.pack_start(m_columns_IdAndName.name);
    combo_type.signal_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnTypeChanged));
    combo_root.signal_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnRootChanged));
    combo_note.signal_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnBaseNoteChanged));
    combo_guitar_mode.signal_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnGuitarModeChanged));
    combo_triad_mode.signal_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnTriadModeChanged));
    combo_type.set_tooltip_markup(_("Sets <b>type</b> of this chord."));
    combo_guitar_mode.set_tooltip_markup(_("Sets <b>mode</b> of this chord."));
    combo_triad_mode.set_tooltip_markup(_("Sets <b>mode</b> of this chord."));
    combo_root.set_tooltip_markup(_("Sets <b>root</b> of this chord."));

    octave.set_range(-5.0,5.0);
    octave.set_increments(1.0,2.0);
    octave.set_width_chars(2);
    octave.signal_value_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnBaseOctaveChanged));
    octave_label.set_text(_("Octave:"));

    inversion.set_range(0.0,2.0);
    inversion.set_increments(1.0,1.0);
    inversion.set_width_chars(1);
    inversion.signal_value_changed().connect(sigc::mem_fun(*this,&ChordWidget::OnInversionChanged));
    inversion.set_tooltip_markup(_("Sets the <b>inversion</b> of the triad."));
    inversion_label.set_text(_("Inversion:"));


    use_base.set_label(_("Base:"));
    use_base.signal_toggled().connect(sigc::mem_fun(*this,&ChordWidget::OnUseBaseToggled));
    use_base.set_tooltip_text(_("Sets whether this chord uses the base note.\n - If on, chord notes are set relatively to the base note.\n - If off, the chord ignores the base note."));
    note_label.set_text(_("\tNote:"));
    eq_label.set_text("=");

    base.set_range(0.0, 128.0);
    base.set_increments(1.0, 12.0);
    base.set_width_chars(3);
    base.signal_value_changed().connect(sigc::mem_fun(*this, &ChordWidget::OnBaseChanged));
    base.set_tooltip_markup(_("The <b>base note</b> for this chord. If it uses the base, which is determined by the switch on the left-hand side, all notes of this chord are given relatively to this base note."));

    line1.pack_start(type_label,Gtk::PACK_SHRINK);
    line1.pack_start(combo_type,Gtk::PACK_SHRINK);
    line2.pack_start(combo_root,Gtk::PACK_SHRINK);
    line2.pack_start(combo_guitar_mode,Gtk::PACK_SHRINK);
    line2.pack_start(combo_triad_mode,Gtk::PACK_SHRINK);
    line3.pack_start(inv_label,Gtk::PACK_SHRINK);
    line3.pack_start(inversion,Gtk::PACK_SHRINK);
    line4.pack_start(use_base,Gtk::PACK_SHRINK);
    line4.pack_start(octave_label,Gtk::PACK_SHRINK);
    line4.pack_start(octave,Gtk::PACK_SHRINK);
    line5.pack_start(note_label,Gtk::PACK_SHRINK);
    line5.pack_start(combo_note,Gtk::PACK_SHRINK);
    line5.pack_start(eq_label,Gtk::PACK_SHRINK);
    line5.pack_start(base,Gtk::PACK_SHRINK);

    for (int x = 0; x < 6; x++) {
        note_buttons[x] = new Gtk::SpinButton;
        note_buttons[x]->set_range(-128.0, 128.0);
        note_buttons[x]->set_increments(1.0, 12.0);
        note_buttons[x]->set_width_chars(3);
        note_buttons[x]->signal_value_changed().connect(sigc::bind<int>(sigc::mem_fun(*this, &ChordWidget::OnNoteChanged), x));
        NotesVBox.pack_start(*note_buttons[x],Gtk::PACK_SHRINK);
    }

    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals=false;
}

void ChordWidget::Select(Chord* ch){
    AnythingSelected = 1;
    chord = ch;
    Update();
}

void ChordWidget::UnSelect(){
    AnythingSelected = 0;
}

ChordWidget::~ChordWidget(){
    for (int x = 0; x < 6; x++)
        delete note_buttons[x];
}

void ChordWidget::OnNoteChanged(int n){
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;

    chord->SetNote(5-n,note_buttons[n]->get_value());
    combo_type.set_active(Chord::CHORD_TYPE_CUSTOM); //this will call the signal handler, which update widgets visibility&sensitivity

    on_note_changed.emit(n,(use_base.get_active())?(note_buttons[n]->get_value()+base.get_value()):note_buttons[n]->get_value());
    on_changed.emit();
}

void ChordWidget::UpdateWhatToShowAndWhatIsSensitive(){
    base.set_sensitive(1);
    use_base.set_sensitive(1);
    note_label.set_sensitive(1);
    combo_note.set_sensitive(1);
    combo_root.hide();
    combo_guitar_mode.hide();
    combo_triad_mode.hide();
    inversion_label.hide();
    inversion.hide();

    Gtk::TreeModel::Row row = *(combo_type.get_active());
    int type = row[m_columns_IdAndName.id];
    switch (type){
        case Chord::CHORD_TYPE_CUSTOM:
            
            break;
        case Chord::CHORD_TYPE_TRIAD:
            combo_root.show();
            combo_triad_mode.show();
            inversion_label.show();
            inversion.show();
            base.set_sensitive(0);
            note_label.set_sensitive(0);
            combo_note.set_sensitive(0);
            use_base.set_sensitive(0);
            break;
        case Chord::CHORD_TYPE_GUITAR:
            combo_root.show();
            combo_guitar_mode.show();
            base.set_sensitive(0);
            note_label.set_sensitive(0);
            combo_note.set_sensitive(0);
            use_base.set_sensitive(0);
            break;
            
        
    }
}

void ChordWidget::OnTypeChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    Gtk::TreeModel::Row row = *(combo_type.get_active());
    int type = row[m_columns_IdAndName.id];

    UpdateWhatToShowAndWhatIsSensitive();

    chord->SetType(type);

    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    use_base.set_active(chord->GetBaseUse());
    combo_note.set_active(chord->GetBaseNote());
    base.set_value(chord->GetBase());
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;
    
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::UpdateNotes(){
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    for (int x =0;x < 6; x++){
        note_buttons[x]->set_value(chord->GetNote(5-x));
    }
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;

}

void ChordWidget::OnRootChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    Gtk::TreeModel::Row row = *(combo_root.get_active());
    chord->SetRoot(row[m_columns_notes.note]);
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnBaseNoteChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    Gtk::TreeModel::Row row = *(combo_note.get_active());
    chord->SetBaseNote(row[m_columns_notes.note]);
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    base.set_value(chord->GetBase());
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnTriadModeChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    Gtk::TreeModel::Row row = *(combo_triad_mode.get_active());
    chord->SetTriadMode(row[m_columns_notes.note]);
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnGuitarModeChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    Gtk::TreeModel::Row row = *(combo_guitar_mode.get_active());
    chord->SetGuitarMode(row[m_columns_notes.note]);
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnBaseOctaveChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    chord->SetBaseOctave(octave.get_value());
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    base.set_value(chord->GetBase());
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnBaseChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    chord->SetBase(base.get_value());
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    octave.set_value(chord->GetBaseOctave());
    combo_note.set_active(chord->GetBaseNote());
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnInversionChanged(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    chord->SetInversion(inversion.get_value());
    UpdateNotes();
    on_changed.emit();
}

void ChordWidget::OnUseBaseToggled(){
    if(!AnythingSelected) return;
    if(we_are_copying_note_values_from_chord_so_do_not_handle_the_signals) return;
    chord->SetBaseUse(use_base.get_active());
    UpdateNotes();
    on_changed.emit();

}

void ChordWidget::Update(){
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = true;
    combo_root.set_active(chord->GetRoot()); //tricky
    combo_note.set_active(chord->GetBaseNote());
    combo_guitar_mode.set_active(chord->GetGuitarMode());
    combo_triad_mode.set_active(chord->GetTriadMode());
    octave.set_value(chord->GetBaseOctave());
    base.set_value(chord->GetBase());
    inversion.set_value(chord->GetInversion());
    use_base.set_active(chord->GetBaseUse());
    combo_type.set_active(chord->GetType());
    UpdateWhatToShowAndWhatIsSensitive();
    we_are_copying_note_values_from_chord_so_do_not_handle_the_signals = false;
    UpdateNotes();
}