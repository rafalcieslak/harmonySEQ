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
    void OnTreeviewRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
    void OnMutedToggleToggled(const Glib::ustring& path);
    void OnNameEdited(const Glib::ustring& path,const Glib::ustring& newtext);
    void TempoChanged();
    Gtk::VBox vbox1;
    Gtk::HBox hbox_up;
    Gtk::SpinButton main_note;
    Gtk::SpinButton tempo_button;
    Gtk::Label tempolabel, mainnotelabel;
    

    class ModelColumns : public Gtk::TreeModel::ColumnRecord{
    public:
        ModelColumns(){add(col_ID);add(col_name);add(col_muted);}
        Gtk::TreeModelColumn<int> col_ID;
        Gtk::TreeModelColumn<Glib::ustring> col_name;
        Gtk::TreeModelColumn<bool> col_muted;
    };

    ModelColumns m_columns;
    Gtk::TreeView m_TreeView;
    Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

};


#endif	/* MAINWINDOW_H */

