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

#include <fstream>
#include <map>
#include "Files.hpp"
#include "messages.hpp"
#include "Sequencer.hpp"
#include "Event.hpp"
#include "Action.hpp"
#include "MidiDriver.hpp"
#include "NoteSequencer.hpp"
#include "ControlSequencer.hpp"
#include "NoteAtom.hpp"
#include "ControllerAtom.hpp"
#include "SequencerManager.hpp"

namespace Files {

bool file_modified = false;
Glib::ustring file_name;
Glib::ustring file_dir;

bs2::signal<void()> on_file_loaded;
bs2::signal<void()> on_file_saved;
bs2::signal<void()> on_file_modified;

//Returns 1 if opening file succeeded, 0 elsewhere.
bool fexists(const char *filename)
{
    std::ifstream ifile(filename);
    return !ifile.fail();
}

bool SetFileModified(bool modified){
    if (modified != file_modified){
        file_modified = modified;
        on_file_modified();
    }
    else
        file_modified = modified;

    return file_modified;
}

void FileModified(){
    SetFileModified(true);
}

void LoadFileDialog(Gtk::Window* parent){
    //Creating a new File Choosing Dialog
    Gtk::FileChooserDialog dialog(_("Choose a file to open..."),Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*parent);
    //And adding to it some buttons.
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OPEN,Gtk::RESPONSE_OK);

    //Creating filters...
    Glib::RefPtr<Gtk::FileFilter> hseq = Gtk::FileFilter::create();
    hseq->set_name("HarmonySEQ files (*.hseq)");
    hseq->add_pattern("*.hseq");
    dialog.add_filter(hseq);
    Glib::RefPtr<Gtk::FileFilter> all = Gtk::FileFilter::create();
    all->set_name("All files");
    all->add_pattern("*");
    dialog.add_filter(all);

    //Running the dialog - passing the control to it, and waiting for the user to choose a file.
    int result = dialog.run();

    //Obtaining the file the user has chosen..
    Glib::ustring filename = dialog.get_filename();

    //Hiding the dialog...
    dialog.hide();

    if (result == Gtk::RESPONSE_CANCEL)
        return;
    if (result != Gtk::RESPONSE_OK){
        *err << "Unexpected dialog response: " << result << ENDL;
    }

    // To avoid lag pause engine while file is being processed.
    bool was_paused = midi->GetPaused();
    if (!was_paused) midi->PauseImmediately();

    LoadFile(filename);
    SetFileModified(0);

    if (!was_paused) midi->Unpause();
}

void SaveFileDialog(Gtk::Window* parent){

    Gtk::FileChooserDialog dialog(_("Choose a file to save..."),Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*parent);
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,Gtk::RESPONSE_OK);

    Glib::RefPtr<Gtk::FileFilter> hseq = Gtk::FileFilter::create();
    hseq->set_name("HarmonySEQ files (*.hseq)");
    hseq->add_pattern("*.hseq");
    dialog.add_filter(hseq);
    Glib::RefPtr<Gtk::FileFilter> all = Gtk::FileFilter::create();
    all->set_name("All files");
    all->add_pattern("*");
    dialog.add_filter(all);

    if (Files::file_name != "")
        dialog.set_filename(Files::file_dir+Files::file_name);

    int result = dialog.run();

    Glib::ustring filename = dialog.get_filename();

    if (result == Gtk::RESPONSE_CANCEL)
        return;
    if (result != Gtk::RESPONSE_OK){
        *err << "Unexpected dialog response: " << result << ENDL;
    }

    //add .hseq extention
    if (dialog.get_filter() == hseq
        and (filename.size() < 5
             or (filename.substr(filename.length() - 5, 5).compare(".hseq")) != 0)) {
        filename += ".hseq";
    }

    //check whether it already exists
    if (Files::fexists(filename.c_str())) {
        char temp[300];
        sprintf(temp, _("File '%s'  already exist."), filename.c_str());
        if (!Ask(temp, _("Do you want to overwrite this file?")))
            return; //user choosed not to overwrite it.
    }

    Files::SaveToFile(filename);

}

