
AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"

if DISABLEOSC
ADD_DEF = -DDISABLE_OSC
else
ADD_DEF = 
endif

bin_PROGRAMS = harmonySEQ
harmonySEQ_SOURCES =  	main.cpp\
						MainWindow.cpp \
						MidiDriver.cpp \
						Sequencer.cpp \
						messages.cpp \
						Files.cpp \
						global.cpp \
						Event.cpp \
						EventGUI.cpp \
						Action.cpp \
						ActionGUI.cpp \
						TreeModels.cpp \
						Chord.cpp \
						ChordWidget.cpp \
						Configuration.cpp \
						SettingsWindow.cpp \
						seqHandle.cpp \
						SequencerWidget.cpp \
						EventsWidget.cpp \
						OSC.cpp \
						MainWindow.h MidiDriver.h Sequencer.h messages.h Files.h global.h Event.h EventGUI.h Action.h ActionGUI.h TreeModels.h Chord.h ChordWidget.h Configuration.h SettingsWindow.h seqHandle.h SequencerWidget.h EventsWidget.h OSC.h\
						gettext.h
						
harmonySEQ_CPPFLAGS = $(GTKMM_CFLAGS) $(ADD_DEF) -Wall -g -O3
harmonySEQ_LDADD = $(GTKMM_LIBS) -lgthread-2.0 -lasound
harmonySEQ_LDFLAGS =  -Wall -g -O3

ACLOCAL_AMFLAGS = -I m4


datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@


LDADD = $(LIBINTL)
