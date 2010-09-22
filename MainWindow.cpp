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

#include "MainWindow.h"
#include "messages.h"
#include "global.h"
#include "MidiDriver.h"
#include "Sequencer.h"
#include "Files.h"
#include "Event.h"

//externs from main.cpp
extern int running;
extern int debugging;




MainWindow::MainWindow()
{
    char temp[30];
    sprintf(temp, "harmonySEQ %s", VERSION);
    set_title(temp);
    set_border_width(5);
    //set_default_size(300,500);

    tempolabel.set_text(_("Tempo:"));
    mainnotelabel.set_text(_("Main Note:"));
    add(vbox1);
    vbox1.pack_start(hbox_up, Gtk::PACK_SHRINK);
    hbox_up.pack_start(mainnotelabel);
    hbox_up.pack_start(main_note, Gtk::PACK_SHRINK);
    main_note.set_range(0, 127);
    main_note.set_value(60.0);
    main_note.set_increments(1, 12);
    main_note.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::MainNoteChanged));
    hbox_up.pack_start(tempolabel);
    hbox_up.pack_start(tempo_button, Gtk::PACK_SHRINK);
    tempo_button.set_range(30, 320);
    tempo_button.set_value(DEFAULT_TEMPO);
    tempo_button.set_increments(1, 10);
    tempo_button.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::TempoChanged));


    vbox1.pack_start(m_TreeView, Gtk::PACK_SHRINK);
    // <editor-fold defaultstate="collapsed" desc="tree">
    { //creating the tree model
        m_refTreeModel = Gtk::ListStore::create(m_columns);
        m_TreeView.set_model(m_refTreeModel);

        if(debugging) m_TreeView.append_column(_("ID"), m_columns.col_ID);
        
        int col_count = m_TreeView.append_column_editable(_("Name"), m_columns.col_name);
        Gtk::CellRenderer* cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererText& txt = dynamic_cast<Gtk::CellRendererText&> (*cell);
        txt.signal_edited().connect(mem_fun(*this, &MainWindow::OnNameEdited));
        col_count = m_TreeView.append_column_editable(_("On"), m_columns.col_muted);
        cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererToggle& tgl = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl.signal_toggled().connect(mem_fun(*this, &MainWindow::OnMutedToggleToggled));
        col_count = m_TreeView.append_column_editable(_("MN"), m_columns.col_apply_mainnote);
        cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererToggle& tgl2 = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl2.signal_toggled().connect(mem_fun(*this, &MainWindow::OnApplyMainNoteToggleToggled));
        col_count = m_TreeView.append_column(_("Chan"), m_columns.col_channel);
        col_count = m_TreeView.append_column(_("Res"), m_columns.col_res);
        col_count = m_TreeView.append_column_numeric(_("Len"), m_columns.col_len,"%g");
        col_count = m_TreeView.append_column(_("Vol"), m_columns.col_vol);


        Gtk::TreeView::Column* pColumn;
        int tricky = 0;
        pColumn = m_TreeView.get_column(tricky);
        if(debugging){
            pColumn->set_sort_column(m_columns.col_ID);
            tricky++;
            pColumn = m_TreeView.get_column(tricky);
        }

        //pColumn->set_sort_column(m_columns.col_name);
        //disabled, as it causes strange problems with TreeRowReference-s
        tricky++;
        pColumn = m_TreeView.get_column(tricky);
        //pColumn->set_sort_column(m_columns.col_muted);
        pColumn->set_fixed_width(10);
        tricky++;
        pColumn = m_TreeView.get_column(tricky);
        //pColumn->set_sort_column(m_columns.col_apply_mainnote);
        pColumn->set_fixed_width(10);
        tricky++;
        pColumn = m_TreeView.get_column(tricky);
        //pColumn->set_sort_column(m_columns.col_channel);
        tricky++;
        pColumn = m_TreeView.get_column(tricky);
        //pColumn->set_sort_column(m_columns.col_res);
        //tricky++;
        //pColumn = m_TreeView.get_column(tricky);
        //pColumn->set_sort_column(m_columns.col_len);

        //drag and drop enabling
        //m_TreeView.enable_model_drag_source();
        //m_TreeView.enable_model_drag_dest();
        //disabled, as it causes strange problems with TreeRowReference-s

        //forbids to typesearch
        m_TreeView.set_enable_search(0);

        //catching row selection signal
        m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this, &MainWindow::OnTreeviewRowActivated));

        //react on selection change (to determine whether it is empty)
        Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_TreeView.get_selection();
        refTreeSelection->signal_changed().connect(mem_fun(*this,&MainWindow::OnSelectionChanged));

        //initial data
        //InitTreeData();
        /*is called from main()*/

    }// </editor-fold>
    vbox1.pack_start(hbox_down, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_add, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_clone, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_remove, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_save, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_open, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_events, Gtk::PACK_SHRINK);
    button_add.set_label(_("Add"));
    button_add.signal_clicked().connect(mem_fun(*this, &MainWindow::OnButtonAddClicked));
    button_save.set_label(_("Save"));
    button_save.signal_clicked().connect(mem_fun(*this, &MainWindow::OnSaveClicked));
    button_open.set_label(_("Open"));
    button_open.signal_clicked().connect(mem_fun(*this, &MainWindow::OnLoadClicked));
    button_remove.set_label(_("Remove"));
    button_remove.signal_clicked().connect(mem_fun(*this, &MainWindow::OnRemoveClicked));
    button_remove.set_sensitive(0);
    button_clone.set_label(_("Clone"));
    button_clone.signal_clicked().connect(mem_fun(*this, &MainWindow::OnCloneClicked));
    button_clone.set_sensitive(0);
    button_events.set_label(_("Events"));
    button_events.signal_clicked().connect(mem_fun(*this, &MainWindow::OnEventsClicked));

    vbox1.pack_start(pass_toggle,Gtk::PACK_SHRINK);
    pass_toggle.set_label(_("Pass MIDI events"));
    pass_toggle.set_active(passing_midi);
    pass_toggle.signal_clicked().connect(mem_fun(*this,&MainWindow::OnPassToggleClicked));




    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect(mem_fun(*this,&MainWindow::OnKeyPress));



    show_all_children(1);

}

