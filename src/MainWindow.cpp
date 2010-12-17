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
#include "EventsWindow.h"
#include "TreeModels.h"
#include "config.h"

Glib::RefPtr< Gdk::Pixbuf > harmonySEQ_logo_48;


MainWindow::MainWindow()
{
    set_border_width(0);
    //set_default_size(300,500);
    UpdateTitle();
            
    tempolabel.set_text(_("Tempo:"));
    mainnotelabel.set_text(_("Main Note:"));
    add(main_vbox);

    // <editor-fold defaultstate="collapsed" desc="menus creation and initialisation">
    m_refActionGroup = Gtk::ActionGroup::create();

    m_refActionGroup->add(Gtk::Action::create("MenuFile",_("File")));
    m_refActionGroup->add(Gtk::Action::create("MenuHelp",_( "Help")));
    m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW), sigc::mem_fun(*this, &MainWindow::OnMenuNewClicked));
    m_refActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN), sigc::mem_fun(*this, &MainWindow::OnMenuOpenClicked));
    m_refActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE), sigc::mem_fun(*this, &MainWindow::OnMenuSaveClicked));
    m_refActionGroup->add(Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS), sigc::mem_fun(*this, &MainWindow::OnMenuSaveAsClicked));
    m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT), sigc::mem_fun(*this, &MainWindow::OnMenuQuitClicked));
    m_refActionGroup->add(Gtk::Action::create("About", Gtk::Stock::ABOUT), sigc::mem_fun(*this, &MainWindow::OnAboutMenuClicked));

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    add_accel_group(m_refUIManager->get_accel_group());

    Glib::ustring ui_info =
            "<ui>"
            "  <menubar name='MenuBar'>"
            "    <menu action='MenuFile'>"
            "      <menuitem action='FileNew'/>"
            "      <separator/>"
            "      <menuitem action='FileOpen'/>"
            "      <menuitem action='FileSave'/>"
            "      <menuitem action='FileSaveAs'/>"
            "      <separator/>"
            "      <menuitem action='FileQuit'/>"
            "    </menu>"
            "    <menu action='MenuHelp'>"
            "      <menuitem action='About'/>"
            "    </menu>"
            "  </menubar>"
            "</ui>";
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        m_refUIManager->add_ui_from_string(ui_info);
    } catch (const Glib::Error& ex) {
        *err << _("ERROR - error while building menus: ") << ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if (ex.get()) {
        *err << _("ERROR - error while building menus: ") << ex->what();
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED


    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar"); // </editor-fold>

    main_vbox.pack_start(*pMenubar,Gtk::PACK_SHRINK);
    main_vbox.pack_start(vbox1,Gtk::PACK_SHRINK);
    vbox1.set_border_width(5);

    vbox1.pack_start(hbox_up, Gtk::PACK_SHRINK);
    hbox_up.pack_start(mainnotelabel);
    hbox_up.pack_start(main_note, Gtk::PACK_SHRINK);
    main_note.set_range(0, 127);
    main_note.set_increments(1, 12);
    main_note.set_value(mainnote);
    main_note.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::MainNoteChanged));
    hbox_up.pack_start(tempolabel);
    hbox_up.pack_start(tempo_button, Gtk::PACK_SHRINK);
    tempo_button.set_range(30, 320);
    tempo_button.set_increments(1, 10);
    tempo_button.set_value(tempo);
    tempo_button.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::TempoChanged));
    hbox_up.pack_start(play_pause_button);

    Gtk::Stock::lookup(Gtk::Stock::MEDIA_PAUSE,Gtk::ICON_SIZE_BUTTON,image_pause);
    Gtk::Stock::lookup(Gtk::Stock::MEDIA_PLAY,Gtk::ICON_SIZE_BUTTON,image_play);
    UpdatePlayPauseButton();
    play_pause_button.signal_clicked().connect(mem_fun(*this,&MainWindow::OnPauseButtonClicked));

    vbox1.pack_start(m_TreeView, Gtk::PACK_SHRINK);
    // <editor-fold defaultstate="collapsed" desc="tree">
    { //creating the tree model
        m_refTreeModel_sequencers = Gtk::ListStore::create(m_columns_sequencers);

        if(debugging) m_TreeView.append_column(_("ID"), m_columns_sequencers.col_ID);
        
        int col_count = m_TreeView.append_column_editable(_("Name"), m_columns_sequencers.col_name);
        Gtk::CellRenderer* cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererText& txt = dynamic_cast<Gtk::CellRendererText&> (*cell);
        txt.signal_edited().connect(mem_fun(*this, &MainWindow::OnNameEdited));

        col_count = m_TreeView.append_column_editable(_("On"), m_columns_sequencers.col_muted);
        Gtk::TreeViewColumn * column = m_TreeView.get_column(col_count-1);
        cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        column->add_attribute(cell->property_cell_background(),m_columns_sequencers.col_colour);
        Gtk::CellRendererToggle& tgl = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl.signal_toggled().connect(mem_fun(*this, &MainWindow::OnMutedToggleToggled));
        
        col_count = m_TreeView.append_column_editable(_("MN"), m_columns_sequencers.col_apply_mainnote);
        cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererToggle& tgl2 = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl2.signal_toggled().connect(mem_fun(*this, &MainWindow::OnApplyMainNoteToggleToggled));
        
        col_count = m_TreeView.append_column(_("Chan"), m_columns_sequencers.col_channel);
        col_count = m_TreeView.append_column(_("Pat"), m_columns_sequencers.col_pattern);
        col_count = m_TreeView.append_column(_("Res"), m_columns_sequencers.col_res);
        col_count = m_TreeView.append_column_numeric(_("Len"), m_columns_sequencers.col_len,"%g");
        col_count = m_TreeView.append_column(_("Vol"), m_columns_sequencers.col_vol);


        Gtk::TreeView::Column* pColumn;
        int tricky = 0;
        pColumn = m_TreeView.get_column(tricky);
        if(debugging){
            pColumn->set_sort_column(m_columns_sequencers.col_ID);
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

        m_TreeView.set_model(m_refTreeModel_sequencers);
        //initial data
        //InitTreeData();
        /*is called from main()*/

    }// </editor-fold>

    vbox1.pack_start(hbox_down, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_add, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_clone, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_remove, Gtk::PACK_SHRINK);
    hbox_down.pack_start(button_events, Gtk::PACK_SHRINK);
    button_add.set_label(_("Add"));
    button_add.signal_clicked().connect(mem_fun(*this, &MainWindow::OnButtonAddClicked));
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
    play_pause_button.get_image()->show();

    // <editor-fold defaultstate="collapsed" desc="loading logos">
    //loading logo
    bool found = 1;
    Glib::ustring filename = "/usr";
    filename += "/share/icons/hicolor/48x48/apps/harmonyseq.png";
    if (!Files::fexists(filename.c_str())) {
        *dbg << _("Failed to load harmonySEQ logo icon from: ") << filename << ENDL;
        filename = "pixmaps/48x48/apps/harmonyseq.png";
        if (!Files::fexists(filename.c_str())) {
            *dbg << _("Failed to load harmonySEQ logo icon from: ") << filename << ENDL;
            filename = "../pixmaps/48x48/apps/harmonyseq.png";
            if (!Files::fexists(filename.c_str())) {
                *dbg << _("Failed to load harmonySEQ logo icon from: ") << filename << ENDL;
                filename = "/usr/local/share/icons/hicolor/48x48/apps/harmonyseq.png";
                if (!Files::fexists(filename.c_str())) {
                    *dbg << _("Failed to load harmonySEQ logo icon from: ") << filename << ENDL << _("Icon not found.\n");
                    found = 0;
                }
            }
        }
    }

    if (found) {
        harmonySEQ_logo_48 = Gdk::Pixbuf::create_from_file(filename); //will be also used a.o. in about-box
        set_icon(harmonySEQ_logo_48);
    } else {
        *err << ("Failed to find harmonySEQ icon file.\n");
    }// </editor-fold>


}

