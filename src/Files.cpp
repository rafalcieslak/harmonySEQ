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
#include "Files.h"
#include "messages.h"
#include "Sequencer.h"
#include "MainWindow.h"
#include "Event.h"
#include "Action.h"
#include "MidiDriver.h"
#include "seqHandle.h"

namespace Files {

     bool file_modified;
    Glib::ustring file_name;
    Glib::ustring file_dir;
    
bool fexists(const char *filename)
{
   //Trick used to tell whether a file exists.
  ifstream ifile(filename);
  //Returns 1 if opening file succeeded, 0 elsewhere.
  return ifile;
}

bool SetFileModified(bool modified){
    if (modified != file_modified){
        file_modified = modified;
        mainwindow->UpdateTitle();
    }
    else
        file_modified = modified;

    return file_modified;
}

void SaveToFile(Glib::ustring filename){

    //The keyfile intepreter.
    Glib::KeyFile kf;

    //temporary strings
    char temp[300];
    char temp2[300];

    //The output file stream we'll put the data into.
    ofstream output_file;

    //Trying to open the file, using Truncate mode.
    output_file.open(filename.c_str(),ios_base::trunc);

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
    kf.set_double(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_TEMPO,tempo);

    kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_SEQ_NUM,seqVector.size());
    kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_EVENTS_NUM,Events.size());

    //And now, save the sequencers.
    //For each sequencer...
    for (unsigned int x = 0; x < seqVector.size(); x++){
        /* THIS HAS TO BE REWRITTEN!!
        if(seqVector[x] == NULL) continue; //Skip this sequencer, if it was removed.
        //Prepare the KEY for this sequencer, according to the pattern in FILE_GROUP_TEMPLATE_SEQ, it will be used to store all  values of this sequencer.
        sprintf(temp,FILE_GROUP_TEMPLATE_SEQ,x);
        //And store the values, as follows:
        kf.set_string(temp,FILE_KEY_SEQ_NAME,seqVector[x]->GetName());
        kf.set_integer(temp,FILE_KEY_SEQ_HANDLE,seqVector[x]->MyHandle);
        kf.set_boolean(temp,FILE_KEY_SEQ_ON,seqVector[x]->GetOn());
        kf.set_integer(temp,FILE_KEY_SEQ_CHANNEL,seqVector[x]->GetChannel());
        //This is depracated
        //kf.set_boolean(temp,FILE_KEY_SEQ_APPLY_MAIN_NOTE,sequencers[x]->GetApplyMainNote());
        kf.set_integer(temp,FILE_KEY_SEQ_VELOCITY,seqVector[x]->GetVelocity());
        kf.set_integer(temp,FILE_KEY_SEQ_RESOLUTION,seqVector[x]->resolution);
        kf.set_double(temp,FILE_KEY_SEQ_LENGTH,seqVector[x]->GetLength());
        kf.set_integer(temp,FILE_KEY_SEQ_PATTERNS_NUMBER,seqVector[x]->patterns.size());
        //Now, save the patterns.
        //For each pattern in this sequencer...
        for (int s=0; s<seqVector[x]->patterns.size();s++){
            //Prepatre the value name for this pattern, according to FILE_KEY_SEQ_PATTERN_TEMPLATE.
            sprintf(temp2,FILE_KEY_SEQ_PATTERN_TEMPLATE,s);
            //Convert sequencer notes of this pattern, which is a 2-dimentional array, into one dimentional vector
            vector<int> S(seqVector[x]->resolution*6);
            for (int r = 0; r < seqVector[x]->resolution; r++){
                for (int c = 0; c < 6; c++){
                    S[r*6+c]=seqVector[x]->GetPatternNote(s,r,c);
                }
            }
            //And store this vector in the keyfile as a list.
            kf.set_integer_list(temp,temp2,S);
        }
        //Finally, store the chord as a list of parameters.
        kf.set_integer_list(temp,FILE_KEY_SEQ_CHORD,seqVector[x]->chord.SaveToVector());
         * **/
    }

    //Then, save the events.
    //For each event...
    for (unsigned int x = 0; x < Events.size(); x++){
        if(Events[x] == NULL) continue; //If it was removed, skip to the next one.
        //Prepare the key, according to the pattern in FILE_GROUP_TEMPLATE_EVENT...
        sprintf(temp,FILE_GROUP_TEMPLATE_EVENT,x);
        //Store some basic data.
        kf.set_integer(temp,FILE_KEY_EVENT_TYPE,Events[x]->type);
        kf.set_integer(temp,FILE_KEY_EVENT_ARG1,Events[x]->arg1);
        kf.set_integer(temp,FILE_KEY_EVENT_ARG2,Events[x]->arg2);
        kf.set_integer(temp,FILE_KEY_EVENT_ACTIONS_NUM,Events[x]->actions.size());

        //Now save all actions of this event.
        //For each action of this event...
        for (unsigned int a = 0; a < Events[x]->actions.size(); a++){
            if (Events[x]->actions[a] == NULL) continue;//If the action was removed, skip to the next one.
            //Save actions's data.
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
            kf.set_integer(temp,temp2,Events[x]->actions[a]->type);
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARGS,a);
            kf.set_integer_list(temp,temp2,Events[x]->actions[a]->args);
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_CHORD,a);
            kf.set_integer_list(temp,temp2,Events[x]->actions[a]->chord.SaveToVector());
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
    //If file name has changed, we have to show it in the title of the main window.
    mainwindow->UpdateTitle();
    
    
    //File has no unsaved changes now, so...
    SetFileModified(0); 
}

