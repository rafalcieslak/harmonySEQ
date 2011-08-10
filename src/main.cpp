/*
    Copyright (C) 2010, 2011 Rafał Cieślak

    This file is part of harmonySEQ.

    HarmonySEQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HarmonySEQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtkmm.h>
#include <getopt.h>

#define I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
#include "global.h"
#undef I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
#include "MidiDriver.h"
#include "messages.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "Event.h"
#include "EventGUI.h"
#include "Action.h"
#include "Files.h"
#include "TreeModels.h"
#include "Configuration.h"
#include "SettingsWindow.h"
#include "OSC.h"
//global objects
vector<Sequencer *> seqVector;
vector<Event *> Events;
double tempo = DEFAULT_TEMPO;
int ports_number;
int running = 1; //states, whether the application is running. When it's changed to 0, all infinite loops in background break, and the whole program closes.
debug* dbg; //the stream-like objects responsible of putting messages into stdio
error* err;     //see above
MidiDriver* midi;
MainWindow* mainwindow;
SettingsWindow* settingswindow;
int passing_midi; //states whether all midi events are passed through, or not.
Glib::ustring file;
std::map<Glib::ustring, int> keymap_stoi; //map used for keyname -> id conversion
std::map<int, Glib::ustring> keymap_itos; //map used for id -> keyname conversion
std::map<int, Glib::ustring> notemap;         //map used for note_number -> note_name conversion

Glib::RefPtr< Gdk::Pixbuf > harmonySEQ_logo_48;
Glib::RefPtr< Gdk::Pixbuf > metronome_icon_24;

int debugging = 0, help = 0, version = 0; //flags set by getopt, depending on command-line parameters

bool metronome;

void print_help(); //forward declaration of few functions
void end_program();

//for getopt - defines which flag is related to which variable
static struct option long_options[]={
    {"debug",no_argument,&debugging,1},
    {"help",no_argument,&help,1},
    {"version",no_argument,&version,1},
    {"pass-midi",no_argument,&passing_midi,1},
    {0,0,0,0}

};

// <editor-fold defaultstate="collapsed" desc="thread classes">

/** Thread-related magic.*/
class threadb : public sigc::trackable {
public:
    threadb();
    ~threadb();
    Glib::Mutex mutex_;
    sigc::signal<void> some_signal;
    void th1();
    void th2();
    bool locked;
};

threadb::threadb() {
}

threadb::~threadb() {
}// </editor-fold>

/**Prepares both keymaps*/
void InitKeyMap()
{
    char temp[30];
    //keys for numbers 0-10
    for (int i = 0; i < 10; i++) {
        sprintf(temp, "%d", i);
        keymap_itos[48 + i] = temp;
        keymap_stoi[temp] = 48 + i;
    }

    //function keys F1-F12
    for (int i = 1; i <= 12; i++) {
        sprintf(temp, "F%d", i);
        keymap_itos[65469 + i] = temp;
        keymap_stoi[temp] = 65469 + i;

    }

    //keys a-z
    temp[1] = 0;
    for (int i = 97; i < 123; i++) {
        temp[0] = i;
        keymap_itos[i] = temp;
        keymap_stoi[temp] = i;

    }
}

/**Prepares notes map*/
void InitNoteMap(){
    notemap[0] = "C";
    notemap[1] = "C#";
    notemap[2] = "D";
    notemap[3] = "D#";
    notemap[4] = "E";
    notemap[5] = "F";
    notemap[6] = "F#";
    notemap[7] = "G";
    notemap[8] = "G#";
    notemap[9] = "A";
    notemap[10] = "A#";
    notemap[11] = "H";
    
}

/**The function that is run, when thread 1 starts.
  **Thread 1 is responsible for midi i/o */
void threadb::th1(){
    *dbg << "th1 started\n";
    //preparing the queue...
    midi->StartQueue();
    //initial call, puts the first ECHO event, to make sure the loop will loop.
    midi->UpdateQueue();
    //go into infinite loop (while running = 1)
    midi->LoopWhileWaitingForInput();
}

