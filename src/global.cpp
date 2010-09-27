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


#include "global.h"
#include "MainWindow.h"
bool Ask(Glib::ustring message, Glib::ustring secondary_message, bool lock_threads){
    if (lock_threads) gdk_threads_enter();
    Gtk::MessageDialog dialog(*mainwindow,message,false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text(secondary_message);

    int result = dialog.run();
    if (lock_threads) gdk_threads_leave();

    switch (result){
        case Gtk::RESPONSE_YES:
            *dbg << "anserwed YES";
            return 1;
        case Gtk::RESPONSE_NO:
            *dbg << "anserwed NO";
            return 0;
    }

    return 0;
}
void Info(Glib::ustring message, Glib::ustring secondary_message, bool lock_threads){

    if (lock_threads) gdk_threads_enter();
    Gtk::MessageDialog dialog(*mainwindow,message,false,Gtk::MESSAGE_INFO,Gtk::BUTTONS_OK);
    dialog.set_secondary_text(secondary_message);

    dialog.run();
    if (lock_threads) gdk_threads_leave();

}