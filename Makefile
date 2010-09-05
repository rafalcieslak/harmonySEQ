## -*- Makefile -*-
##
## User: cielak
## Time: 2010-08-30 17:09:10
## Makefile created by Oracle Solaris Studio.
##
## This file is generated automatically.
##


#### Compiler and tool definitions shared by all build targets #####
CCC = g++
CXX = g++
BASICOPTS = -g -lgthread-2.0
CCFLAGS = $(BASICOPTS) -lasound `pkg-config --cflags --libs gtkmm-2.4`
CXXFLAGS = $(BASICOPTS) -Wall -lasound  `pkg-config --cflags --libs gtkmm-2.4`
CCADMIN = 


# Define the target directories.
TARGETDIR_harmonyseq=build


all: $(TARGETDIR_harmonyseq)/harmonyseq

## Target: harmonyseq
OBJS_harmonyseq =  \
	$(TARGETDIR_harmonyseq)/main.o \
	$(TARGETDIR_harmonyseq)/error.o \
	$(TARGETDIR_harmonyseq)/MidiDriver.o \
	$(TARGETDIR_harmonyseq)/SequencerGUI.o \
	$(TARGETDIR_harmonyseq)/MainWindow.o \
	$(TARGETDIR_harmonyseq)/Sequencer.o \
	$(TARGETDIR_harmonyseq)/debug.o
USERLIBS_harmonyseq = $(SYSLIBS_harmonyseq) 
DEPLIBS_harmonyseq =  
LDLIBS_harmonyseq = $(USERLIBS_harmonyseq)


# Link or archive
$(TARGETDIR_harmonyseq)/harmonyseq: $(TARGETDIR_harmonyseq) $(OBJS_harmonyseq) $(DEPLIBS_harmonyseq)
	$(LINK.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ $(OBJS_harmonyseq) $(LDLIBS_harmonyseq)
	mv $@ $(TARGETDIR_harmonyseq)/../

# Compile source files into .o files
$(TARGETDIR_harmonyseq)/main.o: $(TARGETDIR_harmonyseq) main.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ main.cpp

$(TARGETDIR_harmonyseq)/error.o: $(TARGETDIR_harmonyseq) error.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ error.cpp

$(TARGETDIR_harmonyseq)/MidiDriver.o: $(TARGETDIR_harmonyseq) MidiDriver.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ MidiDriver.cpp

$(TARGETDIR_harmonyseq)/SequencerGUI.o: $(TARGETDIR_harmonyseq) SequencerGUI.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ SequencerGUI.cpp

$(TARGETDIR_harmonyseq)/MainWindow.o: $(TARGETDIR_harmonyseq) MainWindow.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ MainWindow.cpp

$(TARGETDIR_harmonyseq)/Sequencer.o: $(TARGETDIR_harmonyseq) Sequencer.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ Sequencer.cpp

$(TARGETDIR_harmonyseq)/debug.o: $(TARGETDIR_harmonyseq) debug.cpp
	$(COMPILE.cc) $(CCFLAGS_harmonyseq) $(CPPFLAGS_harmonyseq) -o $@ debug.cpp



#### Clean target deletes all generated files ####
clean:
	rm -f \
		$(TARGETDIR_harmonyseq)/harmonyseq \
		$(TARGETDIR_harmonyseq)/main.o \
		$(TARGETDIR_harmonyseq)/error.o \
		$(TARGETDIR_harmonyseq)/MidiDriver.o \
		$(TARGETDIR_harmonyseq)/SequencerGUI.o \
		$(TARGETDIR_harmonyseq)/MainWindow.o \
		$(TARGETDIR_harmonyseq)/Sequencer.o \
		$(TARGETDIR_harmonyseq)/debug.o
	$(CCADMIN)
	rm -f -r $(TARGETDIR_harmonyseq)


# Create the target directory (if needed)
$(TARGETDIR_harmonyseq):
	mkdir -p $(TARGETDIR_harmonyseq)


# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