/**The function that is run, when thread 2 starts.
  **Thread 2 passec control to GTK, which makes it responsble for the GUI. */
void threadb::th2(){
    *dbg << "th2 started\n";


    gdk_threads_enter();
    //Pass control to gtk.
    Gtk::Main::run(*mainwindow);
    gdk_threads_leave();
}

/**Prepates the GUI. Constructs both the MainWindow and the EventsWindow*/
void InitGui(){
    gdk_threads_enter();
    {
        mainwindow = new MainWindow; 
        settingswindow = new SettingsWindow;
    }
    gdk_threads_leave();
    
}

/**Inits gettext, must be called before any internationalized message is required*/
void InitGetText(){
     //sets the locale to user's locale
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE,LOCALEDIR);
    textdomain(PACKAGE);
}

/**Prepares the Midi Driver*/
void InitMidiDriver(){
    //create the midi driver
    midi = new MidiDriver;
    midi->SetTempo(tempo);
}

void LoadConfig(){
    Config::LoadDefaultConfiguration();
    Config::LoadFromFile();
}

void LoadIcons(){
    
    //Trying to find where are the icons located, by looking for harmonyseq.png.
    if (Files::fexists("/usr/share/icons/hicolor/48x48/apps/harmonyseq.png")){
        //seems we are installed defaultly in /usr/share
        *dbg << "harmonySEQ icon found at: /usr/share/icons/hicolor/48x48/apps/harmonyseq.png" << ENDL;
        harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file("/usr/share/icons/hicolor/48x48/apps/harmonyseq.png");
        metronome_icon_24 = Gdk::Pixbuf::create_from_file("/usr/share/harmonyseq/icons/hicolor/24x24/actions/metronome.png");
    } else
    if (Files::fexists("/usr/local/share/icons/hicolor/48x48/apps/harmonyseq.png")){
        //seems we are installed defaultly in /usr/share/local
        *dbg << "harmonySEQ icon found at: /usr/local/share/icons/hicolor/48x48/apps/harmonyseq.png" << ENDL;
        harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file("/usr/local/share/icons/hicolor/48x48/apps/harmonyseq.png");
        metronome_icon_24 = Gdk::Pixbuf::create_from_file("/usr/local/share/harmonyseq/icons/hicolor/24x24/actions/metronome.png");
    } else
    if (Files::fexists("pixmaps/48x48/apps/harmonyseq.png")){
        //seems we are not installed, and run from main source directory
        *dbg << "harmonySEQ icon found at: pixmaps/48x48/apps/harmonyseq.png" << ENDL;
        harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file("pixmaps/48x48/apps/harmonyseq.png");
        metronome_icon_24 = Gdk::Pixbuf::create_from_file("pixmaps/24x24/metronome.png");
    } else
   if (Files::fexists("../pixmaps/48x48/apps/harmonyseq.png")) {
        //seems we are not installed, and run from src/ directory
        *dbg << "harmonySEQ icon found at: ../pixmaps/48x48/apps/harmonyseq.png" << ENDL;
        harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file("../pixmaps/48x48/apps/harmonyseq.png");
        metronome_icon_24 = Gdk::Pixbuf::create_from_file("../pixmaps/24x24/metronome.png");
    } else{
       //icons not found.
        *err << ("Failed to find harmonySEQ icon files.\n");
    }
    
}

/**As in the name: When we got a filename from command-line, we try to open it.*/
bool TryToOpenFileFromCommandLine(){
    gdk_threads_enter(); //lodking the threads. Loading file MAY ASK!!
    bool x = Files::LoadFile(file);
    gdk_threads_leave();
    if (!x){
        return 0;
    }
    else return 1;
}

