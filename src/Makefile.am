
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
						Atom.cpp \
						NoteAtom.cpp \
						ControllerAtom.cpp \
						AtomContainer.cpp \
						PatternWidget.cpp \
						NoteSequencer.cpp \
						ControlSequencer.cpp \
						DiodeMidiEvent.cpp \
						MainWindow.h MidiDriver.h Sequencer.h messages.h Files.h global.h Event.h EventGUI.h Action.h ActionGUI.h TreeModels.h Chord.h ChordWidget.h Configuration.h SettingsWindow.h seqHandle.h SequencerWidget.h EventsWidget.h OSC.h Atom.h NoteAtom.h ControllerAtom.h AtomContainer.h AtomContainer.h PatternWidget.h NoteSequencer.h ControlSequencer.h DiodeMidiEvent.h main.h

harmonySEQ_CPPFLAGS = $(GTKMM_CFLAGS) $(ADD_DEF) -Wall -O3
if DISABLEOSC
harmonySEQ_LDADD = $(GTKMM_LIBS) -lgthread-2.0 -lasound
else
harmonySEQ_LDADD = $(GTKMM_LIBS) -lgthread-2.0 -lasound -llo
endif
harmonySEQ_LDFLAGS =  -Wall -O3

ACLOCAL_AMFLAGS = -I m4


datadir = @datadir@
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@


LDADD = $(LIBINTL)