bool LoadFile(Glib::ustring file){


    *dbg << "trying to open |" << file <<"|--\n";
    Glib::KeyFile kf;
    char temp[3000];
    char temp2[3000];


    //We'll try to open a file. No i/o streams needed here, KeyFile provides us with a useful method load_from_file, which does all the magic.
    try{
        if (!kf.load_from_file(file)) {
            //Returned 1, so something strange is with the file.
            sprintf(temp, _("ERROR - error while trying to read file '%s'\n"), file.c_str());
            *err << temp;
            Info(temp);
            return 1;
        }
    }catch(const Glib::Error& e){
        //Exception cought. So can even tell what's wrong (usually some filesystem-related problem, not a problem with it's contains).
        sprintf(temp, _("ERROR - error while trying to read file '%s': "), file.c_str());
        *err << temp;
        *err << e.what();
        *err << ENDL;
        Info(temp,e.what());
        return 1;
    }


    //A large TRY-CATCH for almost whole LoadFile procedure.
    try{
        //At the very beggining we should check the version of the file.
        int VA = kf.get_integer("harmonySEQ","versionA");
        int VB = kf.get_integer("harmonySEQ","versionB");
        int VC = kf.get_integer("harmonySEQ","versionC");

        /*Error flag.*/
        bool e_flag = 0;

        //Compaing versions...
        if (VA > VERSION_A || (VA == VERSION_A && VB > VERSION_B) || (VA == VERSION_A && VB == VERSION_B && VC > VERSION_C)){
            //File is too new
            sprintf(temp,_("This file was created by harmonySEQ in a newer version (%d.%d.%d). This means it may contain data that is suppored by the newer wersion, but not by the version you are using (%d.%d.%d). It is recommended not to open such file, since it is very likely it may produce strange errors, or may event crash the program unexpectedly. However, in some cases one may want to open such file anyway, for example if it is sure it will open without trouble. Select YES to do so."),VA,VB,VC,VERSION_A,VERSION_B,VERSION_C);
            if (Ask(_("Do you want to open this file?"),temp)){
                //anserwed YES;
                //Lets try to open it with the lastest routine. So just skip...
            }else{
                return 1;
            }

        } else if (VA < VERSION_A || (VA == VERSION_A && VB < VERSION_B) || (VA == VERSION_A && VB == VERSION_B && VC < VERSION_C)){
            //File is older
            sprintf(temp,_("This file was created by older harmonySEQ version (%d.%d)"),VA,VB);
            sprintf(temp2,_("This file contains data in format, which is too old."));
            if(VA == 0 && VB < 15){
              // Unsupported.
                Info(temp2,_("Sorry, but harmonySEQ cannot convert this file to current format.\nThis applies only to files from harmonySEQ older than 0.15.\n\nIf you really need to open this file, try downloading harmonySEQ 0.15, and using it to convert this file to 0.15 format, which will make it openable with this harmonySEQ version."));
                return 1;
            }
            if(VA == 0 && VB == 15){
                Info(temp,_("harmonySEQ will convert this file from 0.15 to 0.16 format."));
                e_flag = ConvertFile_0_15_to_0_16(&kf);
                VA = 0; VB = 16; VC = 0;
            }
            if(VA == 0 && VB == 16){
                Info(temp,_("harmonySEQ will convert this file from 0.16 to 0.17 format."));
                e_flag = ConvertFile_0_16_to_0_17(&kf);
                VA = 0; VB = 17; VC = 0;
            }

        }

        if(!e_flag)
                e_flag = LoadFileCurrent(&kf);


        //In case of errors...
        if (e_flag) {
            sprintf(temp, _("ERROR - error while reading file '%s'\n"), file.c_str());
            *err << temp;
            Info(temp);
            return 1;
        }


        //At beggining, file is unmodidied.
        SetFileModified(0);

        //Looking for the last '/' or '\' in the file patch, and storing file name and patch in appropriate variable.
        int found =  file.find_last_of("/\\");  //Will work on linux and windows both
        file_name = file.substr(found+1);
        file_dir = file.substr(0,found+1);

        //To make sure all goes well:
        midi->Sync();

        on_file_loaded();
        /* TODO: No need to emit this sequencer list change -
         * replacing sequencers list triggers this anyway. */
        SequencerManager::on_sequencer_list_changed();
        on_events_list_changed();

    //Only exception handles are left...
    }catch(const Glib::KeyFileError& e){
        //KeyFile error means some trouble with data in the file. Missing key, wrong format, wrong characters, all these goes here.
        sprintf(temp, _("ERROR - Glib::KeyFile error while processing file '%s': "), file.c_str());
        *err << temp;
        *err << e.what();
        *err << ENDL;
        Info(temp,e.what());
        return 1;
    }catch(const Glib::Error& e){
        //Some other strange file-related errors are cought here.
        sprintf(temp, _("ERROR - unknown error while processing file '%s': "), file.c_str());
        *err << temp;
        *err << e.what();
        *err << ENDL;
        Info(temp,e.what());
        return 1;
    }
    return 0;


}

