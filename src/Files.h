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


#ifndef FILES_H
#define	FILES_H

#include "global.h"

namespace Files{
    extern bool file_modified;
    extern Glib::ustring file_name;

    /**Checks if a file exists.*/
    bool fexists(const char *filename);
    /**Sets file_modified flag, and refreshes the mainwindow title*/
    bool SetFileModified(bool modified);
    /**Saves a file. Does not show any dialog, does not check for overwriting. Just saves.*/
    void SaveToFile(Glib::ustring filename);
    /**Shows a file load dialog*/
    void LoadFileDialog();
    /**Loads a file, but without showing the dialog (so the filename must be passed as an argument)*/
    bool LoadFile(Glib::ustring file);
}


#endif	/* FILES_H */

