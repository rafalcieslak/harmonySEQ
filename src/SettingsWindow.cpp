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

#include "SettingsWindow.h"
#include "Configuration.h"

SettingsWindow::SettingsWindow(){
    set_title(_("Settings"));
    set_border_width(5);
    set_resizable(0);

    add(main_vbox);
    main_vbox.pack_end(lower_hbox,Gtk::PACK_SHRINK);

    lower_hbox.pack_end(cancel_button,Gtk::PACK_SHRINK);
    lower_hbox.pack_end(ok_button,Gtk::PACK_SHRINK);
    lower_hbox.pack_start(restore_defaults_button,Gtk::PACK_SHRINK);
    
    cancel_button.set_label(_("Cancel"));
    ok_button.set_label(_("OK"));
    restore_defaults_button.set_label(_("Restore"));

    main_vbox.pack_start(notebook);
    notebook.append_page(page_main,_("General"));

    page_main.set_border_width(5);
    // <editor-fold defaultstate="collapsed" desc="metronome setting widgets placement">
    page_main.pack_start(metronome_label_hbox,Gtk::PACK_SHRINK);
    page_main.pack_start(metronome_channel_hbox,Gtk::PACK_SHRINK);
    metronome_channel_hbox.pack_start(metronome_channel_label,Gtk::PACK_SHRINK);
    metronome_channel_hbox.pack_start(metronome_channel,Gtk::PACK_SHRINK);
    metronome_label_hbox.pack_start(metronome_label,Gtk::PACK_SHRINK);
    page_main.pack_start(metronome_table);
    metronome_label.set_markup(_("<b>Metronome:</b>"));
    metronome_bar_label.set_text(_("Bar:"));
    metronome_1_4.set_label(_("1/4 bar:"));
    metronome_1_4.set_tooltip_markup(_("Mark here to metronome on every 1/4 of each bar."));
    metronome_1_4.signal_toggled().connect(mem_fun(*this,&SettingsWindow::OnUse14BarToggled));
    metronome_note_label.set_text(_("Note:"));
    metronome_velocity_label.set_text(_("Velocity:"));
    metronome_channel_label.set_text(_("Channel:"));

    metronome_table.resize(3, 3);
    metronome_table.set_border_width(2);
    metronome_table.attach(metronome_note_label,1,2,0,1);
    metronome_table.attach(metronome_velocity_label,2,3,0,1);
    metronome_table.attach(metronome_bar_label,0,1,1,2);
    metronome_table.attach(metronome_bar_note,1,2,1,2);
    metronome_table.attach(metronome_bar_velocity,2,3,1,2);
    metronome_table.attach(metronome_1_4,0,1,2,3);
    metronome_table.attach(metronome_1_4_note,1,2,2,3);
    metronome_table.attach(metronome_1_4_velocity,2,3,2,3);

    metronome_channel.set_range(1.0,16.0);
    metronome_channel.set_increments(1.0,1.0);
    metronome_channel.set_tooltip_markup(_("The channel the metronome will output it's ticks to."));

    metronome_1_4_note.set_range(0.0,127.0);
    metronome_1_4_note.set_increments(1.0,1.0);
    metronome_1_4_note.set_tooltip_markup(_("The metronome's MIDI note pitch"));
    metronome_1_4_velocity.set_range(0.0,127.0);
    metronome_1_4_velocity.set_increments(1.0,1.0);
    metronome_1_4_velocity.set_tooltip_markup(_("The metronome's MIDI note velocity'"));
    metronome_bar_note.set_range(0.0,127.0);
    metronome_bar_note.set_increments(1.0,1.0);
    metronome_bar_note.set_tooltip_markup(_("The metronome's MIDI note pitch"));
    metronome_bar_velocity.set_range(0.0,127.0);
    metronome_bar_velocity.set_increments(1.0,1.0);
    metronome_bar_velocity.set_tooltip_markup(_("The metronome's MIDI note velocity'"));
    // </editor-fold>

    signal_show().connect(mem_fun(*this,&SettingsWindow::OnShowed));

    ok_button.signal_clicked().connect(mem_fun(*this,&SettingsWindow::OnOKClicked));
    cancel_button.signal_clicked().connect(mem_fun(*this,&SettingsWindow::OnCancelClicked));
    restore_defaults_button.signal_clicked().connect(mem_fun(*this,&SettingsWindow::OnRestoreDefaults));

    show_all();
    hide();
}

SettingsWindow::~SettingsWindow(){
    
}

void SettingsWindow::OnShowed(){
    LoadDataFromConfig();
}

void SettingsWindow::LoadDataFromConfig(){
    metronome_channel.set_value(Config::MetronomeChannel);
    metronome_1_4_note.set_value(Config::MetronomeHit2Note);
    metronome_1_4_velocity.set_value(Config::MetronomeHit2Velocity);
    metronome_bar_note.set_value(Config::MetronomeHit1Note);
    metronome_bar_velocity.set_value(Config::MetronomeHit1Velocity);
    metronome_1_4.set_active(Config::MetronomeHit2);
    OnUse14BarToggled(); //Signal handler won't get called automatically, so we'll call it here, to update widget's sensitivity;
    
}

void SettingsWindow::StoreDataToConfig(){
    Config::MetronomeChannel = metronome_channel.get_value();
    Config::MetronomeHit2Note = metronome_1_4_note.get_value();
    Config::MetronomeHit2Velocity = metronome_1_4_velocity.get_value();
    Config::MetronomeHit1Note = metronome_bar_note.get_value();
    Config::MetronomeHit1Velocity = metronome_bar_velocity.get_value();
    Config::MetronomeHit2 = metronome_1_4.get_active();
    
}

void SettingsWindow::OnCancelClicked(){
    //LoadDataFromConfig(); //reload the data is unneeded, as it will be done on show.
    hide();
}

void SettingsWindow::OnOKClicked(){
    StoreDataToConfig();
    Config::SaveToFile();
    hide();
}

void SettingsWindow::OnUse14BarToggled(){
    if (metronome_1_4.get_active()){
        metronome_1_4_note.set_sensitive(1);
        metronome_1_4_velocity.set_sensitive(1);
    }else{
        metronome_1_4_note.set_sensitive(0);
        metronome_1_4_velocity.set_sensitive(0);
    }

}

void SettingsWindow::OnRestoreDefaults(){
    if(Ask(_("Restoring default configuration"), _("Are you sure you want to loose all your settings?"))){
        Config::LoadDefaultConfiguration();
        LoadDataFromConfig();
        Config::SaveToFile();
    }else{
        return;
    }


}