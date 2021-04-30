/*
    Copyright (C) 2010-2012, 2020 Rafał Cieślak

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

#include <map>
#include <string>
#include <thread>
#include <vector>

#include <getopt.h>
#include <iostream>

#include <glibmm/miscutils.h>

#include "Configuration.hpp"
#include "Files.hpp"
#include "MainWindow.hpp"
#include "Engine.hpp"
#include "OSC.hpp"
#include "SettingsWindow.hpp"
#include "TreeModels.hpp"
#include "config.hpp"
#include "resources.hpp"
#include "shared.hpp"


/* Key global objects */
Engine* engine;
MainWindow* mainwindow;
SettingsWindow* settingswindow;

/* TODO: Move this. */
std::map<std::string, int> keymap_stoi; //map used for keyname -> id conversion
std::map<int, std::string> keymap_itos; //map used for id -> keyname conversion


/* flags set by getopt, depending on command-line parameters */
int debug = 0, help = 0, version = 0;
/* for getopt - defines which flag is related to which variable */
static struct option long_options[]={
    {"debug",no_argument,&debug,1},
    {"help",no_argument,&help,1},
    {"version",no_argument,&version,1},
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
    printf(_("harmonySEQ, version %s\n"
            "\n"
            "usage: harmonySEQ [-hdv] [FILE]\n"
            "\n"
            "   -d    --debug       enters debug mode, prints lots of debug messeges\n"
            "   -h    --help        prints this help messase and exits\n"
            "   -v    --version     prints the program version\n"), VERSION);
    printf("\n");
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

    debug = 0;      //by default
    help = 0;           //by default

    //Now, parse the arguments.
    char c, temp[100];
    opterr = 0; //this prevents getarg from printing his error message
    int option_index; //getopt stores index here
    //Calling getopt to parse agrs, until there are none left.
    while((c=getopt_long(argc,argv,"dhv",long_options,&option_index))!=-1){
        switch(c){
            case 0:
                break;
            case 'd':
                debug = 1;
                break;
            case 'h':
                help = 1;
                break;
            case 'v':
                version = 1;
                break;
            case '?':
                if (optopt){
                    sprintf(temp,_("unknown option '%c'"), optopt);
                    std::cerr << temp << std::endl;
                }else{
                    sprintf(temp, _("unrecognized option '%s'"),argv[optind-1]); //a trick to tell what was the LONG option we couldn't recognize.
                    std::cerr << temp << std::endl;
                }
                help = 1;
                break;
            default:
                abort();
                break;
        }

    }

    //print help, if required
    if (help) {
        print_help();
        exit(0);
    }

    //print version, if reuested
    if (version) {
        printf(VERSION);
        printf("\n");
        exit(0);
    }

    //here file path from command line is obtained, if any
    bool file_from_cli = false;
    std::string file;
    if (argc>optind){
        file = argv[optind];
        file_from_cli=1;
    }

    // Initialize GTK
    Gtk::Main gtk_main(argc, argv);

    // For the icon to appear correctly on GNOME/Wayland, this needs to match
    // the ID used in the AppStream XML and desktop files.
    // See: https://gitlab.com/inkscape/inkscape/-/issues/539
    Glib::set_prgname("org.cielak.harmonyseq");

    // Initialize the MIDI driver...
    engine = new Engine();

    //...the maps...
    InitKeyMap();

    //...tree models...
    InitAllTreeModels();

    //..icons...
    LoadIcons(data_path);

    //..CSS provider...
    EnableCSSProvider(data_path);

    //...configuration...
    Config::LoadDefaultConfiguration();
    Config::LoadFromFile();
    Config::SaveToFile();

    // Apply some global config values to the engine
    engine->SetDiodesEnabled(!Config::Interaction::DisableDiodes);

    //...GUI...  Sequencer and UI logic is so intertangled, we must
    // initialize the GUI before any sequencer or event is created.
    mainwindow = new MainWindow;
    settingswindow = new SettingsWindow;

    //Here we init the filename, if it's empty, it means the file was not yet saved
    Files::file_name = "";

    //Trying to open file...
    if (file_from_cli)
        Files::LoadFile(file, nullptr);

    // Quit GTK main loop when main window is closed.
    mainwindow->on_quit_request.connect([&](){gtk_main.quit();});

    mainwindow->show();

    //And creating both threads.
    std::thread engine_thread([](){
        WaitForDispatcher();
        engine->Run();});
    std::thread ui_thread(UIMain);
    //...and the OSC server.
#ifndef DISABLE_OSC
    RunOSCThread();
#endif

    // Wait as long as the UI is running.
    ui_thread.join();

    // Request engine thread to stop.
    engine->Stop();

    // Join remaining threads
    engine_thread.join();
#ifndef DISABLE_OSC
    StopOSCThead();
#endif

    return 0;
}


void engine_thread(){
    engine->Run();
}
