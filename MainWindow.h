/* 
 * File:   MainWindow.h
 * Author: cielak
 *
 * Created on 19 sierpień 2010, 16:21
 */
#include <gtkmm.h>
#include <gtkmm/window.h>
#ifndef MAINWINDOW_H
#define	MAINWINDOW_H
class MainWindow: public Gtk::Window{

public:
    MainWindow();
    virtual ~MainWindow();
    void SetMainNote(int note);
    void MainNoteChanged();
    void OnUserClickedExit();
    bool on_delete_event(GdkEventAny* event);
    void TempoChanged();
    Gtk::VBox vbox1;
    Gtk::HBox hbox_up;
    Gtk::SpinButton main_note;
    Gtk::SpinButton tempo_button;
    Gtk::Label tempolabel, mainnotelabel;

};


#endif	/* MAINWINDOW_H */