int main(int argc, char** argv) {

    //Initializing GTK.
    Glib::thread_init();
    gdk_threads_init();
    Gtk::Main kit(argc, argv);

    //random number generator init, may got useful somewhere in the future
    srand(time(NULL));

    InitGetText();

    running = 1;        //the program IS running
    debugging = 0;      //by default
    help = 0;           //by default
    ports_number = 1;   //by default
    passing_midi = 0;   //by default
    metronome = 0; //by default
    tempo = DEFAULT_TEMPO;
    err = new error();  //error stream is never quiet! so we open it, not caring about what we got in arguments

   
    //Now, parse the arguments.
    char c, temp[100];
    opterr = 0; //this prevents getarg from printing his error message
    int option_index; //getopt stores index here
    //Calling getopt to parse agrs, until there are none left.
    while((c=getopt_long(argc,argv,"dhvp",long_options,&option_index))!=-1){
        switch(c){
            case 0:
                break;
            case 'd':
                debugging = 1;
                break;
            case 'h':
                help = 1;
                break;
            case 'v':
                version = 1;
                break;
            case 'p':
                ports_number = atoi(optarg);
                break;

            case '?':
                if (optopt){
                    sprintf(temp,_("unknown option '%c'\n"), optopt);
                    *err << temp;
                }else{
                    sprintf(temp, gettext("unrecognized option '%s'\n"),argv[optind-1]); //a trick to tell what was the LONG option we couldn't recognize.
                    *err << temp;
                }
                help = 1;
                break;
            default:
                abort();
                break;
        }

    }

    //start the debugger class, enabling it only if debugging = 1.
    dbg = new debug(debugging);
    //print help, if required
    if (help) {print_help();exit(0);}
    //print version, if required
    if (version) {printf(VERSION);printf("\n");exit(0);} 

    //here file path from command line is obtained, if any
    bool file_from_cli = false;
    if (argc>optind){ file = argv[optind];file_from_cli=1;}

    //Initing the driver...
    InitMidiDriver();

    //...the maps...
    InitKeyMap();
    InitNoteMap();

    //...tree models...
    InitAllTreeModels();

    //..icons...
    LoadIcons();

    //...configuration...
    gdk_threads_enter();
    LoadConfig();
    Config::SaveToFile();
    gdk_threads_leave();

    //...GUI...
    InitGui();  //(Ow... better have all the main windows constructed, before any sequencer or event is. Might cause problems elsewhere).

    //...and the OSC server.
#ifndef DISABLE_OSC
    InitOSC();
#endif /*DISABLE_OSC*/
    
    //Here we init the filename, if it's empty, it means the file was not yet saved
    Files::file_name = "";

    //Trying to open file...
    if (file_from_cli) TryToOpenFileFromCommandLine();
    //else InitDefaultData();

    //Putting some values into GUI
    mainwindow->tempo_button.set_value(tempo);

    //Initing trees in both windows.
    mainwindow->InitTreeData();
    mainwindow->UpdateEventWidget();

    //At the beggining, file is not modified.
    Files::SetFileModified(0);

    //Starting an instance of the threads class...
    threadb Th;
    //And creating both threads.
    Glib::Thread::create(sigc::mem_fun(Th, &threadb::th1), 0,true,1,Glib::THREAD_PRIORITY_URGENT);
    Glib::Thread::create(sigc::mem_fun(Th, &threadb::th2),0, true,1,Glib::THREAD_PRIORITY_LOW);

    //Wait for signal to exit the program
    while (running == 1)
        usleep(10000);

    end_program();
    return 0;
}

void print_help(){
    *dbg << "Hey, seems you wish to debug help message?" << "Nothing to debug, just a few printf's!";

    printf(_("harmonySEQ, version %s\n"
            "\n"
            "usage: harmonySEQ [-hdv] [FILE]\n"
            "\n"
            "   -d    --debug       enters debug mode, prints lots of debug messeges\n"
            "         --pass-midi   passes the midi events through the program by default\n"
            "   -h    --help        prints this help messase and exits\n"
            "   -v    --version     prints the program version\n"), VERSION);
    printf("\n");
}

void end_program(){

    *dbg << "ending the program...\n";
    
    gdk_threads_enter();//to ensure thread safety...
    
    if (midi != NULL) { //maybe we are ending the program before midi driver was constructed
        midi->ClearQueue();
        sleep(1); //giving it some time, for the noteoffs that are left on
        midi->AllNotesOff();
        midi->DeleteQueue();
    }
    delete mainwindow;
    delete midi;
    delete dbg;
    delete err;
    exit(0);
}
