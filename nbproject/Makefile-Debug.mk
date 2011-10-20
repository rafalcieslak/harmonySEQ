#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/AtomContainer.o \
	${OBJECTDIR}/src/ChordWidget.o \
	${OBJECTDIR}/_ext/1045285602/EventsWidget.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/_ext/1045285602/OSC.o \
	${OBJECTDIR}/src/EventGUI.o \
	${OBJECTDIR}/src/NoteAtom.o \
	${OBJECTDIR}/src/Chord.o \
	${OBJECTDIR}/src/ActionGUI.o \
	${OBJECTDIR}/src/Sequencer.o \
	${OBJECTDIR}/src/SequencerWidget.o \
	${OBJECTDIR}/src/Atom.o \
	${OBJECTDIR}/_ext/1045285602/ControlSequencer.o \
	${OBJECTDIR}/src/Configuration.o \
	${OBJECTDIR}/src/messages.o \
	${OBJECTDIR}/_ext/1045285602/SettingsWindow.o \
	${OBJECTDIR}/src/ControllerAtom.o \
	${OBJECTDIR}/src/PatternWidget.o \
	${OBJECTDIR}/_ext/1045285602/DiodeMidiEvent.o \
	${OBJECTDIR}/src/Action.o \
	${OBJECTDIR}/_ext/1045285602/seqHandle.o \
	${OBJECTDIR}/src/MainWindow.o \
	${OBJECTDIR}/src/TreeModels.o \
	${OBJECTDIR}/src/MidiDriver.o \
	${OBJECTDIR}/src/Files.o \
	${OBJECTDIR}/src/Event.o \
	${OBJECTDIR}/_ext/1045285602/NoteSequencer.o \
	${OBJECTDIR}/src/global.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 `pkg-config --libs --cflags gtkmm-2.4` -fno-stack-protector 
CXXFLAGS=-m64 `pkg-config --libs --cflags gtkmm-2.4` -fno-stack-protector 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk src/harmonySEQ

src/harmonySEQ: ${OBJECTFILES}
	${MKDIR} -p src
	${LINK.cc} -lasound -o src/harmonySEQ ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/AtomContainer.o: src/AtomContainer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/AtomContainer.o src/AtomContainer.cpp

${OBJECTDIR}/src/ChordWidget.o: src/ChordWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ChordWidget.o src/ChordWidget.cpp

${OBJECTDIR}/_ext/1045285602/EventsWidget.o: ../Programy/harmonySEQ/trunk/src/EventsWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/EventsWidget.o ../Programy/harmonySEQ/trunk/src/EventsWidget.cpp

${OBJECTDIR}/src/main.o: src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/_ext/1045285602/OSC.o: ../Programy/harmonySEQ/trunk/src/OSC.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/OSC.o ../Programy/harmonySEQ/trunk/src/OSC.cpp

${OBJECTDIR}/src/EventGUI.o: src/EventGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/EventGUI.o src/EventGUI.cpp

${OBJECTDIR}/src/NoteAtom.o: src/NoteAtom.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/NoteAtom.o src/NoteAtom.cpp

${OBJECTDIR}/src/Chord.o: src/Chord.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Chord.o src/Chord.cpp

${OBJECTDIR}/src/ActionGUI.o: src/ActionGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ActionGUI.o src/ActionGUI.cpp

${OBJECTDIR}/src/Sequencer.o: src/Sequencer.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Sequencer.o src/Sequencer.cpp

${OBJECTDIR}/src/SequencerWidget.o: src/SequencerWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/SequencerWidget.o src/SequencerWidget.cpp

${OBJECTDIR}/src/Atom.o: src/Atom.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Atom.o src/Atom.cpp

${OBJECTDIR}/_ext/1045285602/ControlSequencer.o: ../Programy/harmonySEQ/trunk/src/ControlSequencer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/ControlSequencer.o ../Programy/harmonySEQ/trunk/src/ControlSequencer.cpp

${OBJECTDIR}/src/Configuration.o: src/Configuration.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Configuration.o src/Configuration.cpp

${OBJECTDIR}/src/messages.o: src/messages.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/messages.o src/messages.cpp

${OBJECTDIR}/_ext/1045285602/SettingsWindow.o: ../Programy/harmonySEQ/trunk/src/SettingsWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/SettingsWindow.o ../Programy/harmonySEQ/trunk/src/SettingsWindow.cpp

${OBJECTDIR}/src/ControllerAtom.o: src/ControllerAtom.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/ControllerAtom.o src/ControllerAtom.cpp

${OBJECTDIR}/src/PatternWidget.o: src/PatternWidget.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/PatternWidget.o src/PatternWidget.cpp

${OBJECTDIR}/_ext/1045285602/DiodeMidiEvent.o: ../Programy/harmonySEQ/trunk/src/DiodeMidiEvent.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/DiodeMidiEvent.o ../Programy/harmonySEQ/trunk/src/DiodeMidiEvent.cpp

${OBJECTDIR}/src/Action.o: src/Action.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Action.o src/Action.cpp

${OBJECTDIR}/_ext/1045285602/seqHandle.o: ../Programy/harmonySEQ/trunk/src/seqHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/seqHandle.o ../Programy/harmonySEQ/trunk/src/seqHandle.cpp

${OBJECTDIR}/src/MainWindow.o: src/MainWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MainWindow.o src/MainWindow.cpp

${OBJECTDIR}/src/TreeModels.o: src/TreeModels.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/TreeModels.o src/TreeModels.cpp

${OBJECTDIR}/src/MidiDriver.o: src/MidiDriver.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/MidiDriver.o src/MidiDriver.cpp

${OBJECTDIR}/src/Files.o: src/Files.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Files.o src/Files.cpp

${OBJECTDIR}/src/Event.o: src/Event.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/Event.o src/Event.cpp

${OBJECTDIR}/_ext/1045285602/NoteSequencer.o: ../Programy/harmonySEQ/trunk/src/NoteSequencer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1045285602
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1045285602/NoteSequencer.o ../Programy/harmonySEQ/trunk/src/NoteSequencer.cpp

${OBJECTDIR}/src/global.o: src/global.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/src/global.o src/global.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} src/harmonySEQ

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
