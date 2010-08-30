
#include "MainWindow.h"
#include "debug.h"
#include "global.h"
#include "MidiDriver.h"
extern int mainnote;
extern debug* dbg;
extern int running;
extern int tempo;
extern MidiDriver* midi;
MainWindow::MainWindow(){
    char temp[30];
    sprintf(temp,"harmonySEQ %s",VERSION);
    set_title(temp);
    set_border_width(5);
    //set_default_size(300,500);

    tempolabel.set_text("Tempo:");
    mainnotelabel.set_text("Main Note:");
    add(vbox1);
    vbox1.pack_start(hbox_up);
    hbox_up.pack_start(mainnotelabel);
    hbox_up.pack_start(main_note,Gtk::PACK_SHRINK);
    main_note.set_range(0,127);
    main_note.set_value(60.0);
    main_note.set_increments(1,12);
    main_note.signal_value_changed().connect(sigc::mem_fun(*this,&MainWindow::MainNoteChanged));
    hbox_up.pack_start(tempolabel);
    hbox_up.pack_start(tempo_button,Gtk::PACK_SHRINK);
    tempo_button.set_range(30,320);
    tempo_button.set_value(tempo);
    tempo_button.set_increments(1,10);
    tempo_button.signal_value_changed().connect(sigc::mem_fun(*this,&MainWindow::TempoChanged));
    show_all_children(1);


}

MainWindow::~MainWindow(){

    
}

void MainWindow::SetMainNote(int note){
    *dbg << "setting note\n";
    main_note.set_value(note);


}

void MainWindow::MainNoteChanged(){
    mainnote = main_note.get_value();


}


bool MainWindow::on_delete_event(GdkEventAny* event){
    *dbg << "user clicked X\n";
    running = 0;
    return 0;
}

void MainWindow::TempoChanged(){
    tempo = tempo_button.get_value();
    midi->SetTempo(tempo);

}