MainWindow::~MainWindow()
{

}


void
MainWindow::MainNoteChanged()
{
    mainnote = main_note.get_value();
}

bool
MainWindow::on_delete_event(GdkEventAny* event)
{
    *dbg << "user clicked X\n";
    running = 0;
    return 0;
}

void
MainWindow::TempoChanged()
{
    tempo = tempo_button.get_value();
    midi->SetTempo(tempo);

}

void
MainWindow::OnTreeviewRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter;


        ///(activated row) <row number>
        *dbg << _("activated row ") << row[m_columns.col_ID];
        *dbg << _(", name is: ") << row[m_columns.col_name] << ENDL;
        gdk_threads_leave(); //not really sure about this thread-lock, but this the only way I found to get it to work
        {
            sequencers[row[m_columns.col_ID]]->ShowWindow();
        }
        gdk_threads_enter();
    }

}

void
MainWindow::OnMutedToggleToggled(const Glib::ustring& path)
{

    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    *dbg << "the clicked row's ID was " << row[m_columns.col_ID] << ENDL;
    sequencers[row[m_columns.col_ID]]->SetOn(row[m_columns.col_muted]);
}

void MainWindow::OnApplyMainNoteToggleToggled(const Glib::ustring& path){

    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    *dbg << "the clicked row's ID was " << row[m_columns.col_ID] << ENDL;
    sequencers[row[m_columns.col_ID]]->SetApplyMainNote(row[m_columns.col_apply_mainnote]);

}

void
MainWindow::OnNameEdited(const Glib::ustring& path, const Glib::ustring& newtext)
{

    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    sequencers[row[m_columns.col_ID]]->SetName(newtext);

    eventswindow->InitTreeData();
}

Gtk::TreeModel::RowReference MainWindow::AddSequencerRow(int n)
{
    *dbg << "wooho! sequener " << n << " was just added, and we have to add it now to a new row in the list!" << ENDL;
    Gtk::TreeModel::iterator iter = m_refTreeModel->append();
    Gtk::TreeModel::Row row = *(iter);
    row[m_columns.col_ID] = n;
    row[m_columns.col_name] = sequencers[n]->GetName();
    row[m_columns.col_muted] = sequencers[n]->GetOn();
    row[m_columns.col_apply_mainnote] = sequencers[n]->GetApplyMainNote();
    row[m_columns.col_channel] = sequencers[n]->GetChannel();
    row[m_columns.col_res] = sequencers[n]->resolution;
    row[m_columns.col_len] = sequencers[n]->length;
    row[m_columns.col_vol] = sequencers[n]->GetVolume();
    Gtk::TreeRowReference rowref(m_refTreeModel,m_refTreeModel->get_path(iter));
    sequencers[n]->row_in_main_window = rowref;
    return rowref;
}