MainWindow::~MainWindow()
{

}

void MainWindow::UpdateTitle(){
    char temp[300];
    if (Files::file_name == ""){
           sprintf(temp, "harmonySEQ %s - %s", VERSION,_("Untitled")) ;
    }else{
        if (Files::file_modified)
           sprintf(temp, "harmonySEQ %s - %s [*]", VERSION,Files::file_name.c_str()) ;
        else
            sprintf(temp, "harmonySEQ %s - %s", VERSION,Files::file_name.c_str());
    }
    set_title(temp);
}

void
MainWindow::MainNoteChanged()
{
    mainnote = main_note.get_value();
    Files::SetFileModified(1);
}

bool
MainWindow::on_delete_event(GdkEventAny* event)
{
    *dbg << "user clicked X\n";
    if(Files::file_modified)
        if(!Ask(_("The file has unsaved changes."),_("Are sure you want to quit?")))
          return 1;
    
    running = 0;
    return 0;
}

void
MainWindow::TempoChanged()
{
    tempo = tempo_button.get_value();
    midi->SetTempo(tempo);
    Files::SetFileModified(1);
}

void
MainWindow::OnTreeviewRowActivated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
    Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->get_iter(path);
    if (iter)
    {
        Gtk::TreeModel::Row row = *iter;


        ///(activated row) <row number>
        *dbg << _("activated row ") << row[m_columns_sequencers.col_ID];
        *dbg << _(", name is: ") << row[m_columns_sequencers.col_name] << ENDL;
        gdk_threads_leave(); //not really sure about this thread-lock, but this the only way I found to get it to work
        {
            sequencers[row[m_columns_sequencers.col_ID]]->ShowWindow();
        }
        gdk_threads_enter();
    }

}

