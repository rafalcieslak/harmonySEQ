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


#ifndef GLOBAL_H
#define	GLOBAL_H
#include <libintl.h>
#include <locale.h>
#include <vector>
#include <map>
#include <functional>
#include <glibmm/main.h>
#include <glibmm/ustring.h>
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

#include <boost/signals2.hpp>
namespace bs2 = boost::signals2;

// Build system config
#include "config.h"

#define ENDL "\n"
//Internal resolution of ALSA sequencer.
#define TICKS_PER_BEAT 192

// This matches MIDI standard.
#define MIDI_PPQ 24

// Macro for easier internalisation.
// Gettext was disabled during source code cleanup, as it's difficult
// to maintain, and we'd rather focus on other features.
#define _(STRING) (STRING)

#define DEFAULT_TEMPO 120.0

// Time (ms) how long does the tempo button blink on beat.
#define FLASH_INTERVAL 25

// Enables colorful indicator when event gets triggered (time in ms).
#define EVENTS_FLASH
#define EVENTS_FLASH_TIMEOUT 80

#define PLAY_ON_EDIT_MS 300

#define SEQUENCE_DEFAULT_SIZE 16
#define NOTES_CONST_SIZE 6
#define DEFAULT_VELOCITY 100

//the resolution of diode blink information
#define DIODES_RES 128

#define SEQUENCER_DEFAULT_NAME _("sequencer")


#ifndef I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
class Sequencer;
class debug;
class error;
class MidiDriver;
class MainWindow;
class Event;
class SettingsWindow;

extern int debugging;
extern debug* dbg;
extern error* err;

extern MidiDriver* midi;
extern SettingsWindow* settingswindow;

extern std::map<Glib::ustring, int> keymap_stoi;
extern std::map<int, Glib::ustring> keymap_itos;
extern std::map<int,Glib::ustring> notemap;

extern Glib::RefPtr< Gdk::Pixbuf > harmonySEQ_logo_48;
extern Glib::RefPtr< Gdk::Pixbuf > metronome_icon_24;
extern Glib::RefPtr< Gdk::Pixbuf > icon_add_note_seq;
extern Glib::RefPtr< Gdk::Pixbuf > icon_add_ctrl_seq;
#endif

/**Ask user a YES-NO question.*/
bool Ask(Glib::ustring message, Glib::ustring secondary_message = "");
/**Shows user a message.*/
void Info(Glib::ustring message, Glib::ustring secondary_message = "");

/** Return current wallclock timestamp as a double **/
double GetRealTime();

void DeferWorkToUIThread(std::function<void()> f);
void UIMain();

#endif	/* GLOBAL_H */