void MainWindow::InitTreeData(){
    *dbg << "loading initial data to the treeview\n";
    m_refTreeModel->clear();
    Gtk::TreeModel::Row row;
    int rowcount = 0;
    for (unsigned int x = 0; x < sequencers.size(); x++) {
        if (!sequencers[x]) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = m_refTreeModel->append();
        row = *(iter);
        row[m_columns.col_ID] = x;
        row[m_columns.col_muted] = sequencers[x]->GetOn();
        row[m_columns.col_name] = sequencers[x]->GetName();
        row[m_columns.col_apply_mainnote] = sequencers[x]->GetApplyMainNote();
        row[m_columns.col_channel] = sequencers[x]->GetChannel();
        row[m_columns.col_res] = sequencers[x]->resolution;
        row[m_columns.col_len] = sequencers[x]->length;
        row[m_columns.col_vol] = sequencers[x]->GetVolume();
        Gtk::TreeRowReference rowref(m_refTreeModel,m_refTreeModel->get_path(iter));
        sequencers[x]->row_in_main_window = rowref;
        rowcount++;

    }

}

void MainWindow::RefreshRow(Gtk::TreeRowReference rowref){
    *dbg << "refreshing row" << ENDL;
    Gtk::TreeModel::Row row = *(m_refTreeModel->get_iter(rowref.get_path()));

    int x = row[m_columns.col_ID];
    row[m_columns.col_muted] = sequencers[x]->GetOn();
    row[m_columns.col_name] = sequencers[x]->GetName();
    row[m_columns.col_apply_mainnote] = sequencers[x]->GetApplyMainNote();
    row[m_columns.col_channel] = sequencers[x]->GetChannel();
    row[m_columns.col_res] = sequencers[x]->resolution;
    row[m_columns.col_len] = sequencers[x]->length;
    row[m_columns.col_vol] = sequencers[x]->GetVolume();

}

void MainWindow::OnLoadClicked(){

    Files::LoadFileDialog();

    
    InitTreeData();
    main_note.set_value(mainnote);
    tempo_button.set_value(tempo);
    //erasing smallers the treeview, but not the window.
    resize(2,2); //resizing to a tiny size, but the window won't get that small, it will be big enough to show all widgets.
}

void MainWindow::OnSaveClicked(){
    Files::SaveToFile();

}

void MainWindow::OnRemoveClicked(){
    Gtk::TreeModel::iterator iter = *(m_TreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id = row[m_columns.col_ID];
    *dbg << "removing row of id " << id << ENDL;
    
    m_refTreeModel->erase(iter);
    //erasing smallers the treeview, but not the window.
    resize(2,2); //resizing to a tiny size, but the window won't get that small, it will be big enough to show all widgets.
    delete sequencers[id];
    sequencers[id] = NULL;

    eventswindow->InitTreeData();

}

void MainWindow::OnButtonAddClicked(){

    Gtk::TreeModel::RowReference rowref = spawn_sequencer();

    Gtk::TreeModel::iterator iter = rowref.get_model()->get_iter(rowref.get_path());
    m_TreeView.get_selection()->select(iter);

}

void MainWindow::OnCloneClicked(){
    Gtk::TreeModel::iterator iter = *(m_TreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id = row[m_columns.col_ID]; 

    Gtk::TreeModel::RowReference rowref = clone_sequencer(id);
    iter = rowref.get_model()->get_iter(rowref.get_path());
    m_TreeView.get_selection()->select(iter);

    FlashTempoStart();
}

void MainWindow::FlashTempoStart(){
    tempo_button.modify_base(Gtk::STATE_NORMAL,Gdk::Color("red"));
    Glib::signal_timeout().connect(mem_fun(*this,&MainWindow::FlashTempoEnd),FLASH_INTERVAL);
}

bool MainWindow::FlashTempoEnd(){
    tempo_button.unset_base(Gtk::STATE_NORMAL);
    return false; //do not repeat the timeout
}

void MainWindow::OnPassToggleClicked(){
    passing_midi = pass_toggle.get_active();


}

bool MainWindow::OnKeyPress(GdkEventKey* event){
    //*dbg << "triggered " << event->keyval << "\n";
    std::map<int,string>::iterator iter;
    iter = keymap_itos.find(event->keyval);
    if(iter != keymap_itos.end()){
        *dbg << "Pressed key '" << iter->second << "'.\n";

    }else
        *dbg << "Unknown key pressed\n";

    FindAndProcessEvents(Event::KEYBOARD,event->keyval);

    return 1;
}

void MainWindow::OnEventsClicked(){
    eventswindow->show();

}

void MainWindow::OnSelectionChanged(){
    Gtk::TreeModel::iterator iter = m_TreeView.get_selection()->get_selected();
    if(iter){
        //something is selected
        button_remove.set_sensitive(1);
        button_clone.set_sensitive(1);
    }else{
        //selection is empty
        button_remove.set_sensitive(0);
        button_clone.set_sensitive(0);

    }

}

