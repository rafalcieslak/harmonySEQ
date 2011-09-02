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


#ifndef FILES_H
#define	FILES_H

#include "global.h"

namespace Files{
    /**States whether the file was modified or not*/
    extern bool file_modified;
    /**Current file name*/
    extern Glib::ustring file_name;
    /**Current file directory*/
    extern Glib::ustring file_dir;

    /**Checks if a file exists.*/
    bool fexists(const char *filename);
    /**Sets file_modified flag, and refreshes the mainwindow title*/
    bool SetFileModified(bool modified);
    void FileModified();
    /**Saves a file. Does not show any dialog, does not check for overwriting. Just saves.*/
    void SaveToFile(Glib::ustring filename);
    /**Shows a file load dialog*/
    void LoadFileDialog();
    /**Loads a file, but without showing the dialog (so the filename must be passed as an argument).
        * Also, it converts it before opening, if it's an old file. */
    bool LoadFile(Glib::ustring file);
    
    /**Loads a file, assuming it's in current format*/
    bool LoadFileCurrent(Glib::KeyFile* kf);
    /**Converts file format from 0.15 to 0.16*/
    bool ConvertFile_0_15_to_0_16(Glib::KeyFile* kf);
}


#endif	/* FILES_H */

