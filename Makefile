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
        error.cpp \
        debug.cpp \
        Files.cpp
        
OBJS	= $(SRCS:.cpp=.o)

BUILD_DIR = build

OBD =   $(BUILD_DIR)/main.o\
        $(BUILD_DIR)/MainWindow.o \
        $(BUILD_DIR)/MidiDriver.o \
        $(BUILD_DIR)/Sequencer.o \
        $(BUILD_DIR)/SequencerGUI.o \
        $(BUILD_DIR)/error.o \
        $(BUILD_DIR)/debug.o \
        $(BUILD_DIR)/Files.o

all: $(APP)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/$@


$(APP):	$(OBJS)
	$(CC) $(OBD) $(LDFLAGS) -o $(APP)

clean:
	rm -f *.o  *~ $(APP) $(BUILD_DIR)/*.o


