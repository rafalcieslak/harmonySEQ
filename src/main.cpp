/*
    Copyright (C) 2010 Rafał Cieślak

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
#include "EventsWindow.h"
#include "Sequencer.h"
#include "Event.h"
#include "EventGUI.h"
#include "Action.h"
#include "Files.h"
#include "TreeModels.h"
//global objects
vector<Sequencer *> sequencers(2);
vector<Event *> events(2);
int mainnote = 60;
double tempo = DEFAULT_TEMPO;
int ports_number;
int running = 1;
debug* dbg;
error* err;
MidiDriver* midi;
MainWindow* mainwindow;
EventsWindow* eventswindow;
int passing_midi;
Glib::ustring file;
std::map<Glib::ustring, int> keymap_stoi;
std::map<int, Glib::ustring> keymap_itos;
std::map<int, Glib::ustring> notemap;
//-/
int debugging = 0, help = 0, version = 0;
int example_notes[6] = {-1,0,2,3,0,0}, example_notes2[6] = {2,3,5,7,0,0};
int example_sequence[8] = {0,1,3,2,1,2,3,1}, example_sequence2[8] = {0,1,3,2,1,2,3,1};
void print_help();
void end_program();

//for getopt
static struct option long_options[]={
    {"debug",no_argument,&debugging,1},
    {"help",no_argument,&help,1},
    {"version",no_argument,&version,1},
    {"pass-midi",no_argument,&passing_midi,1},
    {"ports",required_argument,0,'p'},
    {0,0,0,0}

};

// <editor-fold defaultstate="collapsed" desc="thread classes">

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
    temp[1] = NULL;
    for (int i = 97; i < 123; i++) {
        temp[0] = i;
        keymap_itos[i] = temp;
        keymap_stoi[temp] = i;

    }
}

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
void threadb::th1(){
//midi processing thread
    *dbg << "th1 started\n";
    int npfd;
    struct pollfd* pfd;


    *dbg << "The queue is starting!\n";
    snd_seq_start_queue(midi->seq_handle,midi->queueid,NULL);
    snd_seq_drain_output(midi->seq_handle);

    midi->UpdateQueue(); //initial
    npfd = snd_seq_poll_descriptors_count(midi->seq_handle,POLLIN);
    pfd = (struct pollfd*)alloca(npfd*sizeof(struct pollfd*));
    snd_seq_poll_descriptors(midi->seq_handle,pfd,npfd,POLLIN);

    while(running == 1){
    if (poll(pfd,npfd,1000)>0)
        //*dbg << "w00t! event!\n";
        midi->ProcessInput();
    };
    //when stoped running
}

void threadb::th2(){
//gtk thread
    *dbg << "th2 started\n";
    gdk_threads_enter();
    //pass control to gtk
    Gtk::Main::run(*mainwindow);
    gdk_threads_leave();
}

void InitGui(){
    gdk_threads_enter();
    {
        mainwindow = new MainWindow; 
        eventswindow = new EventsWindow;
    }
    gdk_threads_leave();
    
}

void InitDefaultData(){
    
        sequencers[0] = new Sequencer(example_sequence,example_notes,"seq 0");
        sequencers[1] = new Sequencer(example_sequence2,example_notes2,"seq 1");
    
        events[0] = new Event(Event::KEYBOARD,keymap_stoi.find("1")->second,0);
        events[1] = new Event(Event::KEYBOARD,keymap_stoi.find("2")->second,0);
        events.push_back(new Event(Event::KEYBOARD,keymap_stoi.find("3")->second,0));
        events[0]->actions.push_back(new Action(Action::MAINOTE_SET,48));
        events[1]->actions.push_back(new Action(Action::MAINOTE_SET,60));
        events[2]->actions.push_back(new Action(Action::MAINOTE_SET,72));
}

void InitGetText(){

    //gettext inits
    setlocale(LC_ALL, ""); //sets the locale to user's locale
    bindtextdomain(PACKAGE,LOCALEDIR);
    textdomain(PACKAGE);

}

void InitMidiDriver(){

    //create the midi driver
    midi = new MidiDriver;
    midi->SetTempo(tempo);

}

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
    //gtk inits
    Glib::thread_init();
    gdk_threads_init();
    Gtk::Main kit(argc, argv);

    //random number generator init, may got usefull somewhere in the future
    srand(time(NULL));

    InitGetText();

    running = 1;        //the program IS running
    debugging = 0;      //by default
    help = 0;           //by default
    ports_number = 1;   //by default
    passing_midi = 0;   //by default
    tempo = DEFAULT_TEMPO;
    err = new error();  //error stream is never quiet!

   
    //first, parse the arguments
    char c, temp[100];
    opterr = 0; //this prevents getarg from printing his error message
    int option_index; //getopt stores index here
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

    dbg = new debug(debugging); //start the debugger class
    if (help) {print_help();exit(0);} //print help if required
    if (version) {printf(VERSION);printf("\n");exit(0);} //print version

    //here file path from command line is obtained, if any
    bool file_from_cli = false;
    if (argc>optind){ file = argv[optind];file_from_cli=1;}

    InitMidiDriver();

    InitKeyMap();
    InitNoteMap();

    InitAllTreeModels();

    InitGui();  //Ow... better have all the main windows constructed, before any sequencer on event is.

    InitDefaultData();

    if (file_from_cli) TryToOpenFileFromCommandLine();
    //else InitDefaultData();

    mainwindow->tempo_button.set_value(tempo);
    mainwindow->main_note.set_value(mainnote);

    mainwindow->InitTreeData();
    eventswindow->InitTreeData();

    threadb Th;
    /*Glib::Thread * const th1 =*/ Glib::Thread::create(sigc::mem_fun(Th, &threadb::th1), true);
    /*Glib::Thread * const th2 =*/  Glib::Thread::create(sigc::mem_fun(Th, &threadb::th2), true);
    //wait for signal to exit the program
    while (running == 1)
        usleep(10000);
    sleep(1);

    end_program();
    return 0;
}

void print_help(){
    *dbg << "Hey, seems you wish to debug help message?" << "Nothing to debug, just a few printf's!";
    printf(_("harmonySEQ, version %s\n"
            "\n"
            "usage: harmonySEQ [-hdvp] [FILE]\n"
            "\n"
            "    -p --ports n     sets ports number to n\n"
            "    -d --debug       enters debug mode, prints lots of debug messeges\n"
            "    -h --help        prints this help messase and exits\n"
            "    -v --version     prints the program version\n"), VERSION);
    printf(_("       --pass-midi   passes the midi events through the program by default\n"));
    printf("\n");


}

void end_program(){

    *dbg << "ending the program...\n";
    if (midi != NULL) { //maybe we are ending the program before midi driver was constructed
        midi->ClearQueue();
        midi->DeleteQueue();
    }
    delete mainwindow;
    delete eventswindow;
    delete midi;
    delete dbg;
    delete err;
    exit(0);
}
