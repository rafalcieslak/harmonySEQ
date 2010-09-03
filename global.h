/* 
 * File:   global.h
 * Author: cielak
 *
 * Created on 19 sierpień 2010, 14:53
 */

#ifndef GLOBAL_H
#define	GLOBAL_H
#include <gtkmm.h>
#include <libintl.h>
#include <locale.h>
#include <fstream>

#define VERSION "0.6"
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

