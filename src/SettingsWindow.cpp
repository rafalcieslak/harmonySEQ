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
#include "MainWindow.hpp"
#include "SettingsWindow.hpp"
#include "Configuration.hpp"

SettingsWindow::SettingsWindow(){
    hide();
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
    notebook.append_page(page_midi,_("MIDI"));
    notebook.append_page(page_OSC,_("OSC"));

    page_main.set_border_width(5);
    page_midi.set_border_width(5);
    page_OSC.set_border_width(5);

    page_main.pack_start(disable_diodes_vbox);

    disable_diodes_vbox.pack_start(disable_diodes_label_hbox);
    disable_diodes_label_hbox.pack_start(disable_diodes_label,Gtk::PACK_SHRINK);
    disable_diodes_vbox.pack_start(disable_diodes,Gtk::PACK_SHRINK);
    disable_diodes_label.set_markup(_("<b>Disable diodes</b>"));
    disable_diodes.set_label(_("Disable all animated diodes"));
    disable_diodes.set_tooltip_markup(_("Disables animated diodes, which may improve performance.\n<b>Requires restart to take effect.</b>"));

    page_main.pack_start(sepFPS);
    page_main.pack_start(boxFPS);
    boxFPS.pack_start(boxFPSlabel);
    boxFPS.pack_start(boxFPSbutton_label);
    boxFPSlabel.pack_start(FPSlabel,Gtk::PACK_SHRINK);
    boxFPSbutton_label.pack_start(FPSbutton_label,Gtk::PACK_SHRINK);
    boxFPSbutton_label.pack_start(FPSbutton,Gtk::PACK_SHRINK);
    FPSlabel.set_markup(_("<b>Pattern display quality</b>"));
    FPSbutton_label.set_text(_("Max. FPS: "));
    FPSbutton.set_range(1.0,200.0);
    FPSbutton.set_increments(1.0,10.0);
    FPSbutton.set_tooltip_markup(_("The <b>maximum</b> FPS value used when smooth animation is needed, for example when dragging notes. Note this is the maximum value, it is usually much much lower, as the pattern is redrawn only when nessesary.\n\nHigh values may cause higher CPU usage when dragging notes, moving graph points etc., but result in much smoothier animations."));
    FPSbutton_label.set_tooltip_markup(_("The <b>maximum</b> FPS value used when smooth animation is needed, for example when dragging notes. Note this is the maximum value, it is usually much much lower, as the pattern is redrawn only when nessesary.\n\nHigh values may cause higher CPU usage when dragging notes, moving graph points etc., but result in much smoothier animations."));

    page_main.pack_start(sep1);

    // <editor-fold defaultstate="collapsed" desc="visible colums">
    page_main.pack_start(columns_label_hbox);
    page_main.pack_start(colums_vbox);


    columns_label_hbox.pack_start(colums_label,Gtk::PACK_SHRINK);
    colums_label.set_markup(_("<b>Visible colums:</b>"));

    colums_vbox.pack_start(colums_name);
    colums_vbox.pack_start(colums_onoff);
    colums_vbox.pack_start(colums_channel);
    colums_vbox.pack_start(colums_pattern);
    colums_vbox.pack_start(colums_resolution);
    colums_vbox.pack_start(colums_length);
    colums_vbox.pack_start(colums_chord);

    colums_name.set_label(_("Name"));
    colums_onoff.set_label(_("Channel"));
    colums_channel.set_label(_("Channel"));
    colums_pattern.set_label(_("Patern"));
    colums_resolution.set_label(_("Resolution"));
    colums_length.set_label(_("Length"));
    colums_chord.set_label(_("Chord (notes) / Ctrl. No."));

    //these are fake checkbuttons, for it is impossible to hide the name or the on/off switch
    colums_name.set_active(1);
    colums_name.set_sensitive(0);
    colums_onoff.set_active(1);
    colums_onoff.set_sensitive(0);
    // </editor-fold>
    // <editor-fold defaultstate="collapsed" desc="metronome setting widgets placement">
    page_midi.pack_start(metronome_label_hbox,Gtk::PACK_SHRINK);
    page_midi.pack_start(metronome_channel_hbox,Gtk::PACK_SHRINK);
    metronome_channel_hbox.pack_start(metronome_channel_label,Gtk::PACK_SHRINK);
    metronome_channel_hbox.pack_start(metronome_channel,Gtk::PACK_SHRINK);
    metronome_label_hbox.pack_start(metronome_label,Gtk::PACK_SHRINK);
    page_midi.pack_start(metronome_table,Gtk::PACK_SHRINK);
    page_midi.pack_start(sep2,Gtk::PACK_SHRINK);
    metronome_label.set_markup(_("<b>Metronome:</b>"));
    metronome_bar_label.set_text(_("Bar:"));
    metronome_1_4.set_label(_("1/4 bar:"));
    metronome_1_4.set_tooltip_markup(_("Mark here, and the metronome will also tick on every 1/4 of each bar."));
    metronome_1_4.signal_toggled().connect(std::bind(&SettingsWindow::OnUse14BarToggled, this));
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

    page_OSC.pack_start(osc_port_label_hbox,Gtk::PACK_SHRINK);
    page_OSC.pack_start(osc_note_restart,Gtk::PACK_SHRINK);
    page_OSC.pack_start(osc_sep,Gtk::PACK_SHRINK);
    page_OSC.pack_start(osc_description_hbox,Gtk::PACK_SHRINK);
    osc_port_label_hbox.pack_start(osc_port_label,Gtk::PACK_SHRINK);
    osc_port_label_hbox.pack_start(osc_port,Gtk::PACK_SHRINK);
    osc_port_label.set_text(_("OSC port: "));
    osc_port.set_range(0.0,65536.0);
    osc_port.set_increments(1.0,1024.0);
    osc_port.set_tooltip_markup(_("The port OSC server will listen at."));
    osc_note_restart.set_markup(_("<span size='small'>Changing this setting needs restarting\nharmonySEQ to take effect.</span>"));
    osc_description_hbox.pack_start(osc_description,Gtk::PACK_SHRINK);
    osc_description.set_markup(_("\n<span size='small'><b>harmonySEQ supports following OSC messages:</b>\n<i>/harmonyseq/pause</i> and <i>/harmonyseq/play</i>\n<i>/harmonyseq/tempo [float]</i>\n<i>/harmonyseq/sync</i> (for synchronizing)\n<i>/karmonyseq/kill</i> (to quit immidiatelly)\n<i>/harmonyseq/event [int]</i> (to trigger events with\nthe given tag).</span>"));


    ok_button.signal_clicked().connect(std::bind(&SettingsWindow::OnOKClicked, this));
    cancel_button.signal_clicked().connect(std::bind(&SettingsWindow::OnCancelClicked, this));
    restore_defaults_button.signal_clicked().connect(std::bind(&SettingsWindow::OnRestoreDefaults, this));

    show_all();
    hide();

    signal_show().connect(std::bind(&SettingsWindow::OnShowed, this));
}

