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


#include "Files.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "messages.h"
#include "Event.h"

namespace Files {
    
void SaveToFile(){
    *dbg << "saiving to file!\n";
    Gtk::FileChooserDialog dialog(_("Choose a file to save..."),Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*mainwindow);
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,Gtk::RESPONSE_OK);

    Gtk::FileFilter hseq;
    hseq.set_name("HarmonySEQ files (*.hseq)");
    hseq.add_pattern("*.hseq");
    dialog.add_filter(hseq);
    Gtk::FileFilter all;
    all.set_name("All files");
    all.add_pattern("*");
    dialog.add_filter(all);

    Glib::KeyFile kf;

    int result = dialog.run();
    Glib::ustring filename = dialog.get_filename();
    char temp[300];
    char temp2[300];
    ofstream output_file;

    *dbg << "FILENAMEEND  =  " << filename.substr(filename.length()-5,5) << ENDL;
    if (dialog.get_filter() == &hseq) if(filename.size() < 5 || 0 != (filename.substr(filename.length()-5,5).compare(".hseq"))) { *dbg << "LOL";filename += ".hseq";}
    // 
    switch (result){
        case Gtk::RESPONSE_OK:


            output_file.open(filename.c_str(),ios_base::trunc);
            if(!output_file.good()){
                sprintf(temp,_("ERROR - error while opening file %s to write.\n"),filename.c_str());
                *err << temp;
                Info(temp);

            }
            sprintf(temp,"harmonyseq file, generated by harmonySEQ %s. Manual editing not suggested, since the program does not check whether the data here is correct - may cause crashes.",VERSION);
            kf.set_comment(temp);
            kf.set_string("harmonySEQ","version",VERSION);
            kf.set_integer("harmonySEQ","versionA",VERSION_A);
            kf.set_integer("harmonySEQ","versionB",VERSION_B);
            kf.set_integer("harmonySEQ","versionC",VERSION_C);
            kf.set_double(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_TEMPO,tempo);
            kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_MAINNOTE,mainnote);
            kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_SEQ_NUM,sequencers.size());
            kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_EVENTS_NUM,events.size());

            for (unsigned int x = 0; x < sequencers.size(); x++){
                if(sequencers[x] == NULL) continue;
                sprintf(temp,FILE_GROUP_TEMPLATE_SEQ,x);
                kf.set_string(temp,FILE_KEY_SEQ_NAME,sequencers[x]->GetName());
                kf.set_boolean(temp,FILE_KEY_SEQ_ON,sequencers[x]->GetOn());
                kf.set_integer(temp,FILE_KEY_SEQ_CHANNEL,sequencers[x]->GetChannel());
                kf.set_boolean(temp,FILE_KEY_SEQ_APPLY_MAIN_NOTE,sequencers[x]->GetApplyMainNote());
                kf.set_integer(temp,FILE_KEY_SEQ_VOLUME,sequencers[x]->GetVolume());
                kf.set_integer(temp,FILE_KEY_SEQ_RESOLUTION,sequencers[x]->resolution);
                kf.set_double(temp,FILE_KEY_SEQ_LENGTH,sequencers[x]->length);
                kf.set_integer_list(temp,FILE_KEY_SEQ_SEQUENCE,sequencers[x]->sequence);
                kf.set_integer_list(temp,FILE_KEY_SEQ_NOTES,sequencers[x]->notes);

            }

            for (unsigned int x = 0; x < events.size(); x++){
                if(events[x] == NULL) continue;
                sprintf(temp,FILE_GROUP_TEMPLATE_EVENT,x);
                kf.set_integer(temp,FILE_KEY_EVENT_TYPE,events[x]->type);
                kf.set_integer(temp,FILE_KEY_EVENT_ARG1,events[x]->arg1);
                kf.set_integer(temp,FILE_KEY_EVENT_ARG2,events[x]->arg2);
                kf.set_integer(temp,FILE_KEY_EVENT_ACTIONS_NUM,events[x]->actions.size());
                for (unsigned int a = 0; a < events[x]->actions.size(); a++){
                    if (events[x]->actions[a] == NULL) continue;
                    sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
                    kf.set_integer(temp,temp2,events[x]->actions[a]->type);
                    sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARG1,a);
                    kf.set_integer(temp,temp2,events[x]->actions[a]->arg1);
                    sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARG2,a);
                    kf.set_integer(temp,temp2,events[x]->actions[a]->arg2);
                }
            }


            //*dbg << kf.to_data();
            output_file << kf.to_data().c_str();

            output_file.close();
            break;
        case Gtk::RESPONSE_CANCEL:

            break;

        default:
            *dbg << "unknown response returned!\n";
        break;
    }

}