void SaveToFile(Glib::ustring filename){

    //This routine should always save using current format.

    //The keyfile intepreter.
    Glib::KeyFile kf;

    //temporary strings
    char temp[300];
    char temp2[300];

    //The output file stream we'll put the data into.
    std::ofstream output_file;

    //Trying to open the file, using Truncate mode.
    output_file.open(filename.c_str(), std::ios_base::trunc);

    //If something went wrong...
    if(!output_file.good()){
        sprintf(temp,_("ERROR - error while opening file %s to write.\n"),filename.c_str());
        //Inform the user about the mistake both by STDIO and a nice graphical MessageBox.
        *err << temp;
        Info(temp);
    }

    //The comment in the very first line of .hseq file.
    sprintf(temp,"harmonyseq file, generated by harmonySEQ %s. Manual editing not suggested, since the program does not check whether the data here is correct - may cause crashes.",VERSION);
    kf.set_comment(temp);

    //Setting some basic data...
    kf.set_string("harmonySEQ","version",VERSION);
    kf.set_integer("harmonySEQ","versionA",VERSION_A);
    kf.set_integer("harmonySEQ","versionB",VERSION_B);
    kf.set_integer("harmonySEQ","versionC",VERSION_C);
    kf.set_double("System","tempo",midi->GetTempo());

    auto seqs = SequencerManager::GetAll();
    kf.set_integer("System","sequencers_number", seqs.size());
    kf.set_integer("System","events_number", Events.size());

    int x = 0;
    //And now, save the sequencers.
    //For each sequencer...
    for (auto seq : seqs){
        if(seq == NULL) continue; //Skip this sequencer, if it was removed.
        //Prepare the KEY for this sequencer, according to the pattern in FILE_GROUP_TEMPLATE_SEQ, it will be used to store all  values of this sequencer.
        sprintf(temp,"Seq %d",x);
        //And store the values, as follows:
        kf.set_string(temp,"name",seq->GetName());
        kf.set_boolean(temp,"on",seq->GetOn());
        kf.set_integer(temp,"channel",seq->GetChannel());
        kf.set_integer(temp,"resolution",seq->GetResolution());
        kf.set_integer(temp,"length_numerator",seq->GetLengthNumerator());
        kf.set_integer(temp,"length_denominator",seq->GetLengthDenominator());
        int seq_type = seq->GetType();
        kf.set_integer(temp,"seq_type",seq_type);
        kf.set_integer(temp,"patterns_num",seq->patterns.size());
        kf.set_integer(temp,"active_pattern",seq->GetActivePatternNumber());
        //Now, save the patterns.
        if(seq_type == SEQ_TYPE_NOTE){
                auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(seq);
                kf.set_boolean(temp,"expand_chord",noteseq->expand_chord);
                kf.set_integer(temp,"gate_percent", noteseq->GetGatePercent());
                //For each pattern in this sequencer...
                for (int s = 0; s < (int)seq->patterns.size(); s++) {
                    sprintf(temp2, "pattern_%d", s);
                    int notes = seq->patterns[s]->GetSize();
                    std::vector<double> S(notes * 4);
                    for (int r = 0; r < notes; r++) {
                        NoteAtom* note = dynamic_cast<NoteAtom*>(seq->patterns[s]->get(r));
                        S[4*r] = note->time;
                        S[4*r+1] = (double)note->pitch;
                        S[4*r+2] = (double)note->velocity;
                        S[4*r+3]= note->length;
                    }
                    //And store this vector in the keyfile as a list.
                    kf.set_double_list(temp, temp2, S);
                }
                //Finally, store the chord as a list of parameters.
                kf.set_integer_list(temp,"chord",noteseq->chord.SaveToVector());
        }else if(seq_type == SEQ_TYPE_CONTROL){
                auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer> (seq);
                kf.set_integer(temp,"controller", ctrlseq->GetControllerNumber());
                //For each pattern in this sequencer...
                for (int s = 0; s < (int) seq->patterns.size(); s++) {
                    sprintf(temp2, "pattern_%d", s);
                    int notes = seq->patterns[s]->GetSize();
                    std::vector<double> S(notes * 3);
                    for (int r = 0; r < notes; r++) {
                        ControllerAtom* ctrl = dynamic_cast<ControllerAtom*> (seq->patterns[s]->get(r));
                        S[3 * r] = ctrl->time;
                        S[3 * r + 1] = (double) ctrl->value;
                        S[3 * r + 2] = (double) ctrl->slope_type;
                    }
                    //And store this vector in the keyfile as a list.
                    kf.set_double_list(temp, temp2, S);
                }
        }
        x++;
    }

    //Then, save the events.
    //For each event...
    for (unsigned int x = 0; x < Events.size(); x++){
        auto ev = Events[x];
        if(ev == NULL) continue; //If it was removed, skip to the next one.
        //Prepare the key, according to the pattern in FILE_GROUP_TEMPLATE_EVENT...
        sprintf(temp,"Event %d",x);
        //Store some basic data.
        kf.set_integer(temp,"type",ev->type);
        kf.set_integer(temp,"arg1",ev->arg1);
        kf.set_integer(temp,"arg2",ev->arg2);
        kf.set_integer(temp,"actions",ev->actions.size());

        //Now save all actions of this event.
        //For each action of this event...
        for (unsigned int a = 0; a < ev->actions.size(); a++){
            auto ac = ev->actions[a];
            if (ac == NULL) continue;//If the action was removed, skip to the next one.
            //Save actions's data.
            sprintf(temp2,"Action_%d_type",a);
            kf.set_integer(temp,temp2,ac->type);
            sprintf(temp2,"Action_%d_args",a);
            kf.set_integer_list(temp,temp2,ac->args);
            sprintf(temp2,"Action_%d_chord",a);
            kf.set_integer_list(temp,temp2,ac->chord.SaveToVector());

            sprintf(temp2,"Action_%d_seq",a);
            std::shared_ptr<Sequencer> target = ac->target_seq.lock();
            if(target)
                for(int r = 0; r < seqs.size(); r++)
                    if(seqs[r] == target)
                        kf.set_integer(temp,temp2,r);
        }
    }


    //Output to file. (kf.to_data() outputs plain-text output.)
    output_file << kf.to_data().c_str();

    //File is saved, we are so nice to close it.
    output_file.close();

    int found;
    //Looking for the last '/' or '\' in the file patch, and storing file name and patch in appropriate variable.
    found =  filename.find_last_of("/\\");
    file_name = filename.substr(found+1);
    file_dir = filename.substr(0,found+1);

    //File has no unsaved changes now, so...
    SetFileModified(0);

    on_file_saved();
}

