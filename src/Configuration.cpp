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
    namespace Metronome{
        int Channel;
        int Hit1Note;
        int Hit2Note;
        int Hit1Velocity;
        int Hit2Velocity;
        bool Hit2;
    }

    namespace VisibleColumns {
        bool Channel;
        bool Pattern;
        bool Resolution;
        bool Length;
        bool ChordAndCtrlNo;
    }
    namespace Interaction{
        bool PlayOnEdit;
        int PatternRefreshMS;
    }
    namespace OSC{
        int Port;
    }
    
    void LoadDefaultConfiguration(){
        //Default values
        Metronome::Channel = 10;
        Metronome::Hit1Note = 76;
        Metronome::Hit2Note = 77;
        Metronome::Hit1Velocity = 100;
        Metronome::Hit2Velocity = 60;
        Metronome::Hit2 = true;
        VisibleColumns::Channel = 1;
        VisibleColumns::Pattern = 1;
        VisibleColumns::Resolution = 1;
        VisibleColumns::Length = 1;
        VisibleColumns::ChordAndCtrlNo = 1;
        Interaction::PlayOnEdit = 1;
        Interaction::PatternRefreshMS = 40;
        OSC::Port = 7773;
    }

    void LoadFromFile(){
        *dbg << "loading config from file..." << ENDL;
        Glib::KeyFile kf;

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
        if(kf.has_group(KF_GROUP_METRONOME)){
           if(kf.has_key(KF_GROUP_METRONOME,KF_METRONOME_KEY_CHANNEL)) Metronome::Channel = kf.get_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_CHANNEL);
           if(kf.has_key(KF_GROUP_METRONOME,KF_METRONOME_KEY_H1_NOTE)) Metronome::Hit1Note = kf.get_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H1_NOTE);
           if(kf.has_key(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2_NOTE)) Metronome::Hit2Note = kf.get_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2_NOTE);
           if(kf.has_key(KF_GROUP_METRONOME,KF_METRONOME_KEY_H1_VELOCITY)) Metronome::Hit1Velocity = kf.get_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H1_VELOCITY);
           if(kf.has_key(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2_VELOCITY)) Metronome::Hit2Velocity = kf.get_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2_VELOCITY);
           if(kf.has_key(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2)) Metronome::Hit2 = kf.get_boolean(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2);
        }
        if(kf.has_group("Visible columns")){
            if(kf.has_key("Visible columns","Channel")) VisibleColumns::Channel = kf.get_boolean("Visible columns","Channel");
            if(kf.has_key("Visible columns","Pattern")) VisibleColumns::Pattern = kf.get_boolean("Visible columns","Pattern");
            if(kf.has_key("Visible columns","Resolution")) VisibleColumns::Resolution = kf.get_boolean("Visible columns","Resolution");
            if(kf.has_key("Visible columns","Length")) VisibleColumns::Length = kf.get_boolean("Visible columns","Length");
            if(kf.has_key("Visible columns","Chord")) VisibleColumns::ChordAndCtrlNo = kf.get_boolean("Visible columns","Chord");
        }
        if(kf.has_group("Interaction")){
            if(kf.has_key("Interaction","PlayOnEdit")) Interaction::PlayOnEdit = kf.get_boolean("Interaction","PlayOnEdit");
            if(kf.has_key("Interaction","PatternRefreshMS")) Interaction::PatternRefreshMS = kf.get_integer("Interaction","PatternRefreshMS");
        }
        if(kf.has_group("OSC")){
            if(kf.has_key("OSC","Port")) OSC::Port = kf.get_integer("OSC","Port");
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

        //Storing data
        kf.set_comment("This is harmonySEQ's config file. Manual editing is not recommended, as may result in strange crashes.");
        kf.set_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_CHANNEL,Metronome::Channel);
        kf.set_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H1_NOTE,Metronome::Hit1Note);
        kf.set_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2_NOTE,Metronome::Hit2Note);
        kf.set_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H1_VELOCITY,Metronome::Hit1Velocity);
        kf.set_integer(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2_VELOCITY,Metronome::Hit2Velocity);
        kf.set_boolean(KF_GROUP_METRONOME,KF_METRONOME_KEY_H2,Metronome::Hit2);
        kf.set_boolean("Visible columns","Channel",VisibleColumns::Channel);
        kf.set_boolean("Visible columns","Pattern",VisibleColumns::Pattern);
        kf.set_boolean("Visible columns","Resolution",VisibleColumns::Resolution);
        kf.set_boolean("Visible columns","Length",VisibleColumns::Length);
        kf.set_boolean("Visible columns","Chord",VisibleColumns::ChordAndCtrlNo);
        kf.set_boolean("Interaction","PlayOnEdit",Interaction::PlayOnEdit);
        kf.set_integer("Interaction","PatternRefreshMS",Interaction::PatternRefreshMS);
        kf.set_integer("OSC","Port",OSC::Port);

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

