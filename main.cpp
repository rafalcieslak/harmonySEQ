/* 
 * File:   main.cpp
 * Author: cielak
 *
 * Created on 18 sierpień 2010, 16:43
 */
#include <gtkmm.h>
#include <cstdlib>
#include <cstdio>
#include <getopt.h>
#include <alsa/asoundlib.h>
#include "global.h"
#include "MidiDriver.h"
#include "debug.h"
#include "error.h"
#include "MainWindow.h"
#include "Sequencer.h"


using namespace std;
int debugging = 0, help = 0, version = 0;
debug* dbg;
error* err;
int mainnote = 60;
int example_notes[6] = {-1,0,2,3,0,0}, example_notes2[6] = {2,3,5,7,0,0};
double tempo = DEFAULT_TEMPO;
int example_sequence[8] = {0,1,3,2,1,2,3,1}, example_sequence2[8] = {0,1,3,2,1,2,3,1};
vector<Sequencer *> sequencers(2);
MidiDriver* midi;
int ports_number;
int running = 1;
void print_help();
void end_program();
void sigint(int sig);
MainWindow* mainwindow;
//for getopt
static struct option long_options[]={
    {"debug",no_argument,&debugging,1},
    {"help",no_argument,&help,1},
    {"version",no_argument,&version,1},
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

void threadb::th1(){
//midi processing thread
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
    gdk_threads_enter();
    //pass control to gtk
    Gtk::Main::run(*mainwindow);
    gdk_threads_leave();
}

int main(int argc, char** argv) {
    //gtk inits
    Glib::thread_init();
    g_thread_init(NULL);
    gdk_threads_init();
    Gtk::Main kit(argc, argv);

    //gettext inits
    setlocale(LC_ALL, ""); //sets the locale to user's locale
    bindtextdomain("harmonySEQ","locale");
    textdomain("harmonySEQ");

    err = new error(); //error stream is never quiet!

    
    //prepare the signals catchers
   // signal(SIGINT,sigint);
   // signal(SIGTERM,sigint); //awww, crashes the terminal... wtf?

    //first, parse the arguments
    char c, temp[100];
    debugging = 0; //by default
    help = 0;
    ports_number = 1;
    opterr = 0; //this prevents getopt from printing his error message
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
                *dbg << _("setting ports number to ") << ports_number << ENDL;
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
    
    //here the non-oprion arguments should be parsed (file to load etc.), but let's leave it for now...


    tempo = DEFAULT_TEMPO;
    //create the midi driver
    midi = new MidiDriver;
    *dbg << "setting tempo to " << tempo << ENDL;
    midi->SetTempo(tempo);
    //random number generator init
    srand(time(NULL));

    gdk_threads_enter();
    {
        sequencers[0] = new Sequencer(example_sequence,example_notes,"seq 0");
        sequencers[1] = new Sequencer(example_sequence2,example_notes2,"seq 1");
        mainwindow = new MainWindow;
    }
    gdk_threads_leave();

    threadb Th;
    running = 1; //the program IS running
    Glib::Thread * const th1 = Glib::Thread::create(sigc::mem_fun(Th, &threadb::th1), true);
    Glib::Thread * const th2 = Glib::Thread::create(sigc::mem_fun(Th, &threadb::th2), true);


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
            "usage: harmonySEQ [-hdvp]\n"
            "\n"
            "    -p --ports n     sets ports number to n\n"
            "    -d --debug       enters debug mode, prints lots of debug messeges\n"
            "    -h --help        prints this help messase and exits\n"
            "    -v --version     prints the program version\n"), VERSION);
    printf("\n");


}

void end_program(){

    *dbg << "ending the program...\n";
    if (midi != NULL) { //maybe we are ending the program before midi driver was constructed
        midi->ClearQueue();
        midi->DeleteQueue();
    }
    delete mainwindow;
    delete midi;
    delete dbg;
    delete err;
    exit(0);
}

void sigint(int sig){
    *dbg << "SIGINT caught!\n";
    end_program();

}