bool LoadFileCurrent(Glib::KeyFile* kfp){

        int seqNum;
        char temp[3000];
        char temp2[1000];

        //Read some basic data...
        midi->SetTempo(kfp->get_double("System", "tempo"));
        seqNum = kfp->get_integer("System", "sequencers_number");

        //Get rid of any seqeuncers.
        SequencerManager::Clear();

        //Now we'll process all sequencers that are in the file.
        std::vector<std::shared_ptr<Sequencer>> seqs(seqNum);

        for (int x = 0; x < seqNum; x++) {
            //First prepare the key.
            sprintf(temp, "Seq %d", x);

            //In case there is no such sequencer...
            if (!kfp->has_group(temp)) {
                sprintf(temp, _("ERROR - Missing sequencer %d in file\n"), x);
                *err << temp;
                return 1;
            }
            int seq_type = kfp->get_integer(temp,"seq_type");

            std::shared_ptr<Sequencer> seq;
            //If we got here, this means this sequencer was NOT removed. So: let's create it.
            if(seq_type == SEQ_TYPE_NOTE) seq = std::make_shared<NoteSequencer>();
            else if (seq_type == SEQ_TYPE_CONTROL) seq = std::make_shared<ControlSequencer>();

            seqs[x] = seq;

            //Put some data into it...
            seq->SetName(kfp->get_string(temp, "name"));
            seq->SetOn(kfp->get_boolean(temp, "on"));
            seq->SetChannel(kfp->get_integer(temp, "channel"));
            seq->SetResolution(kfp->get_integer(temp, "resolution"));
            seq->SetLength(kfp->get_integer(temp, "length_numerator"),kfp->get_integer(temp, "length_denominator"));

            /* Remove the original pattern that each sequencer gets by default. */
            seq->patterns.clear();

            int n = kfp->get_integer(temp, "patterns_num");

            if(seq_type == SEQ_TYPE_NOTE){
                    auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(seq);

                    noteseq->expand_chord = kfp->get_boolean(temp,"expand_chord");
                    if(kfp->has_key(temp, "gate_percent"))
                        noteseq->SetGatePercent(kfp->get_integer(temp, "gate_percent"));

                    //For each pattern we load...
                    for (int s = 0; s < n; s++) {
                        auto pattern = std::make_shared<AtomContainer>();
                        //Prepare the value name...
                        sprintf(temp2, "pattern_%d", s);
                        //And get the pattern from file
                        std::vector<double> pattern_data = kfp->get_double_list(temp, temp2);

                        if(pattern_data.size()%4 != 0){*err << "ERROR - pattern size in file %4 != 0  - aborting opening"; return 1;}
                        //Simple algorithm, just copying data from the pattern from file to the pattern in the sequencer
                        for (unsigned int w = 0; w < pattern_data.size(); w+=4) {
                            NoteAtom *note = new NoteAtom;
                            note->time = pattern_data[w];
                            note->pitch = pattern_data[w + 1];
                            note->velocity = pattern_data[w + 2];
                            note->length = pattern_data[w + 3];
                            pattern->Add(note);
                        }
                        noteseq->AddPattern(pattern);
                    }//next pattern

                    //Here we load the chord (if any)
                    if (kfp->has_key(temp, "chord")) {
                        std::vector<int> vec = kfp->get_integer_list(temp, "chord");
                        noteseq->chord.SetFromVector(vec);
                    }

            }else if (seq_type == SEQ_TYPE_CONTROL) {

                auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(seq);

                ctrlseq->SetControllerNumber(kfp->get_integer(temp, "controller"));

                //For each pattern we load...
                for (int s = 0; s < n; s++) {
                    auto pattern = std::make_shared<AtomContainer>();

                    //Prepare the value name...
                    sprintf(temp2, "pattern_%d", s);
                    //And get the pattern from file
                    std::vector<double> pattern_data = kfp->get_double_list(temp, temp2);

                    if (pattern_data.size() % 3 != 0) {
                        *err << "ERROR - pattern size in file %3 != 0  - aborting opening";
                        return 1;
                    }
                    //Simple algorithm, just copying data from the pattern from file to the pattern in the sequencer
                    for (unsigned int w = 0; w < pattern_data.size(); w += 3) {
                            ControllerAtom *ctrl = new ControllerAtom;
                            ctrl->time = pattern_data[w];
                            ctrl->value = pattern_data[w+1];
                            if(pattern_data[w+2] == SLOPE_TYPE_FLAT) ctrl->slope_type = SLOPE_TYPE_FLAT;
                            else if(pattern_data[w+2] == SLOPE_TYPE_LINEAR) ctrl->slope_type = SLOPE_TYPE_LINEAR;
                            pattern->Add(ctrl);
                    }

                    seq->AddPattern(pattern);
                }//next pattern

            }

            seq->SetActivePatternNumber(kfp->get_integer(temp,"active_pattern"));

            //Just to make sure, check if the sequencer we've just loaded from file has any patterns...
            if (seq->patterns.size() == 0)
                /* There were no patterns in the file - likely a
                 * bug. Any sequencer needs at least one pattern. */
                seq->AddPattern(std::make_shared<AtomContainer>());

            //Now proceed to the...
        } //...next sequencer.

        //Done loading sequencers.

        //Now, proceed to events.

        //No such key? something really wrong.
        if (!kfp->has_key("System", "events_number")) {
            *err << "ERROR - No events_number key in file.\n";
            return 1;
        }

        //Number of events.
        seqNum = kfp->get_integer( "System", "events_number");

        //Get rid of any events.
        ClearEvents();
        //For each event in file...
        for (int x = 0; x < seqNum; x++) {
            sprintf(temp, "Event %d", x);
            //If there is no key related to this number of event, this means this event was removed.
            if (!kfp->has_group(temp)) {
                //So we'll put instead a null pointer, so it'll look as a removed one, and skip to look for the next event.
                Events.push_back(NULL);
                continue;
            }
            //First, create a new event...
            Event* ev = new Event();
            //Put some data into it...
            ev->type = kfp->get_integer(temp, "type");
            ev->arg1 = kfp->get_integer(temp, "arg1");
            ev->arg2 = kfp->get_integer(temp, "arg2");
            int actions_num = kfp->get_integer(temp, "actions");
            //For each action of this event...
            for (int a = 0; a < actions_num; a++) {
                sprintf(temp2, "Action_%d_type", a);
                if (!kfp->has_key(temp, temp2)) {
                    //there is no such action in file (was removed, was a NULL pointer while saving file) - skip to next action
                    ev->actions.push_back(NULL);
                    continue;
                }
                //Create a new action
                Action* ac = new Action(Action::NONE);

                //Fill it with data: type...
                sprintf(temp2, "Action_%d_type", a);
                ac->type = kfp->get_integer(temp, temp2);
                //...arguments...
                sprintf(temp2, "Action_%d_args", a);
                ac->args = kfp->get_integer_list(temp, temp2);

                //...a sequencer, if any....
                sprintf(temp2, "Action_%d_seq", a);
                if (kfp->has_key(temp, temp2)) {
                    int seqid = kfp->get_integer(temp, temp2);
                    ac->target_seq = seqs[seqid];
                }

                //...and a chord, if any.
                sprintf(temp2, "Action_%d_chord", a);
                if (kfp->has_key(temp, temp2)) {
                    std::vector<int> vec = kfp->get_integer_list(temp, temp2);
                    ac->chord.SetFromVector(vec);
                }

                ev->actions.push_back(ac);
            }//next action.

            Events.push_back(ev);
        }//next event.

        /* Finally, apply the change to the list of registered sequencers. */
        SequencerManager::ReplaceAll(seqs);

        //And that's all, file is loaded.
        return 0;
}

