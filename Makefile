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
        Files.cpp \
	global.cpp \
	Event.cpp \
	EventsWindow.cpp \
	EventGUI.cpp \
	Action.cpp
        
OBJS	= $(SRCS:.cpp=.o)


all: $(APP)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@


$(APP):	$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(APP)

clean:
	rm -f *.o  *~ $(APP)