void LoadFileDialog(){
    //Creating a new File Choosing Dialog
    Gtk::FileChooserDialog dialog(_("Choose a file to open..."),Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*mainwindow);
    //And adding to it some buttons.
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OPEN,Gtk::RESPONSE_OK);

    //Creating filters...
    Gtk::FileFilter hseq;
    hseq.set_name("HarmonySEQ files (*.hseq)");
    hseq.add_pattern("*.hseq");
    dialog.add_filter(hseq);
    Gtk::FileFilter all;
    all.set_name("All files");
    all.add_pattern("*");
    dialog.add_filter(all);

    //Running the dialog - passing the control to it, and waiting for the user to choose a file.
    int result = dialog.run();

    //Obtaining the file the user has chosen..
    Glib::ustring filename = dialog.get_filename();
    
    //Hiding the dialog...
    dialog.hide();
    
    //To avoid lag....
    bool was_paused = false;
    if (!midi->GetPaused()){
        midi->PauseQueueImmediately();
        was_paused = true;
    }
    //And depending on which button did the user choose
    switch (result){
        case Gtk::RESPONSE_OK:
            //User clicked OK. Calling other procedure, that will load the file.
            LoadFile(filename);
            SetFileModified(0);
            break;
        case Gtk::RESPONSE_CANCEL:
            //User clicked CANCEL. Do nothing.
            if (was_paused) midi->ContinueQueue();
            return;
            break;
        default:
            //User uses cheat codes.
            *dbg << "unknown response returned!\n";
            if (was_paused) midi->ContinueQueue();
            return;
        break;
    }

    //Some things that must be done to update the GUI fully.
    mainwindow->InitTreeData();
    mainwindow->tempo_button.set_value(tempo);
    mainwindow->UpdateEventWidget();

    if (was_paused) midi->ContinueQueue();
}