bool ConvertFile_0_15_to_0_16(Glib::KeyFile* kfp){
    //done.
    double tempo = kfp->get_double("System","tempo");
    tempo /= 4;
    kfp->remove_key("System","tempo");
    kfp->set_double("System","tempo",tempo);


    char temp[2000];
    char temp2[200];
    char temp3[200];
    int n = kfp->get_integer("System","sequencers_number");
    for(int x = 0; x < n; x++){
        sprintf(temp,"Seq %d",x);
        kfp->set_boolean(temp,"expand_chord",1);
        kfp->set_integer(temp,"seq_type",SEQ_TYPE_NOTE);
        double length = kfp->get_double(temp,"length");
        kfp->remove_key(temp,"length");
        if(length == 0.125){
                kfp->set_integer(temp,"length_numerator",1);
                kfp->set_integer(temp,"length_denominator",8);
        }else if(length == 0.25){
                kfp->set_integer(temp,"length_numerator",1);
                kfp->set_integer(temp,"length_denominator",4);
        }else if(length == 0.5){
                kfp->set_integer(temp,"length_numerator",1);
                kfp->set_integer(temp,"length_denominator",2);
        }else if(length == 1.0){
                kfp->set_integer(temp,"length_numerator",1);
                kfp->set_integer(temp,"length_denominator",1);
        }else if(length == 2.0){
                kfp->set_integer(temp,"length_numerator",2);
                kfp->set_integer(temp,"length_denominator",1);
        }else if(length == 4.0){
                kfp->set_integer(temp,"length_numerator",4);
                kfp->set_integer(temp,"length_denominator",1);
        }else if(length == 8.0){
                kfp->set_integer(temp,"length_numerator",8);
                kfp->set_integer(temp,"length_denominator",1);
        }else{
                kfp->set_integer(temp,"length_numerator",1);
                kfp->set_integer(temp,"length_denominator",1);
        }
        int seq_n = kfp->get_integer(temp,"sequences_num");
        kfp->remove_key(temp,"sequences_num");
        kfp->set_integer(temp,"patterns_num",seq_n);
        kfp->set_integer(temp,"active_pattern",0);
        int res = kfp->get_integer(temp,"resolution");
        double note_len = 1.0/res;
        int vel = kfp->get_integer(temp,"volume");
        kfp->remove_key(temp,"volume");
        for(int w = 0; w < seq_n; w++){
            sprintf(temp2,"sequence_%d",w);
            sprintf(temp3,"pattern_%d",w);
            std::vector<int> A = kfp->get_integer_list(temp,temp2);
            kfp->remove_key(temp,temp2);
            std::vector<double> B;
            for (int t = 0; t<res;t++){
                for(int y = 0; y<6;y++){
                    if(A[t*6+y]){
                        B.push_back(t*note_len);
                        B.push_back(y);
                        B.push_back(vel);
                        B.push_back(note_len);
                    }
                }
            }
            kfp->set_double_list(temp,temp3,B);
        }

    }
    return 0;
}