void LoadFileDialog(){
    Gtk::FileChooserDialog dialog(_("Choose a file to open..."),Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*mainwindow);
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,Gtk::RESPONSE_OK);

    Gtk::FileFilter hseq;
    hseq.set_name("HarmonySEQ files (*.hseq)");
    hseq.add_pattern("*.hseq");
    dialog.add_filter(hseq);
    Gtk::FileFilter all;
    all.set_name("All files");
    all.add_pattern("*");
    dialog.add_filter(all);

    int result = dialog.run();
    Glib::ustring filename = dialog.get_filename();
    dialog.hide();
    switch (result){
        case Gtk::RESPONSE_OK:
            LoadFile(filename);
            break;
        case Gtk::RESPONSE_CANCEL:
            break;
        default:
            *dbg << "unknown response returned!\n";
        break;
    }

    mainwindow->InitTreeData();
    mainwindow->main_note.set_value(mainnote);
    mainwindow->tempo_button.set_value(tempo);
    eventswindow->InitTreeData();
}


bool LoadFile(Glib::ustring file){

    *dbg << "trying to open |" << file <<"|--\n";
    int number;
    Glib::KeyFile kf;
    char temp[1000];
    char temp2[100];
    try{
        if (!kf.load_from_file(file)) {
            sprintf(temp, _("ERROR - error while trying to read file '%s'\n"), file.c_str());
            *err << temp;
            Info(temp);
            return 1;
        }
    }catch(Glib::Error error){
        sprintf(temp, _("ERROR - error while trying to read file '%s': "), file.c_str());
        *err << temp;
        *err << error.what();
        *err << ENDL;
        Info(temp,error.what());
        return 1;

    }

    try{
        int VA = kf.get_integer("harmonySEQ","versionA");
        int VB = kf.get_integer("harmonySEQ","versionB");
        int VC = kf.get_integer("harmonySEQ","versionC");
        if (VA > VERSION_A || (VA == VERSION_A && VB > VERSION_B) || (VA == VERSION_A && VB == VERSION_B && VC > VERSION_C)){
            sprintf(temp,_("This file was created by harmonySEQ in a newer version (%d.%d.%d). This means it may contain data that is suppored by the newer wersion, but not by the version you are using (%d.%d.%d). It is recommended not to open such file, since it is very likely it may produce strange errors, or may event crash the program unexpectedly. However, in some cases one may want to open such file anyway, for example if it is sure it will open without trouble. Select YES to do so."),VA,VB,VC,VERSION_A,VERSION_B,VERSION_C);
            if (Ask(_("Do you want to open this file?"),temp,false)){
                //anserwed YES;
            }else{
                return 1;
            }

        } else if (VA < VERSION_A || (VA == VERSION_A && VB < VERSION_B) || (VA == VERSION_A && VB == VERSION_B && VC < VERSION_C)){
            sprintf(temp,_("This file was created by harmonySEQ in an older version (%d.%d.%d). This means it may miss some data that is required to be in file by the version you are using (%d.%d.%d). It is recommended not to open such file, since it is very likely it may produce strange errors, or may event crash the program unexpectedly. However, in some cases one may want to open such file anyway, for example if it is sure it will open without trouble. Select YES to do so."),VA,VB,VC,VERSION_A,VERSION_B,VERSION_C);
            if (Ask(_("Do you want to open this file?"),temp,false)){
                //anserwed YES;
            }else{
                return 1;
            }
        }

        tempo = kf.get_double(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_TEMPO);
        mainnote = kf.get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_MAINNOTE);
        number = kf.get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_SEQ_NUM);


        clear_sequencers(); //woa hua hua hua!

        for (int x = 0; x < number; x++) {
            sprintf(temp, FILE_GROUP_TEMPLATE_SEQ, x);
            if (!kf.has_group(temp)) {

                sequencers.push_back(NULL);
                continue;
            }

            sequencers.push_back(new Sequencer());

            sequencers[x]->SetName(kf.get_string(temp, FILE_KEY_SEQ_NAME));
            sequencers[x]->SetOn(kf.get_boolean(temp, FILE_KEY_SEQ_ON));
            sequencers[x]->SetChannel(kf.get_integer(temp, FILE_KEY_SEQ_CHANNEL));
            sequencers[x]->SetApplyMainNote(kf.get_boolean(temp, FILE_KEY_SEQ_APPLY_MAIN_NOTE));
            if(kf.has_key(temp,FILE_KEY_SEQ_VOLUME))
                sequencers[x]->SetVolume(kf.get_integer(temp, FILE_KEY_SEQ_VOLUME));
            else //old file. does not have volume values in it.
                sequencers[x]->SetVolume(DEFAULT_VOLUME);
            sequencers[x]->resolution = kf.get_integer(temp, FILE_KEY_SEQ_RESOLUTION);
            sequencers[x]->length = kf.get_double(temp, FILE_KEY_SEQ_LENGTH);

            sequencers[x]->sequence.clear();
            std::vector<int> sequence = kf.get_integer_list(temp, FILE_KEY_SEQ_SEQUENCE);
            for (unsigned int n = 0; n < sequence.size(); n++) {
                sequencers[x]->sequence.push_back(sequence[n]);

            }

            sequencers[x]->notes.clear();
            *dbg << "now loading notes...\n";
            std::vector<int> notes = kf.get_integer_list(temp, FILE_KEY_SEQ_NOTES);
            for (unsigned int n = 0; n < notes.size(); n++) {
                *dbg << notes[n] << ENDL;
                sequencers[x]->notes.push_back(notes[n]);

            }
            sequencers[x]->UpdateGui();
        }

        int are_there_events_in_file = kf.has_key(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_EVENTS_NUM);
        if(!are_there_events_in_file) return 0; //THIS SHOULD BE REMOVED IN SOME NEWER VERSION, SINCE THERE ARE NOT MANY FILES OF VERSION 0.9 OR LOWER

        number = kf.get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_EVENTS_NUM);

        ClearEvents();

        for (int x = 0; x < number; x++){
            sprintf(temp, FILE_GROUP_TEMPLATE_EVENT, x);
            if (!kf.has_group(temp)) {

                events.push_back(NULL);
                continue;
            }
            events.push_back(new Event());
            *dbg << temp << ENDL;
            events[x]->type = kf.get_integer(temp,FILE_KEY_EVENT_TYPE);
            *dbg << events[x]->type<<ENDL;
            events[x]->arg1 = kf.get_integer(temp,FILE_KEY_EVENT_ARG1);
            events[x]->arg2 = kf.get_integer(temp,FILE_KEY_EVENT_ARG2);
            int actions_num = kf.get_integer(temp,FILE_KEY_EVENT_ACTIONS_NUM);
            for (int a = 0; a < actions_num; a++){
                sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
                if (!kf.has_key(temp,temp2)){
                    //there is no such action in file (was removed, was a NULL pointer while saving file)
                    events[x]->actions.push_back(NULL);
                    continue;
                }
                events[x]->actions.push_back(new Action(Action::NONE));

                events[x]->actions[a]->type = kf.get_integer(temp,temp2);
                sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARG1,a);
                events[x]->actions[a]->arg1 = kf.get_integer(temp,temp2);
                sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARG2,a);
                events[x]->actions[a]->arg2 = kf.get_integer(temp,temp2);

            }

            events[x]->UpdateGUI();
        }
    }catch(Glib::KeyFileError error){
        sprintf(temp, _("ERROR - Glib::KeyFile error while processing file '%s': "), file.c_str());
        *err << temp;
        *err << error.what();
        *err << ENDL;
        Info(temp,error.what());
        return 1;
        

    }catch(Glib::Error error){
        sprintf(temp, _("ERROR - unknown error while processing file '%s': "), file.c_str());
        *err << temp;
        *err << error.what();
        *err << ENDL;
        Info(temp,error.what());
        return 1;


    }
    return 0;


}
}//namespace files