SettingsWindow::~SettingsWindow(){

}

void SettingsWindow::OnShowed(){
    LoadDataFromConfig();
}

void SettingsWindow::LoadDataFromConfig(){
    metronome_channel.set_value(Config::Metronome::Channel);
    metronome_1_4_note.set_value(Config::Metronome::Hit2Note);
    metronome_1_4_velocity.set_value(Config::Metronome::Hit2Velocity);
    metronome_bar_note.set_value(Config::Metronome::Hit1Note);
    metronome_bar_velocity.set_value(Config::Metronome::Hit1Velocity);
    metronome_1_4.set_active(Config::Metronome::Hit2);
    OnUse14BarToggled(); //Signal handler won't get called automatically, so we'll call it here, to update widget's sensitivity;
    colums_channel.set_active(Config::VisibleColumns::Channel);
    colums_pattern.set_active(Config::VisibleColumns::Pattern);
    colums_resolution.set_active(Config::VisibleColumns::Resolution);
    colums_length.set_active(Config::VisibleColumns::Length);
    colums_chord.set_active(Config::VisibleColumns::ChordAndCtrlNo);
    disable_diodes.set_active(Config::Interaction::DisableDiodes);
    FPSbutton.set_value(1000/Config::Interaction::PatternRefreshMS);
    osc_port.set_value(Config::OSC::Port);

}

void SettingsWindow::StoreDataToConfig(){
    Config::Metronome::Channel = metronome_channel.get_value();
    Config::Metronome::Hit2Note = metronome_1_4_note.get_value();
    Config::Metronome::Hit2Velocity = metronome_1_4_velocity.get_value();
    Config::Metronome::Hit1Note = metronome_bar_note.get_value();
    Config::Metronome::Hit1Velocity = metronome_bar_velocity.get_value();
    Config::Metronome::Hit2 = metronome_1_4.get_active();
    Config::VisibleColumns::Channel = colums_channel.get_active();
    Config::VisibleColumns::Pattern = colums_pattern.get_active();
    Config::VisibleColumns::Resolution = colums_resolution.get_active();
    Config::VisibleColumns::Length = colums_length.get_active();
    Config::VisibleColumns::ChordAndCtrlNo  = colums_chord.get_active();
    Config::Interaction::DisableDiodes = disable_diodes.get_active();
    Config::Interaction::PatternRefreshMS = 1000/FPSbutton.get_value();
    Config::OSC::Port = osc_port.get_value();

    Config::on_changed();
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
