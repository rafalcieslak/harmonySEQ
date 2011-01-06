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


#ifndef GLOBAL_H
#define	GLOBAL_H
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <libintl.h>
#include <locale.h>
#include <vector>
#include <map>
#include <glibmm/ustring.h>
#include "gettext.h"

#define VERSION_STRING "0.14"//VERSION is provided by autotools in config.h
#define VERSION_A 0
#define VERSION_B 14
#define VERSION_C 0

#define ENDL "\n"
//Internal resolution of ALSA sequencer.
#define TICKS_PER_QUARTERNOTE 128
#define TICKS_PER_NOTE (4*TICKS_PER_QUARTERNOTE)
//Makro for easier internalisation.
#define _(STRING) gettext(STRING)

#define DEFAULT_TEMPO 240.0

//How long is the tempo indicatior RED.
#define FLASH_INTERVAL 50
    //in ms

//enables colorful indicator when event gets triggered
#define EVENTS_FLASH
#define EVENTS_FLASH_TIMEOUT 200
    //in ms

#define SEQUENCE_DEFAULT_SIZE 8
#define NOTES_CONST_SIZE 6
#define DEFAULT_VOLUME 100

//the following may not work with gettext. careful!
#define SEQUENCER_DEFAULT_NAME _("sequencer")

#define RESOLUTIONS_NUM 7
#define RESOLUTIONS {1,2,4,8,16,32,64}
#define LENGTHS_NUM 7
#define LENGTHS {0.125,0.25,0.5,1.0,2.0,4.0,8.0}

#define FILE_GROUP_SYSTEM "System"
#define FILE_KEY_SYSTEM_TEMPO "tempo"
#define FILE_KEY_SYSTEM_MAINNOTE "mainnote"
#define FILE_KEY_SYSTEM_SEQ_NUM "sequencers_number"
#define FILE_KEY_SYSTEM_EVENTS_NUM "events_number"

#define FILE_GROUP_TEMPLATE_SEQ "Seq %d"
#define FILE_KEY_SEQ_RESOLUTION "resolution"
#define FILE_KEY_SEQ_LENGTH "length"
#define FILE_KEY_SEQ_ON "on"
#define FILE_KEY_SEQ_APPLY_MAIN_NOTE "apply_mn"
#define FILE_KEY_SEQ_CHANNEL "channel"
#define FILE_KEY_SEQ_VOLUME "volume"
#define FILE_KEY_SEQ_NAME "name"
#define FILE_KEY_SEQ_SEQUENCE "sequence"
#define FILE_KEY_SEQ_PATTERNS_NUMBER "sequences_num"
#define FILE_KEY_SEQ_ACTIVE_SEQUENCE "active_sequence"
#define FILE_KEY_SEQ_PATTERN_TEMPLATE "sequence_%d" //God, it should be "pattern_%d". Too late to change, older files use "sequence".  Maybe in future a workaround will be found.
#define FILE_KEY_SEQ_CHORD "chord"

#define FILE_GROUP_TEMPLATE_EVENT "Event %d"
#define FILE_KEY_EVENT_TYPE "type"
#define FILE_KEY_EVENT_ARG1 "arg1"
#define FILE_KEY_EVENT_ARG2 "arg2"
#define FILE_KEY_EVENT_ACTIONS_NUM "actions"

#define FILE_GROUP_TEMPLATE_EVENT_ACTION_TYPE "Action_%d_type"
#define FILE_GROUP_TEMPLATE_EVENT_ACTION_ARGS "Action_%d_args"
#define FILE_GROUP_TEMPLATE_EVENT_ACTION_CHORD "Action_%d_chord"


#ifndef I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
class Sequencer;
class debug;
class error;
class MidiDriver;
class MainWindow;
class Event;
class EventsWindow;
extern std::vector<Sequencer *> sequencers;
extern double tempo;
extern int ports_number;
extern int running;
extern int debugging;
extern debug* dbg;
extern error* err;
extern MidiDriver* midi;
extern MainWindow* mainwindow;
extern EventsWindow* eventswindow;
extern int passing_midi;
extern std::map<Glib::ustring, int> keymap_stoi;
extern std::map<int, Glib::ustring> keymap_itos;
extern std::vector<Event *> events;
extern std::map<int,Glib::ustring> notemap;
#endif
/**Ask user a YES-NO question.*/
bool Ask(Glib::ustring message, Glib::ustring secondary_message = "", bool lock_threads = false);
/**Shows user a message.*/
void Info(Glib::ustring message, Glib::ustring secondary_message = "", bool lock_threads = false);

#endif	/* GLOBAL_H */

