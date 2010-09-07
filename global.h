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
#include <gtkmm.h>
#include <libintl.h>
#include <locale.h>
#include <fstream>
#include <vector>
using namespace std;
#define VERSION "0.9"
#define ENDL "\n"
#define TICKS_PER_QUARTERNOTE 128
#define TICKS_PER_NOTE (4*TICKS_PER_QUARTERNOTE)
#define _(STRING) gettext(STRING)
#define DEFAULT_TEMPO 240.0

#define FLASH_INTERVAL 50
    //in ms

#define SEQUENCE_DEFAULT_SIZE 8
#define NOTES_CONST_SIZE 6

//the following may not work with gettext. careful!
#define SEQUENCER_DEFAULT_NAME _("sequencer")

#define RESOLUTIONS_NUM 7
#define RESOLUTIONS {1,2,4,8,16,32,64}
#define LENGTHS_NUM 7
#define LENGHTS {0.125,0.25,0.5,1.0,2.0,4.0,8.0}

#define FILE_GROUP_SYSTEM "System"
#define FILE_KEY_SYSTEM_TEMPO "tempo"
#define FILE_KEY_SYSTEM_MAINNOTE "mainnote"
#define FILE_KEY_SYSTEM_SEQ_NUM "sequencers_number"

#define FILE_GROUP_TEMPLATE_SEQ "Seq %d"
#define FILE_KEY_SEQ_RESOLUTION "resolution"
#define FILE_KEY_SEQ_LENGTH "length"
#define FILE_KEY_SEQ_ON "on"
#define FILE_KEY_SEQ_APPLY_MAIN_NOTE "apply_mn"
#define FILE_KEY_SEQ_CHANNEL "channel"
#define FILE_KEY_SEQ_NAME "name"
#define FILE_KEY_SEQ_SEQUENCE "sequence"
#define FILE_KEY_SEQ_NOTES "notes"
#endif	/* GLOBAL_H */

