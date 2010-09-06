APP = harmonySEQ

CC = g++

GLOBALFLAGS = -O3 -Wall -g

CFLAGS = `pkg-config --cflags gtkmm-2.4` \
         $(GLOBALFLAGS)

LDFLAGS = `pkg-config --libs gtkmm-2.4` \
	      $(GLOBALFLAGS) -lgthread-2.0 -lasound



SRCS =  main.cpp\
        MainWindow.cpp \
        MidiDriver.cpp \
        Sequencer.cpp \
        SequencerGUI.cpp \
        messages.cpp \
        Files.cpp
        
OBJS	= $(SRCS:.cpp=.o)


OBD =   main.o\
        MainWindow.o \
        MidiDriver.o \
        Sequencer.o \
        SequencerGUI.o \
        messages.o \
        Files.o

all: $(APP)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@


$(APP):	$(OBD)
	$(CC) $(OBD) $(LDFLAGS) -o $(APP)

clean:
	rm -f *.o  *~ $(APP)