void
MainWindow::OnMutedToggleToggled(const Glib::ustring& path)
{

    Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;

   sequencers[row[m_columns_sequencers.col_ID]]->SetOn(!row[m_columns_sequencers.col_muted]);
   sequencers[row[m_columns_sequencers.col_ID]]->UpdateGui();
   if(sequencers[row[m_columns_sequencers.col_ID]]->row_in_main_window) RefreshRow(sequencers[row[m_columns_sequencers.col_ID]]->row_in_main_window);

   //Files::SetFileModified(1); do not detect mutes
}

void MainWindow::OnApplyMainNoteToggleToggled(const Glib::ustring& path){

    Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    sequencers[row[m_columns_sequencers.col_ID]]->SetApplyMainNote(!row[m_columns_sequencers.col_apply_mainnote]);
   if(sequencers[row[m_columns_sequencers.col_ID]]->row_in_main_window) RefreshRow(sequencers[row[m_columns_sequencers.col_ID]]->row_in_main_window);

    Files::SetFileModified(1);
}

void
MainWindow::OnNameEdited(const Glib::ustring& path, const Glib::ustring& newtext)
{

    Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    sequencers[row[m_columns_sequencers.col_ID]]->SetName(newtext);
   if(sequencers[row[m_columns_sequencers.col_ID]]->row_in_main_window) RefreshRow(sequencers[row[m_columns_sequencers.col_ID]]->row_in_main_window);

    eventswindow->UpdateAll();
    Files::SetFileModified(1);
}

Gtk::TreeModel::RowReference MainWindow::AddSequencerRow(int x)
{
    *dbg << "wooho! sequener " << x << " was just added, and we have to add it now to a new row in the list!" << ENDL;
    Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->append();
    Gtk::TreeModel::Row row = *(iter);
    row[m_columns_sequencers.col_ID] = x;
    row[m_columns_sequencers.col_name] = sequencers[x]->GetName();
    row[m_columns_sequencers.col_muted] = sequencers[x]->GetOn();
    row[m_columns_sequencers.col_apply_mainnote] = sequencers[x]->GetApplyMainNote();
    row[m_columns_sequencers.col_channel] = sequencers[x]->GetChannel();
    row[m_columns_sequencers.col_pattern] = sequencers[x]->active_pattern;
    row[m_columns_sequencers.col_res] = sequencers[x]->resolution;
    row[m_columns_sequencers.col_len] = sequencers[x]->length;
    row[m_columns_sequencers.col_vol] = sequencers[x]->GetVolume();
    if(sequencers[x]->GetOn()){
        row[m_columns_sequencers.col_colour] = "green1";
    }else if (sequencers[x]->GetPlayOncePhase() == 2 || sequencers[x]->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = "yellow1";
    }else if(sequencers[x]->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = "yellow2";
    }else{
        row[m_columns_sequencers.col_colour] = "white";
    }
    Gtk::TreeRowReference rowref(m_refTreeModel_sequencers,m_refTreeModel_sequencers->get_path(iter));
    sequencers[x]->row_in_main_window = rowref;
    return rowref;
}



