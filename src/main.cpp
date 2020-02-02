/*
    Copyright (C) 2010-2012 Rafał Cieślak

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
#include "main.h"
#define I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
#include "global.h"
#undef I_DO_NOT_WANT_EXTERNS_FROM_GLOBAL_H
#include "MidiDriver.h"
#include "messages.h"
#include "MainWindow.h"
#include "Sequencer.h"
#include "Event.h"
#include "EventGUI.h"
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
    threadb* Th;
int passing_midi; //states whether all midi events are passed through, or not.
Glib::ustring file;
std::map<Glib::ustring, int> keymap_stoi; //map used for keyname -> id conversion
std::map<int, Glib::ustring> keymap_itos; //map used for id -> keyname conversion
//map used for note_number -> note_name conversion
std::map<int, Glib::ustring> notemap = {
    {0, "C"},
    {1, "C#"},
    {2, "D"},
    {3, "D#"},
    {4, "E"},
    {5, "F"},
    {6, "F#"},
    {7, "G"},
    {8, "G#"},
    {9, "A"},
    {10, "A#"},
    {11, "H"},
};

Glib::RefPtr< Gdk::Pixbuf > harmonySEQ_logo_48;
Glib::RefPtr< Gdk::Pixbuf > metronome_icon_24;
Glib::RefPtr< Gdk::Pixbuf > icon_add_note_seq;
Glib::RefPtr< Gdk::Pixbuf > icon_add_ctrl_seq;
Glib::RefPtr< Gdk::Pixbuf > icon_slope_linear;
Glib::RefPtr< Gdk::Pixbuf > icon_slope_flat;

int debugging = 0, help = 0, version = 0; //flags set by getopt, depending on command-line parameters

bool metronome;
bool diodes_disabled;

void print_help(); //forward declaration of few functions

//for getopt - defines which flag is related to which variable
static struct option long_options[]={
    {"debug",no_argument,&debugging,1},
    {"help",no_argument,&help,1},
    {"version",no_argument,&version,1},
    {"pass-midi",no_argument,&passing_midi,1},
    {0,0,0,0}

};

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

threadb::threadb() {
}

threadb::~threadb() {
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

std::string DetermineDataPath() {
  std::string test_file = "style/harmonySEQ.css";
  std::vector<std::string> candidates = {
#ifdef RELEASE
    // Installed with cmake - only look for resources in installation
    // path.
    DATA_PATH,
#else
    // OOTB with cmake
    "../",
    "./",
#endif
  };

  for(const std::string& candidate: candidates){
    std::string path = candidate + test_file;
    if(Files::fexists(path.c_str())){
      printf("Data files found in %s\n", candidate.c_str());
      return candidate;
    }
  }

  printf("Data files could not be located in any of these paths:\n");
  for(const std::string& candidate: candidates)
    printf("    %s\n", candidate.c_str());
  printf("This usually indicates harmonySEQ was not installed correctly.\n");
  exit(1);
}

void LoadIcons(std::string data_path){
    harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/48x48/apps/harmonyseq.png");
    metronome_icon_24 = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/24x24/metronome.png");
    icon_add_ctrl_seq = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/24x24/add_ctrl_seq.png");
    icon_add_note_seq = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/24x24/add_note_seq.png");
    icon_slope_flat = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/16x16/slope_flat.png");
    icon_slope_linear = Gdk::Pixbuf::create_from_file(data_path + "pixmaps/16x16/slope_linear.png");
}

void EnableCSSProvider(std::string data_path) {
  auto css_provider = Gtk::CssProvider::create();
  try{
    css_provider->load_from_path(data_path + "style/harmonySEQ.css");
    Gtk::StyleContext::add_provider_for_screen(
      Gdk::Screen::get_default(),
      css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
  }catch(const Gtk::CssProviderError& e){
    printf("CssProviderError: %s\n", e.what().c_str());
    exit(1);
  }
}

int main(int argc, char** argv) {
    //random number generator init, may got useful somewhere in the future
    srand(time(NULL));

    // Find where data files are. This is tied with the build system.
    // If this fails, harmonySEQ must not have been installed correctly.
    std::string data_path = DetermineDataPath();

    running = 1;        //the program IS running
    debugging = 0;      //by default
    help = 0;           //by default
    ports_number = 1;   //by default
    passing_midi = 0;   //by default
    metronome = 0; //by default
    tempo = DEFAULT_TEMPO;
    err = new error();  //error stream is never quiet! so we open it, not caring about what we got in arguments

    //Start thread class...
    Th = new threadb;

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
                    sprintf(temp, _("unrecognized option '%s'\n"),argv[optind-1]); //a trick to tell what was the LONG option we couldn't recognize.
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

    //Initializing GTK.
    Glib::thread_init();
    gdk_threads_init();
    Gtk::Main kit(argc, argv);

    //Initing the driver...
    //create the midi driver
    midi = new MidiDriver;
    midi->SetTempo(tempo);

    //...the maps...
    InitKeyMap();

    //...tree models...
    InitAllTreeModels();

    //..icons...
    LoadIcons(data_path);

    //..CSS provider...
    EnableCSSProvider(data_path);

    //...configuration...
    gdk_threads_enter();
    Config::LoadDefaultConfiguration();
    Config::LoadFromFile();
    Config::SaveToFile();
    if(Config::Interaction::DisableDiodes) diodes_disabled = 1;
    else diodes_disabled = 0;
    gdk_threads_leave();

    //...GUI...  Sequencer and UI logic is so intertangled, we must
    // initialize the GUI before any sequencer or even is created.
    gdk_threads_enter();
    mainwindow = new MainWindow;
    settingswindow = new SettingsWindow;
    gdk_threads_leave();

    //...and the OSC server.
#ifndef DISABLE_OSC
    InitOSC();
#endif /*DISABLE_OSC*/

    //Here we init the filename, if it's empty, it means the file was not yet saved
    Files::file_name = "";

    //Trying to open file...
    if (file_from_cli) {
        gdk_threads_enter(); //locking the threads. Loading file MAY ASK!!
        bool x = Files::LoadFile(file);
        gdk_threads_leave();
    }

    //Putting some values into GUI
    mainwindow->tempo_button.set_value(tempo);

    //Initing trees in both windows.
    mainwindow->InitTreeData();
    mainwindow->UpdateEventWidget();

    //And creating both threads.
    Glib::Thread::create(sigc::mem_fun(*Th, &threadb::th1), 0,true,1,Glib::THREAD_PRIORITY_URGENT);
    Glib::Thread::create(sigc::mem_fun(*Th, &threadb::th2),0, true,1,Glib::THREAD_PRIORITY_LOW);

    //Wait for signal to exit the program
    while (running == 1)
        usleep(10000);

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

    return 0;
}
