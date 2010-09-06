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

#define VERSION "0.8"
#define ENDL "\n"
#define TICKS_PER_QUARTERNOTE 128
#define _(STRING) gettext(STRING)
#define DEFAULT_TEMPO 240.0

#define FILE_GROUP_SYSTEM "System"
#define FILE_KEY_SYSTEM_TEMPO "tempo"
#define FILE_KEY_SYSTEM_MAINNOTE "mainnote"
#define FILE_KEY_SYSTEM_SEQ_NUM "sequencers_number"

#define FILE_GROUP_TEMPLATE_SEQ "Seq %d"
#define FILE_KEY_SEQ_ON "on"
#define FILE_KEY_SEQ_APPLY_MAIN_NOTE "apply_mn"
#define FILE_KEY_SEQ_CHANNEL "channel"
#define FILE_KEY_SEQ_NAME "name"
#define FILE_KEY_SEQ_SEQUENCE "sequence"
#define FILE_KEY_SEQ_NOTES "notes"
#endif	/* GLOBAL_H */

