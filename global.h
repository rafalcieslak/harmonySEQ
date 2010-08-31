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

#define VERSION "0.4"
#define ENDL "\n"
#define TICKS_PER_QUARTERNOTE 128
#define _(STRING) gettext(STRING)
#define DEFAULT_TEMPO 240.0
#endif	/* GLOBAL_H */