bool LoadFile(Glib::ustring file){


    *dbg << "trying to open |" << file <<"|--\n";
    Glib::KeyFile kf;
    char temp[3000];
    char temp2[1000];


    //We'll try to open a file. No i/o streams needed here, KeyFile provides us with a useful method load_from_file, which does all the magic.
    try{
        if (!kf.load_from_file(file)) {
            //Returned 1, so something strange is with the file.
            sprintf(temp, _("ERROR - error while trying to read file '%s'\n"), file.c_str());
            *err << temp;
            Info(temp);
            return 1;
        }
    }catch(Glib::Error e){
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
        //Compaing versions...
        if (VA > VERSION_A || (VA == VERSION_A && VB > VERSION_B) || (VA == VERSION_A && VB == VERSION_B && VC > VERSION_C)){
            //File is too new
            sprintf(temp,_("This file was created by harmonySEQ in a newer version (%d.%d.%d). This means it may contain data that is suppored by the newer wersion, but not by the version you are using (%d.%d.%d). It is recommended not to open such file, since it is very likely it may produce strange errors, or may event crash the program unexpectedly. However, in some cases one may want to open such file anyway, for example if it is sure it will open without trouble. Select YES to do so."),VA,VB,VC,VERSION_A,VERSION_B,VERSION_C);
            if (Ask(_("Do you want to open this file?"),temp)){
                //anserwed YES;
            }else{
                return 1;
            }

        } else if (VA < VERSION_A || (VA == VERSION_A && VB < VERSION_B) || (VA == VERSION_A && VB == VERSION_B && VC < VERSION_C)){
            //File is too old
            sprintf(temp,_("This file was created by harmonySEQ in an older version (%d.%d.%d). This means it may miss some data that is required to be in file by the version you are using (%d.%d.%d). It is recommended not to open such file, since it is very likely it may produce strange errors, or may event crash the program unexpectedly. However, in some cases one may want to open such file anyway, for example if it is sure it will open without trouble. Select YES to do so."),VA,VB,VC,VERSION_A,VERSION_B,VERSION_C);
            if (Ask(_("Do you want to open this file?"),temp)){
                //anserwed YES;
            }else{
                return 1;
            }
        }

        /**Error flag.*/
        int e_flag = 0;

        if(VA == 0 && VB == 15){
            e_flag = LoadFile015(&kf);
        }else if(VA == 0 && VB < 15 ){
            e_flag = LoadFilePre015(&kf);
        }else{
            //oh god! dunno what to do, file is in unknown version, but the user wants to open it anyway!
            //Lets try using the newest routine, and hope it works...
            e_flag = LoadFile015(&kf);
        }

        //In case of errors...
        if (e_flag) {
            sprintf(temp, _("ERROR - error while reading file '%s'\n"), file.c_str());
            *err << temp;
            Info(temp);
            return 1;
        }

        //To make sure all goes well:
        midi->Sync();
        
        //At beggining, file is unmodidied.
        SetFileModified(0);

        //Looking for the last '/' or '\' in the file patch, and storing file name and patch in appropriate variable.
        int found =  file.find_last_of("/\\");  //Will work on linux and windows both
        file_name = file.substr(found+1);
        file_dir = file.substr(0,found+1);
        
         //If file name has changed, we have to show it in the title of the main window.
        mainwindow->UpdateTitle();



    //Only exception handles are left...
    }catch(Glib::KeyFileError e){
        //KeyFile error means some trouble with data in the file. Missing key, wrong format, wrong characters, all these goes here.
        sprintf(temp, _("ERROR - Glib::KeyFile error while processing file '%s': "), file.c_str());
        *err << temp;
        *err << e.what();
        *err << ENDL;
        Info(temp,e.what());
        return 1;
    }catch(Glib::Error e){
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

//===============FILE LOADING ROUTINES GO HERE====================

bool LoadFile015(Glib::KeyFile* kfp){

    int seqNum;
    char temp[3000];
    char temp2[1000];

    //Read some basic data...
    tempo = kfp->get_double(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_TEMPO);
    seqNum = kfp->get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_SEQ_NUM);

    //Get rid of any seqeuncers.
    ClearSequencers(); //woa hua hua hua!
    ResetSeqHandles();
    //Needed to determine the next free handle...
    int maximal_handle = 0;
    //Now we'll process all sequencers that are in the file.
    for (int x = 0; x < seqNum; x++) {
        //First prepare the key.
        sprintf(temp, FILE_GROUP_TEMPLATE_SEQ, x);

        //In case there is no such sequencer...
        if (!kfp->has_group(temp)) {
                sprintf(temp, _("ERROR - Missing sequencer %d in file\n"), x);
                *err << temp;
                return 1;
        }
        /*THIS HAS TO BE REWRITTEN
        //If we got here, this means this sequencer was NOT removed. So: let's create it.
        seqVector.push_back(new Sequencer());

        seqHandle h = kfp->get_integer(temp,FILE_KEY_SEQ_HANDLE);
        seqVector[x]->MyHandle = h;
        if(h > maximal_handle) maximal_handle = h;
        AddCustomSeqHandle(h,x);
        //Put some data into it...
        seqVector[x]->SetName(kfp->get_string(temp, FILE_KEY_SEQ_NAME));
        seqVector[x]->SetOn(kfp->get_boolean(temp, FILE_KEY_SEQ_ON));
        seqVector[x]->SetChannel(kfp->get_integer(temp, FILE_KEY_SEQ_CHANNEL));
        seqVector[x]->resolution = kfp->get_integer(temp, FILE_KEY_SEQ_RESOLUTION);
        seqVector[x]->SetLength(kfp->get_double(temp, FILE_KEY_SEQ_LENGTH));
        seqVector[x]->SetVelocity(kfp->get_integer(temp, FILE_KEY_SEQ_VELOCITY));


        seqVector[x]->patterns.clear();

        //Now, load the patterns.
        //Number of pattrens -> n
        int n = kfp->get_integer(temp,FILE_KEY_SEQ_PATTERNS_NUMBER);
        //For each pattern we load...
        for(int s =0; s < n; s++){
            //First add an empty pattern.
            seqVector[x]->AddPattern();
            //Prepare the value name...
            sprintf(temp2,FILE_KEY_SEQ_PATTERN_TEMPLATE,s);
            //And get the pattern from file
            std::vector<int> pattern = kfp->get_integer_list(temp, temp2);

            //Simple algorithm, just copying data from the pattern from file to the pattern in the sequencer
            for (unsigned int n = 0; n < pattern .size(); n++) {
                for(int r = 0; r < seqVector[x]->resolution; r++){
                    for(int c = 0; c < 6; c++){
                            seqVector[x]->SetPatternNote(s,r,c,pattern [r*6+c]);
                    }
                 }
             }

         }//next pattern

        //Just to make sure, check if the sequencer we've just loaded from file has any patterns...
        if(seqVector[x]->patterns.size() == 0)
            //wtf, there were no sequences in the file? strange. We have to create one in order to prevent crashes. What would be a sequencer with no patterns? At least that's something we better aviod.
            seqVector[x]->AddPattern();
        

        //Here we load the chord (if any)
        if (kfp->has_key(temp,FILE_KEY_SEQ_CHORD)){
            std::vector<int> vec =   kfp->get_integer_list(temp,FILE_KEY_SEQ_CHORD);
            seqVector[x]->chord.SetFromVector(vec);
        }
         * **/
        //Now proceed to the...
    }  //...next sequencer.

    ManuallySetSeqHandleCounter(maximal_handle+1);
    
    //Done loading sequencers.

    //Now, proceed to events.

    //No such key? something really wrong. There should be an error message, TODO.
    if(!kfp->has_key(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_EVENTS_NUM)) { *err << "ERROR - No EventsNumber key in file.\n"; return 1;}

    //Number of events.
    seqNum = kfp->get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_EVENTS_NUM);

    //Get rid of any events.
    ClearEvents();

    //For each event in file...
    for (int x = 0; x < seqNum; x++){

        sprintf(temp, FILE_GROUP_TEMPLATE_EVENT, x);
        //If there is no key related to this number of event, this means this event was removed.
        if (!kfp->has_group(temp)) {
            //So we'll put instead a null pointer, so it'll look as a removed one, and skip to look for the next event.
            Events.push_back(NULL);
            continue;
        }
        //First, create a new event...
        Events.push_back(new Event());
        //Put some data into it...
        Events[x]->type = kfp->get_integer(temp,FILE_KEY_EVENT_TYPE);
        Events[x]->arg1 = kfp->get_integer(temp,FILE_KEY_EVENT_ARG1);
        Events[x]->arg2 = kfp->get_integer(temp,FILE_KEY_EVENT_ARG2);
        int actions_num = kfp->get_integer(temp,FILE_KEY_EVENT_ACTIONS_NUM);
        //For each action of this event...
        for (int a = 0; a < actions_num; a++){
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
            if (!kfp->has_key(temp,temp2)){
                //there is no such action in file (was removed, was a NULL pointer while saving file) - skip to next action
                Events[x]->actions.push_back(NULL);
                continue;
            }
            //Create a new action
            Events[x]->actions.push_back(new Action(Action::NONE));

            //Fill it with data: type...
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
            Events[x]->actions[a]->type = kfp->get_integer(temp,temp2);
            //...arguments...
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARGS,a);
            Events[x]->actions[a]->args = kfp->get_integer_list(temp,temp2);

            //...and a chord, if any.
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_CHORD,a);
            if (kfp->has_key(temp,temp2)){
                vector<int> vec = kfp->get_integer_list(temp,temp2);
                Events[x]->actions[a]->chord.SetFromVector(vec);
             }
            //Update the chord GUI.
            Events[x]->actions[a]->GUIUpdateChordwidget();
        }//next action.

        //Update this event's GUI, using newly loaded data.
        Events[x]->UpdateGUI();

    }//next event.





     //And that's all, file is loaded.
    return 0;

}

