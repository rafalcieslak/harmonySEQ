/*
    Copyright (C) 2010-2012, 2020 Rafał Cieślak

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

#ifndef CONFIG_H
#define	CONFIG_H

#include <boost/signals2.hpp>
namespace bs2 = boost::signals2;

#include <glibmm/ustring.h>


namespace Config{
    namespace Metronome{
       extern int Channel;
        extern int Hit1Note;
        extern int Hit2Note;
        extern int Hit1Velocity;
        extern int Hit2Velocity;
        extern bool Hit2;
    }
    namespace Interaction{
        extern bool PlayOnEdit;
        extern int PatternRefreshMS;
        extern bool DisableDiodes;
    }
    namespace OSC{
        extern int Port;
    }
    /**Inits the configuration with default data.*/
    void LoadDefaultConfiguration();

    /**Loads the configuration from the config file*/
    void LoadFromFile();

    /**Stores the configuration in the config file*/
    void SaveToFile();

    /**Returns the path to config file, depending on the OS*/
    Glib::ustring GetConfigFilePath();

    /**Returns the path to config directory, depending on the OS*/
    Glib::ustring GetConfigDirPath();

    /**Returns the path to the old config file, depending on the OS*/
    Glib::ustring GetOldConfigFilePath();

    /**Returns the path to the old config directory, depending on the OS*/
    Glib::ustring GetOldConfigDirPath();

    /**Creates the config directory*/
    void MakeConfigDir();

/* This signal is normally emitted by the UI thread, but don't assume this is always the case. */
extern bs2::signal<void()> on_changed;

    #define KF_GROUP_METRONOME "Metronome"
#define KF_METRONOME_KEY_CHANNEL "Channel"
#define KF_METRONOME_KEY_H1_NOTE "H1 note"
#define KF_METRONOME_KEY_H2_NOTE "H2 note"
#define KF_METRONOME_KEY_H1_VELOCITY "H1 velocity"
#define KF_METRONOME_KEY_H2_VELOCITY "H2 velocity"
#define KF_METRONOME_KEY_H2 "H2"
}


#endif	/* CONFIG_H */
