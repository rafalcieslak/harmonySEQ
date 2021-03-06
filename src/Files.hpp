/*
    Copyright (C) 2010-2011, 2020 Rafał Cieślak

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

#include <glibmm/ustring.h>
#include <glibmm/keyfile.h>

#include <boost/signals2.hpp>
namespace bs2 = boost::signals2;


namespace Gtk {class Window;}


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
    void SaveToFile(Glib::ustring filename, Gtk::Window* parent);
    /**Shows a file load dialog*/
    void LoadFileDialog(Gtk::Window* parent);
    /**Shows a file save dialog*/
    void SaveFileDialog(Gtk::Window* parent);
    /**Loads a file, but without showing the dialog (so the filename must be passed as an argument).
        * Also, it converts it before opening, if it's an old file. */
    bool LoadFile(Glib::ustring file, Gtk::Window* parent);

    /**Loads a file, assuming it's in current format*/
    bool LoadFileCurrent(Glib::KeyFile* kf);
    /**Converts file format from 0.15 to 0.16*/
    bool ConvertFile_0_15_to_0_16(Glib::KeyFile* kf);
    /**Converts file format from 0.16 to 0.17*/
    bool ConvertFile_0_16_to_0_17(Glib::KeyFile* kf);

extern bs2::signal<void()> on_file_loaded;
extern bs2::signal<void()> on_file_saved;
extern bs2::signal<void()> on_file_modified;

}


#endif	/* FILES_H */
