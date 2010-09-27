AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"

SUBDIRS = po examples

bin_PROGRAMS = harmonySEQ
harmonySEQ_SOURCES =  	main.cpp\
						MainWindow.cpp \
						MidiDriver.cpp \
						Sequencer.cpp \
						SequencerGUI.cpp \
						messages.cpp \
						Files.cpp \
						global.cpp \
						Event.cpp \
						EventsWindow.cpp \
						EventGUI.cpp \
						Action.cpp \
						ActionGUI.cpp \
						TreeModels.cpp \
						MainWindow.h MidiDriver.h Sequencer.h SequencerGUI.h messages.h Files.h global.h Event.h EventsWindow.h EventGUI.h Action.h ActionGUI.h TreeModels.h \
						gettext.h
						
harmonySEQ_CPPFLAGS = $(GTKMM_CFLAGS) -Wall -g -O3
harmonySEQ_LDADD = $(GTKMM_LIBS) -lgthread-2.0 -lasound
harmonySEQ_LDFLAGS =  -Wall -g -O3

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath CHANGELOG LICENSE

datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@


LDADD = $(LIBINTL)
