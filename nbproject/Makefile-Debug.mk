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
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/SequencerGUI.o \
	${OBJECTDIR}/MainWindow.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/MidiDriver.o \
	${OBJECTDIR}/Files.o \
	${OBJECTDIR}/Sequencer.o \
	${OBJECTDIR}/error.o \
	${OBJECTDIR}/debug.o


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
	"${MAKE}"  -f nbproject/Makefile-Debug.mk harmonyseq

harmonyseq: ${OBJECTFILES}
	${LINK.cc} -lasound -o harmonyseq ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/SequencerGUI.o: SequencerGUI.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/SequencerGUI.o SequencerGUI.cpp

${OBJECTDIR}/MainWindow.o: MainWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/MainWindow.o MainWindow.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/MidiDriver.o: MidiDriver.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/MidiDriver.o MidiDriver.cpp

${OBJECTDIR}/Files.o: Files.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Files.o Files.cpp

${OBJECTDIR}/Sequencer.o: Sequencer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/Sequencer.o Sequencer.cpp

${OBJECTDIR}/error.o: error.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/error.o error.cpp

${OBJECTDIR}/debug.o: debug.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -Wall -I. -MMD -MP -MF $@.d -o ${OBJECTDIR}/debug.o debug.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} harmonyseq

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