void MainWindow::InitTreeData(){
    *dbg << "loading initial data to the treeview\n";
    m_refTreeModel_sequencers->clear();
    Gtk::TreeModel::Row row;
    int rowcount = 0;
    for (unsigned int x = 0; x < sequencers.size(); x++) {
        if (!sequencers[x]) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = m_refTreeModel_sequencers->append();
        row = *(iter);
        row[m_columns_sequencers.col_ID] = x;
        row[m_columns_sequencers.col_muted] = sequencers[x]->GetOn();
        row[m_columns_sequencers.col_name] = sequencers[x]->GetName();
        row[m_columns_sequencers.col_apply_mainnote] = sequencers[x]->GetApplyMainNote();
        row[m_columns_sequencers.col_channel] = sequencers[x]->GetChannel();
        row[m_columns_sequencers.col_res] = sequencers[x]->resolution;
        row[m_columns_sequencers.col_pattern] = sequencers[x]->active_pattern;
        row[m_columns_sequencers.col_len] = sequencers[x]->length;
        row[m_columns_sequencers.col_vol] = sequencers[x]->GetVolume();
        Gtk::TreeRowReference rowref(m_refTreeModel_sequencers,m_refTreeModel_sequencers->get_path(iter));
        sequencers[x]->row_in_main_window = rowref;
    if(sequencers[x]->GetOn()){
        row[m_columns_sequencers.col_colour] = "green1";
    }else if (sequencers[x]->GetPlayOncePhase() == 2 || sequencers[x]->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = "yellow1";
    }else if(sequencers[x]->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = "yellow2";
    }else{
        row[m_columns_sequencers.col_colour] = "white";
    }
        rowcount++;

    }

}

void MainWindow::RefreshRow(Gtk::TreeRowReference rowref){

    *dbg << "Refreshing ROW\n";
    Gtk::TreeModel::Row row = *(m_refTreeModel_sequencers->get_iter(rowref.get_path()));
    int x = row[m_columns_sequencers.col_ID];
    Sequencer* seq = sequencers[x];
    row[m_columns_sequencers.col_muted] = seq->GetOn();
    row[m_columns_sequencers.col_name] = seq->GetName();
    row[m_columns_sequencers.col_apply_mainnote] = seq->GetApplyMainNote();
    row[m_columns_sequencers.col_channel] = seq->GetChannel();
    row[m_columns_sequencers.col_res] = seq->resolution;
    row[m_columns_sequencers.col_pattern] = seq->active_pattern;
    row[m_columns_sequencers.col_len] = seq->length;
    row[m_columns_sequencers.col_vol] = seq->GetVolume();
    if(sequencers[x]->GetOn()){
        row[m_columns_sequencers.col_colour] = "green1";
    }else if (sequencers[x]->GetPlayOncePhase() == 2 || sequencers[x]->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = "yellow1";
    }else if(sequencers[x]->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = "yellow2";
    }else{
        row[m_columns_sequencers.col_colour] = "white";
    }
  //  m_TreeView.queue_draw();
    
}

