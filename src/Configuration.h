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
   extern int MetronomeChannel;
    extern int MetronomeHit1Note;
    extern int MetronomeHit2Note;
    extern int MetronomeHit1Velocity;
    extern int MetronomeHit2Velocity;
    extern bool MetronomeHit2;

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
}


#endif	/* CONFIG_H */