bool LoadFilePre015(Glib::KeyFile* kfp){

    int number;
    char temp[3000];
    char temp2[1000];

    //At the very beggining we should check the version of the file.
    int VA = kfp->get_integer("harmonySEQ","versionA");
    int VB = kfp->get_integer("harmonySEQ","versionB");
    int VC = kfp->get_integer("harmonySEQ","versionC");
    //Slider-compatible mode is switched on, when the file we open does not support polyphony  (version 0.12 or earlier).
    //In this case we must translate monophonic data, to polyphonic.
    int slider_compatible_mode = 0;
    int chord_compatible_mode = 0;
    if (VA == 0 && VB <= 12) slider_compatible_mode = 1; //That's the case, when monophonic data will be converted to polyphonic.
    if(VA == 0 && VB <= 13) chord_compatible_mode = 1; //Chord is stored in the old format.

    //Read some basic data...
    tempo = kfp->get_double(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_TEMPO);
    int mainnote, use_main_note;
    if (chord_compatible_mode) mainnote= kfp->get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_MAINNOTE);
    number = kfp->get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_SEQ_NUM);

    //Get rid of any seqeuncers.
    ClearSequencers(); //woa hua hua hua!
    ResetSeqHandles();

    //The sequener ID might be shifted, as the old files were full of removed empty sequencers.
    std::map<int, int> seq_unstretching_map;
    //Now we'll process all sequencers that are in the file.
    for (int x = 0; x < number; x++) {
        //First prepare the key.
        sprintf(temp, FILE_GROUP_TEMPLATE_SEQ, x);
        //If there is no such key in the file, that means this sequencer was skipped while saving.
        if (!kfp->has_group(temp)) {
            //We'll suplement it with an empty pointer, so it'll look like like a removed sequencer, and skip to look for the next one.
            seqVector.push_back(NULL);
            continue;
        }
        /*THIS HAS TO BE REWRITTEN
        //If we got here, this means this sequencer was NOT removed. So: let's create it.
        seqVector.push_back(new Sequencer());
        //calculate shift...
        seq_unstretching_map[x] = seqVector.size()-1;
        
        seqHandle h = RequestNewSeqHandle(x);
        seqV(x)->MyHandle = h;
        //Put some data into it...
        seqVector[x]->SetName(kfp->get_string(temp, FILE_KEY_SEQ_NAME));
        seqVector[x]->SetOn(kfp->get_boolean(temp, FILE_KEY_SEQ_ON));
        seqVector[x]->SetChannel(kfp->get_integer(temp, FILE_KEY_SEQ_CHANNEL));
        if (chord_compatible_mode) use_main_note = kfp->get_boolean(temp, FILE_KEY_SEQ_APPLY_MAIN_NOTE);
        seqVector[x]->resolution = kfp->get_integer(temp, FILE_KEY_SEQ_RESOLUTION);
        seqVector[x]->SetLength(kfp->get_double(temp, FILE_KEY_SEQ_LENGTH));

        //Check whether velocity is saved in file.
        if(kfp->has_key(temp,FILE_KEY_SEQ_VELOCITY))
            seqVector[x]->SetVelocity(kfp->get_integer(temp, FILE_KEY_SEQ_VELOCITY));
        //Because if it's not...
        else
            //...we need to set it to a default value.
            seqVector[x]->SetVelocity(DEFAULT_VELOCITY);
        seqVector[x]->patterns.clear();

        //Now, load the patterns.
        if(kfp->has_key(temp,FILE_KEY_SEQ_SEQUENCE)){
                //VEEEERY old file, seems it uses only one pattern, this case may be abandoned in future, since noone uses soooo old files (not sure, but probably it's 0.10 or earlier)
                seqVector[x]->AddPattern();
                std::vector<int> sequence = kfp->get_integer_list(temp, FILE_KEY_SEQ_SEQUENCE);
                    for(int r = 0; r < seqVector[x]->resolution; r++){
                        for(int c = 0; c < 6; c++){
                            if (c == sequence[r])
                                seqVector[x]->SetPatternNote(0,r,c,1);
                            else
                                seqVector[x]->SetPatternNote(0,r,c,0);

                        }
                }
        }else{
            //Normal case: there are many patterns.
            //Number of pattrens -> n
            int n = kfp->get_integer(temp,FILE_KEY_SEQ_PATTERNS_NUMBER);
            //For each pattern we load...
            for(int s =0; s < n; s++){
                //First add an empty pattern.
                seqVector[x]->AddPattern();
                //Prepare the value name...
                sprintf(temp2,FILE_KEY_SEQ_PATTERN_TEMPLATE,s);
                //And get the pattern from file
                std::vector<int> pattern = kfp->get_integer_list(temp, temp2);

                //Check for the old-file case...
                if (slider_compatible_mode){
                    //used to load old files <=0.12.0
                    for(int r = 0; r < seqVector[x]->resolution; r++){
                        for(int c = 0; c < 6; c++){
                            //(If this is the note, that is chosen in file...)
                            if (c == pattern [r])
                                //mark it as ON
                                seqVector[x]->SetPatternNote(s,r,c,1);
                            else
                                //mark it as OFF
                                seqVector[x]->SetPatternNote(s,r,c,0);
                        }
                    }
                }else{
                    //used to load new files >=0.13.0
                    //Simple algorithm, just copying data from the pattern from file to the pattern in the sequencer
                    for (unsigned int n = 0; n < pattern .size(); n++) {
                    for(int r = 0; r < seqVector[x]->resolution; r++){
                        for(int c = 0; c < 6; c++){
                                seqVector[x]->SetPatternNote(s,r,c,pattern [r*6+c]);
                        }
                    }
                     }
                }

            }//next pattern

            //Just to make sure, check if the sequencer we've just loaded from file has any patterns...
            if(seqVector[x]->patterns.size() == 0)
                //wtf, there were no sequences in the file? strange. We have to create one in order to prevent crashes. What would be a sequencer with no patterns? At least that's something we beeter aviod.
                seqVector[x]->AddPattern();
        }

        //Here we load the chord (if any)
        if (kfp->has_key(temp,FILE_KEY_SEQ_CHORD)){
            std::vector<int> vec =   kfp->get_integer_list(temp,FILE_KEY_SEQ_CHORD);
            if (!chord_compatible_mode){
                seqVector[x]->chord.SetFromVector(vec);
            }else{ //old file
                seqVector[x]->chord.SetBaseUse(use_main_note);
                seqVector[x]->chord.SetBase(mainnote);
                seqVector[x]->chord.SetFromVector_OLD_FILE_PRE_0_14(vec);
            }
        }

        //Now proceed to the...
         * **/
    }  //...next sequencer.

    //Done loading sequencers.

    //Now, proceed to events.

    //No such key? something really wrong. There should be an error message, TODO.
    if(!kfp->has_key(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_EVENTS_NUM)) return 1;

    //Number of events.
    number = kfp->get_integer(FILE_GROUP_SYSTEM, FILE_KEY_SYSTEM_EVENTS_NUM);

    //Get rid of any events.
    ClearEvents();

    //For each event in file...
    for (int x = 0; x < number; x++){

        sprintf(temp, FILE_GROUP_TEMPLATE_EVENT, x);
        //If there is no key related to this number of event, this means this event was removed.
        if (!kfp->has_group(temp)) {
            //So we'll put instead a null pointer, so it'll look as a removed one, and skip to look for the next event.
            Events.push_back(NULL);
            continue;
        }
        //First, create a new event...
        Events.push_back(new Event());
        //Put some data into it...
        Events[x]->type = kfp->get_integer(temp,FILE_KEY_EVENT_TYPE);
        Events[x]->arg1 = kfp->get_integer(temp,FILE_KEY_EVENT_ARG1);
        Events[x]->arg2 = kfp->get_integer(temp,FILE_KEY_EVENT_ARG2);
        int actions_num = kfp->get_integer(temp,FILE_KEY_EVENT_ACTIONS_NUM);
        //For each action of this event...
        for (int a = 0; a < actions_num; a++){
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
            if (!kfp->has_key(temp,temp2)){
                //there is no such action in file (was removed, was a NULL pointer while saving file) - skip to next action
                Events[x]->actions.push_back(NULL);
                continue;
            }
            //Create a new action
            Events[x]->actions.push_back(new Action(Action::NONE));

            //Fill it with data: type...
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE,a);
            Events[x]->actions[a]->type = kfp->get_integer(temp,temp2);
            //...arguments...
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_ARGS,a);
            Events[x]->actions[a]->args = kfp->get_integer_list(temp,temp2);

            //NOW WARNING! Some actions use ARG1 to store sequencer handle. However, older files store the ID and not the handle.
            //So, in case the action is one of that type...
            if (Events[x]->actions[a]->type == Action::SEQ_CHANGE_CHORD || Events[x]->actions[a]->type == Action::SEQ_CHANGE_ONE_NOTE || Events[x]->actions[a]->type == Action::SEQ_CHANGE_PATTERN||
                    Events[x]->actions[a]->type == Action::SEQ_ON_OFF_TOGGLE || Events[x]->actions[a]->type == Action::SEQ_PLAY_ONCE)
                //substitute the ID with the handle
            {
                Events[x]->actions[a]->args[1] = seqV(seq_unstretching_map[Events[x]->actions[a]->args[1]])->MyHandle;
            }
            //...and a chord, if any.
            sprintf(temp2,FILE_GROUP_TEMPLATE_EVENT_ACTION_CHORD,a);
            if (kfp->has_key(temp,temp2)){
                vector<int> vec = kfp->get_integer_list(temp,temp2);
                    if (!chord_compatible_mode) {
                        Events[x]->actions[a]->chord.SetFromVector(vec);
                    } else { //old file
                        Events[x]->actions[a]->chord.SetBaseUse(1);
                        Events[x]->actions[a]->chord.SetBase(mainnote);
                        Events[x]->actions[a]->chord.SetFromVector_OLD_FILE_PRE_0_14(vec);
                    }
             }
            //Update the chord GUI.
            Events[x]->actions[a]->GUIUpdateChordwidget();
        }//next action.

        //Update this event's GUI, using newly loaded data.
        Events[x]->UpdateGUI();

    }//next event.




     //And that's all, file is loaded.
    return 0;
}


}//namespace files
