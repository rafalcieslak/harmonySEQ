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

#include <glibmm-2.4/glibmm/keyfile.h>
#include <fstream>

#include "Configuration.h"
#include "messages.h"

#ifdef __linux__
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#endif

namespace Config{
    int MetronomeChannel;
    int MetronomeHit1Note;
    int MetronomeHit2Note;
    int MetronomeHit1Velocity;
    int MetronomeHit2Velocity;

    void LoadDefaultConfiguration(){
        MetronomeChannel = 10;
        MetronomeHit1Note = 76;
        MetronomeHit2Note = 77;
        MetronomeHit1Velocity = 100;
        MetronomeHit2Velocity = 60;
    }

    void LoadFromFile(){
        *dbg << "loading config from file..." << ENDL;
        Glib::KeyFile kf;
        char temp[200];

        Glib::ustring path = GetConfigFilePath();

        //We'll try to open a file. No i/o streams needed here, KeyFile provides us with a useful method load_from_file, which does all the magic.
        try {
            if (!kf.load_from_file(path)) {
                //Returned 1, so confing file is wrong. So we'll not load it.
                return;
            }
        } catch (Glib::Error e) {
            //Exception cought. Leave the file, maybe it doesn't exist etc.
            return;
        }

        //oh, the file is all right. Load the data then.
        if(kf.has_group(CONFIG_FILE_GROUP_METRONOME)){
           if(kf.has_key(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_CHANNEL)) MetronomeChannel = kf.get_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_CHANNEL);
           if(kf.has_key(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H1_NOTE)) MetronomeHit1Note = kf.get_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H1_NOTE);
           if(kf.has_key(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H2_NOTE)) MetronomeHit2Note = kf.get_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H2_NOTE);
           if(kf.has_key(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H1_VELOCITY)) MetronomeHit1Velocity = kf.get_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H1_VELOCITY);
           if(kf.has_key(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H2_VELOCITY)) MetronomeHit2Velocity = kf.get_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H2_VELOCITY);
        }

    }

    void SaveToFile(){
        *dbg << "saving to config file..." << ENDL;
        Glib::KeyFile kf;
        char temp[200];
        std::ofstream output_file;

        Glib::ustring path = GetConfigFilePath();

        MakeConfigDir();//in case it doesnt exist
        //Trying to open the file, using Truncate mode.
        output_file.open(path.c_str(),std::ios_base::trunc);

        //If something went wrong...
        if (!output_file.good()) {
            sprintf(temp, _("ERROR - error while trying to save config file %s.\n(errno = %d)"),path.c_str(),errno);
            //Inform the user about the errror both by STDIO and a nice graphical MessageBox.
            *err << temp;
            Info(temp);
            return;
        }

        kf.set_comment("This is harmonySEQ's config file. Manual editing is not recommended, as may result in strange crashes.");
        kf.set_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_CHANNEL,MetronomeChannel);
        kf.set_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H1_NOTE,MetronomeHit1Note);
        kf.set_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H2_NOTE,MetronomeHit2Note);
        kf.set_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H1_VELOCITY,MetronomeHit1Velocity);
        kf.set_integer(CONFIG_FILE_GROUP_METRONOME,CONFIG_FILE_METRONOME_KEY_H2_VELOCITY,MetronomeHit2Velocity);

        output_file <<  kf.to_data().c_str();

        output_file.close();
        *dbg << "saving to config file done." << ENDL;
    }


    Glib::ustring GetConfigFilePath(){
        char temp[200];

        //Obtaining home directory path. This is PLATFORM-SPECYFIC!
#ifdef __linux__
        struct passwd *pw = getpwuid(getuid());
        sprintf(temp, "%s/.harmonySEQ/harmonySEQ.conf",pw->pw_dir);
        return temp;
#endif

    }


    Glib::ustring GetConfigDirPath(){
        char temp[200];

        //Obtaining home directory path. This is PLATFORM-SPECYFIC!
#ifdef __linux__
        struct passwd *pw = getpwuid(getuid());
        sprintf(temp, "%s/.harmonySEQ", pw->pw_dir);
        return temp;
#endif

    }
    
    void MakeConfigDir(){
        mkdir(GetConfigDirPath().c_str(),0755); //chmod 0755 = owner can read write and execute, others cannot write but can read & execute
        if (errno == 17) {
            //everything all right, the directory exists
        }
        
    }

}