void MainWindow::OnRemoveClicked(){
    Gtk::TreeModel::iterator iter = *(m_TreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id = row[m_columns_sequencers.col_ID];
    *dbg << "removing row of id " << id << ENDL;
    
    m_refTreeModel_sequencers->erase(iter);
    //erasing smallers the treeview, but not the window.
    resize(2,2); //resizing to a tiny size, but the window won't get that small, it will be big enough to show all widgets.
    delete sequencers[id];
    sequencers[id] = NULL;

    eventswindow->InitTreeData();

    Files::SetFileModified(1);
}

void MainWindow::OnButtonAddClicked(){

    Gtk::TreeModel::RowReference rowref = spawn_sequencer();

    Gtk::TreeModel::iterator iter = rowref.get_model()->get_iter(rowref.get_path());
    m_TreeView.get_selection()->select(iter);

    Files::SetFileModified(1);
}

void MainWindow::OnCloneClicked(){
    Gtk::TreeModel::iterator iter = *(m_TreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id = row[m_columns_sequencers.col_ID];

    Gtk::TreeModel::RowReference rowref = clone_sequencer(id);
    iter = rowref.get_model()->get_iter(rowref.get_path());
    m_TreeView.get_selection()->select(iter);

   // FlashTempoStart(); wtf it is doing here?????

    Files::SetFileModified(1);
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
    std::map<int,Glib::ustring>::iterator iter;
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

void MainWindow::UpdatePlayPauseButton(){
    switch (midi->GetPaused()){
        case true:
            play_pause_button.set_label(_("Play"));
            play_pause_button.set_image(image_play);
            play_pause_button.get_image()->show();
            break;
        case false:
            play_pause_button.set_label(_("Pause"));
            play_pause_button.set_image(image_pause);
            play_pause_button.get_image()->show();
            break;
    }

}

void MainWindow::OnPauseButtonClicked(){
    
     switch (midi->GetPaused()){
        case true:
            midi->ContinueQueue();
            break;
        case false:
            midi->PauseQueueImmediately();
            break;
    }
}

void MainWindow::OnAboutMenuClicked(){
    Gtk::AboutDialog aboutbox;
    aboutbox.set_transient_for(*this);
    aboutbox.set_program_name("harmonySEQ");
    aboutbox.set_version(VERSION);
    aboutbox.set_logo(harmonySEQ_logo_48);
    aboutbox.set_copyright("Copyright © 2010 Rafał Cieślak");
    aboutbox.set_comments(_("A MIDI sequencing application helpful for music composers and live artists."));
    //TRANSLATORS:The GNU GPL v.3
     aboutbox.set_license(_("HarmonySEQ is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n"
                                                           "(at your option) any later version.\n\nHarmonySEQ is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A P"
                                                            "ARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>." ));
    aboutbox.set_website("http://harmonyseq.wordpress.com");
    aboutbox.set_website_label(_("harmonySEQ website"));
    std::vector<Glib::ustring> authors;
    //TRANSLATORS:First caption in authors list
    authors.push_back(_("Main author"));
    authors.push_back("       Rafał Cieślak <rafalcieslak256@gmail.com>");
    aboutbox.set_authors(authors);
    //TRANSLATORS: The list of translators to be placed in about-box
    aboutbox.set_translator_credits(_(" translator-credits"));
    
    aboutbox.run();

}

void MainWindow::OnMenuQuitClicked(){
        if(Files::file_modified)
            if(!Ask(_("The file has unsaved changes."),_("Are sure you want to quit?")))
                return;

        hide();
        running = 0;
}

void MainWindow::OnMenuNewClicked(){
    Files::file_name = "";
    

    //clear everything.
    ClearEvents();
    ClearSequencers();
    eventswindow->UpdateAll();
    InitTreeData();

    tempo = 120.0;
    tempo_button.set_value(120.0);
    mainnote = 60;
    main_note.set_value(60.0);

    resize(2,2);
    UpdateTitle();
    
    Files::SetFileModified(0);
}

void MainWindow::OnMenuOpenClicked(){
    if(Files::file_modified)
        if(!Ask(_("The file has unsaved changes."),_("Are sure you want to loose them and open another file?")))
        {
            return;
        }
    Files::LoadFileDialog();
    Files::SetFileModified(0);
    //erasing smallers the treeview, but not the window.
    resize(2,2); //resizing to a tiny size, but the window won't get that small, it will be big enough to show all widgets.
}

void MainWindow::OnMenuSaveClicked(){

    if (Files::file_name == "") //first save, so behave just if we were saving as
        OnMenuSaveAsClicked();

    Files::SaveToFile(Files::file_dir+Files::file_name);

}

void MainWindow::OnMenuSaveAsClicked(){

    *dbg << "saiving to file!\n";
    Gtk::FileChooserDialog dialog(_("Choose a file to save..."),Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*mainwindow);
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,Gtk::RESPONSE_OK);

    Gtk::FileFilter hseq;
    hseq.set_name("HarmonySEQ files (*.hseq)");
    hseq.add_pattern("*.hseq");
    dialog.add_filter(hseq);
    Gtk::FileFilter all;
    all.set_name("All files");
    all.add_pattern("*");
    dialog.add_filter(all);

    if (Files::file_name != "")
        dialog.set_filename(Files::file_dir+Files::file_name);
    
    int result = dialog.run();

    Glib::ustring filename = dialog.get_filename();
    char temp[300];
    
    switch (result) {
        case Gtk::RESPONSE_OK:

            //add .hseq extention
            if (dialog.get_filter() == &hseq) if (filename.size() < 5 || 0 != (filename.substr(filename.length() - 5, 5).compare(".hseq"))) {
                    filename += ".hseq";
              }
            //check whether it already exists
            if (Files::fexists(filename.c_str())) {
                sprintf(temp, _("File '%s'  already exist."), filename.c_str());
                if (!Ask(temp, _("Do you want to overwrite this file?")))
                    return; //user choosed not to overwrite it.
            }
            
            Files::SaveToFile(filename);

        case Gtk::RESPONSE_CANCEL:

            break;

        default:
            *dbg << "unknown response returned!\n";
            break;
    }
    
}