bool ConvertFile_0_16_to_0_17(Glib::KeyFile* kfp){
    /* In 0.16, each sequencer carried a unique handle, which was
     * treated as it's ID. Actions that reference sequencers would
     * save that ID in the file, so that their reference can be
     * restored. In 0.17 this was changed, as sequencer reference is
     * now considered a special action's argument, and is no longer an
     * arbitrary integer. Action now store their sequencer reference
     * with an ID of the sequencer as used in the file. */

    /* For this reason, this conversion function needs to translate
     * these references from legacy handles to the new, simpler
     * format. */

    int seqnum = kfp->get_integer("System","sequencers_number");

    std::map<int, int> seq_handle_to_id;

    char temp[500];
    char temp2[500];

    /* Build the handle->id map. */
    for(int x = 0; x < seqnum; x++){
        sprintf(temp,"Seq %d",x);
        int h = kfp->get_integer(temp,"handle");
        seq_handle_to_id[h] = x;
    }

    int evnum = kfp->get_integer( "System", "events_number");
    for (int e = 0; e < evnum; e++) {
        sprintf(temp, "Event %d", e);
        int actions_num = kfp->get_integer(temp, "actions");
        for (int a = 0; a < actions_num; a++) {
            sprintf(temp2, "Action_%d_type", a);
            if (!kfp->has_key(temp, temp2)) {
                //there is no such action in file (was removed, was a NULL pointer while saving file) - skip to next action
                continue;
            }
            int type = kfp->get_integer(temp, temp2);
            /* Only process specific action types. */
            if(type == Action::SEQ_ON_OFF_TOGGLE ||
               type == Action::SEQ_CHANGE_ONE_NOTE ||
               type == Action::SEQ_CHANGE_CHORD ||
               type == Action::SEQ_PLAY_ONCE ||
               type == Action::SEQ_CHANGE_PATTERN ||
               type == Action::SEQ_TRANSPOSE_OCTAVE) {
                sprintf(temp2, "Action_%d_args", a);
                std::vector<int> args = kfp->get_integer_list(temp, temp2);
                int h = args[1];
                int id = seq_handle_to_id[h];
                sprintf(temp2,"Action_%d_seq",a);
                kfp->set_integer(temp, temp2, id);
            }
        }
    }

    return 0;
}

}//namespace files
