AUTOMAKE_OPTIONS = foreign

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
						TreeModels.cpp
						
harmonySEQ_CPPFLAGS = $(GTKMM_CFLAGS) -Wall -g -O3
harmonySEQ_LDADD = $(GTKMM_LIBS) -lgthread-2.0 -lasound
harmonySEQ_LDFLAGS =  -Wall -g -O3
