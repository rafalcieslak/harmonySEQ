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

#ifndef CONFIG_H
#define	CONFIG_H

#include "global.h"

namespace Config{
    namespace Metronome{
       extern int Channel;
        extern int Hit1Note;
        extern int Hit2Note;
        extern int Hit1Velocity;
        extern int Hit2Velocity;
        extern bool Hit2;
    }
    namespace VisibleColumns{
        extern bool Channel;
        extern bool Pattern;
        extern bool Resolution;
        extern bool Length;
        extern bool ChordAndCtrlNo;
    }
    namespace Interaction{
        extern bool PlayOnEdit;
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

    /**Creates the config directory*/
    void MakeConfigDir();

    #define KF_GROUP_METRONOME "Metronome"
#define KF_METRONOME_KEY_CHANNEL "Channel"
#define KF_METRONOME_KEY_H1_NOTE "H1 note"
#define KF_METRONOME_KEY_H2_NOTE "H2 note"
#define KF_METRONOME_KEY_H1_VELOCITY "H1 velocity"
#define KF_METRONOME_KEY_H2_VELOCITY "H2 velocity"
#define KF_METRONOME_KEY_H2 "H2"
}


#endif	/* CONFIG_H */

