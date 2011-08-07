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

#include "MainWindow.h"
#include "messages.h"
#include "global.h"
#include "MidiDriver.h"
#include "Sequencer.h"
#include "Files.h"
#include "Event.h"
#include "TreeModels.h"
#include "config.h"
#include "SettingsWindow.h"
#include "Configuration.h"

bool CtrlKeyDown;
bool ShiftKeyDown;

Gtk::TreeModel::iterator row_inserted_by_drag;
bool seq_list_drag_in_progress;

MainWindow::MainWindow()
{
    set_border_width(0);
    //set_resizable(0);
    set_default_size(950,600);
    set_size_request(600,450); //minimum size
    //set_resizable(0);
    UpdateTitle();

    wTempoLabel.set_text(_("Tempo:"));
    add(wMainVBox);

    // <editor-fold defaultstate="collapsed" desc="menus, actions and toolbar creation and initialisation">
    m_refActionGroup = Gtk::ActionGroup::create();

    m_refActionGroup->add(Gtk::Action::create("MenuFile",_("File")));
    m_refActionGroup->add(Gtk::Action::create("MenuHelp",_( "Help")));
    m_refActionGroup->add(Gtk::Action::create("MenuTools",_( "Tools")));
    m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW,_("New"),_("Creates a new file.")), sigc::mem_fun(*this, &MainWindow::OnMenuNewClicked));
    m_refActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN,_("Open"),_("Opens a file.")), sigc::mem_fun(*this, &MainWindow::OnMenuOpenClicked));
    m_refActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE,_("Save"),_("Saves the current file.")), sigc::mem_fun(*this, &MainWindow::OnMenuSaveClicked));
    m_refActionGroup->add(Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS,_("Save as..."),_("Saves the current file with a different name.")), sigc::mem_fun(*this, &MainWindow::OnMenuSaveAsClicked));
    m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT,_("Quit"),_("Quits harmonySEQ.")), sigc::mem_fun(*this, &MainWindow::OnMenuQuitClicked));
    m_refActionGroup->add(Gtk::Action::create("Preferences", Gtk::Stock::PREFERENCES,_("Preferences"),_("harmonySEQ configuration.")), sigc::mem_fun(*this, &MainWindow::OnPreferencesClicked));
    m_refActionGroup->add(Gtk::Action::create("AddSeq", Gtk::Stock::ADD, _("Add"),_("Adds a new seqencer")), sigc::mem_fun(*this, &MainWindow::OnAddSeqClicked));
    m_refActionGroup->add(Gtk::Action::create("RemoveSeq", Gtk::Stock::REMOVE, _("Remove"),_("Removes selected sequencer")), sigc::mem_fun(*this, &MainWindow::OnRemoveClicked));
    m_refActionGroup->add(Gtk::Action::create("DuplicateSeq", Gtk::Stock::CONVERT, _("Duplicate"), _("Duplicates selected sequencer")), sigc::mem_fun(*this, &MainWindow::OnCloneClicked));
    m_refActionGroup->add(Gtk::Action::create("About", Gtk::Stock::ABOUT), sigc::mem_fun(*this, &MainWindow::OnAboutMenuClicked));
    m_refActionGroup->add(Gtk::ToggleAction::create("Metronome", _("Metronome"),_("Toggle metronome on/off")), sigc::mem_fun(*this, &MainWindow::OnMetronomeToggleClicked));
    m_refActionGroup->add(Gtk::Action::create("PlayPause", Gtk::Stock::MEDIA_PAUSE, _("Play/Pause"),_("Toggle play/pause")), sigc::mem_fun(*this, &MainWindow::OnPlayPauseClicked));
    m_refActionGroup->add(Gtk::ToggleAction::create("PassMidiEvents", _("Pass MIDI events"),_("States whether MIDI events are passed-through harmonySEQ.")), sigc::mem_fun(*this, &MainWindow::OnPassToggleClicked));
    m_refActionGroup->add(Gtk::ToggleAction::create("PlayOnEdit", _("Play on edit"),_("If on, harmonySEQ will play a brief preview of note, when it's added, or changed manually in chord..")), sigc::mem_fun(*this, &MainWindow::OnPlayOnEditClicked));
    m_refActionGroup->add(Gtk::Action::create("seq/PlayOnce", Gtk::Stock::MEDIA_NEXT, _("Play once"), _("Plays the sequence once.")), sigc::mem_fun(*this, &MainWindow::OnPopupPlayOnce));
    m_refActionGroup->add(Gtk::Action::create("seq/Remove", Gtk::Stock::REMOVE, _("Remove"), _("Removes the sequencer.")), sigc::mem_fun(*this, &MainWindow::OnPopupRemove));
    m_refActionGroup->add(Gtk::Action::create("seq/Duplicate", Gtk::Stock::CONVERT, _("Duplicate"), _("Duplicates the sequencer")), sigc::mem_fun(*this, &MainWindow::OnPopupDuplicate));

    m_refActionGroup->add(Gtk::Action::create("Empty"));

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
            "    <menu action='MenuTools'>"
            "      <menuitem action='PassMidiEvents'/>"
            "      <menuitem action='PlayOnEdit'/>"
            "      <separator/>"
            "      <menuitem action='Preferences'/>"
            "    </menu>"
            "    <menu action='MenuHelp'>"
            "      <menuitem action='About'/>"
            "    </menu>"
            "  </menubar>"

            "  <toolbar name='ToolBar'>"
            "   <toolitem action='FileNew'/>"
            "   <toolitem action='FileOpen'/>"
            "   <toolitem action='FileSave'/>"
            "   <toolitem action='FileSaveAs'/>"
            "   <separator/>"
            "   <toolitem action='AddSeq'/>"
            "   <toolitem name='RemoveTool' action='RemoveSeq'/>"
            "   <toolitem name='DuplicateTool' action='DuplicateSeq'/>"
            "   <separator expand='true'/>"
            "   <toolitem name='Metronome' action='Metronome'/>"
            "   <toolitem name='TempoLabel' action='Empty'/>"
            "   <toolitem name='Tempo' action='Empty'/>"
            "   <toolitem name='PlayPauseTool' action='PlayPause'/>"
            "  </toolbar>"

            "  <popup name='Popup'>"
            "   <menuitem action='seq/PlayOnce'/>"
            "   <separator/>"
            "   <menuitem action='seq/Duplicate'/>"
            "   <menuitem action='seq/Remove'/>"
            "  </popup>"
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

    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
    Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar");
    Gtk::Widget* pPopup = m_refUIManager->get_widget("/Popup");

    Gtk::Menu& Popup = dynamic_cast<Gtk::Menu&>(*pPopup);
    wPopupMenu = &Popup;
    wPopupMenu->accelerate(*this);

    Gtk::Toolbar& Toolbar = dynamic_cast<Gtk::Toolbar&> (*pToolbar);
    Toolbar.set_toolbar_style(Gtk::TOOLBAR_BOTH_HORIZ);
    Toolbar.set_border_width(0);

    Gtk::Widget* pRemoveTool = m_refUIManager->get_widget("/ToolBar/RemoveTool");
    pRemoveTool->set_sensitive(0);
    Gtk::Widget* pDuplicateTool = m_refUIManager->get_widget("/ToolBar/DuplicateTool");
    pDuplicateTool->set_sensitive(0);
    Gtk::Widget* pPlayPauseTool = m_refUIManager->get_widget("/ToolBar/PlayPauseTool");
    Gtk::ToolItem& PlayPauseTool = dynamic_cast<Gtk::ToolItem&> (*pPlayPauseTool);
    PlayPauseTool.set_is_important(1); // will display text text to the icon
    Gtk::Widget* pMetronome = m_refUIManager->get_widget("/ToolBar/Metronome");
    Gtk::ToggleToolButton& MetronomeTool = dynamic_cast<Gtk::ToggleToolButton&> (*pMetronome);
    MetronomeTool.set_active(metronome);
    Gtk::Widget* pTempo = m_refUIManager->get_widget("/ToolBar/Tempo");
    Gtk::ToolItem& TempoTool = dynamic_cast<Gtk::ToolItem&> (*pTempo);
    Gtk::Widget* pTempoLabelTool = m_refUIManager->get_widget("/ToolBar/TempoLabel");
    Gtk::ToolItem& TempoLabelTool = dynamic_cast<Gtk::ToolItem&> (*pTempoLabelTool);
    // </editor-fold>

    wMainVBox.pack_start(*pMenubar,Gtk::PACK_SHRINK);
    wMainVBox.pack_start(Toolbar,Gtk::PACK_SHRINK);
    wMainVBox.pack_start(wVBox1);
    wVBox1.pack_end(wFrame,Gtk::PACK_SHRINK);
    wFrame.add(wFrameNotebook);
    wFrame.set_border_width(1);
    wFrame.set_label(_("Sequencer properties"));

    wFrameNotebook.append_page(wNoSeqSelected);
    wFrameNotebook.append_page(seqWidget);
    wFrameNotebook.set_current_page(0);
    wFrameNotebook.set_show_tabs(0);
    wFrameNotebook.set_show_border(0);

    wNoSeqSelected.set_text(_("(No sequencer selected)"));
    wNoSeqSelected.set_tooltip_markup(_("<b>There is no sequencer selected</b>, so it's properties cannot be displayed.\n\nSelect one from the list above, or add a new one."));
    wNoSeqSelected.set_sensitive(0);

    TempoTool.remove();
    TempoTool.add(tempo_button);
    TempoTool.set_homogeneous(0);
    TempoLabelTool.remove();
    TempoLabelTool.add(wTempoLabel);
    TempoLabelTool.set_homogeneous(0);
    tempo_button.set_range(2, 1000);
    tempo_button.set_tooltip_markup(_("Sets the <b>tempo</b> applied to all sequencers.\nIt also blinks when a bar starts."));
    tempo_button.set_increments(1, 10);
    tempo_button.set_value(tempo);
    tempo_button.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::TempoChanged));

    UpdatePlayPauseTool();
    UpdatePassMidiToggle(); //sometimes we pass midi by default.
    UpdatePlayOnEditToggle(); //as above

    wScrolledWindow.add(wTreeView);
    wScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC); //The sliders should be shown only when needed
    wVBox1.pack_start(wHPaned); //will expand, no shrinking
    wHPaned.pack1(wScrolledWindow,1,1);
    wHPaned.pack2(eventsWidget,0,0);
    wHPaned.set_position(625);


    // <editor-fold defaultstate="collapsed" desc="tree">
    { //creating the tree model
        TreeModel_sequencers = Gtk::ListStore::create(m_columns_sequencers);

        wTreeView.append_column(_("Handle"), m_columns_sequencers.col_handle);
        
        int col_count = wTreeView.append_column_editable(_("Name"), m_columns_sequencers.col_name);
        Gtk::CellRenderer* cell = wTreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererText& txt = dynamic_cast<Gtk::CellRendererText&> (*cell);
        txt.signal_edited().connect(mem_fun(*this, &MainWindow::OnNameEdited));

        col_count = wTreeView.append_column_editable(_("On"), m_columns_sequencers.col_muted);
        Gtk::TreeViewColumn * column = wTreeView.get_column(col_count-1);
        cell = wTreeView.get_column_cell_renderer(col_count - 1);
        column->add_attribute(cell->property_cell_background(),m_columns_sequencers.col_colour);
        Gtk::CellRendererToggle& tgl = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl.signal_toggled().connect(mem_fun(*this, &MainWindow::OnMutedToggleToggled));
        
        col_count = wTreeView.append_column(_("Channel"), m_columns_sequencers.col_channel);
        col_count = wTreeView.append_column(_("Pattern"), m_columns_sequencers.col_pattern);
        col_count = wTreeView.append_column(_("Resolution"), m_columns_sequencers.col_res);
        col_count = wTreeView.append_column_numeric(_("Length"), m_columns_sequencers.col_len,"%g");
        col_count = wTreeView.append_column(_("Chord"), m_columns_sequencers.col_chord);


        Gtk::TreeView::Column* pColumn;
        int col_iter = 0;
        pColumn = wTreeView.get_column(col_iter);

        col_iter++;
        pColumn = wTreeView.get_column(col_iter);


        col_iter++;
        pColumn = wTreeView.get_column(col_iter);
        pColumn->set_fixed_width(10);

        //drag and drop enabling
        wTreeView.enable_model_drag_source();
        wTreeView.enable_model_drag_dest();

        wTreeView.signal_drag_begin().connect(sigc::mem_fun(*this, &MainWindow::OnTreeviewDragBegin));
        wTreeView.signal_drag_end().connect(sigc::mem_fun(*this, &MainWindow::OnTreeviewDragEnd));
        TreeModel_sequencers->signal_row_deleted().connect(sigc::mem_fun(*this, &MainWindow::OnTreeModelRowDeleted));
        TreeModel_sequencers->signal_row_inserted().connect(sigc::mem_fun(*this, &MainWindow::OnTreeModelRowInserted));

        //forbids to typesearch
        wTreeView.set_enable_search(0);

        //click signal (for popup)
        wTreeView.signal_button_press_event().connect(sigc::mem_fun(*this,&MainWindow::OnTreviewButtonPress), false);

        //react on selection change
        Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = wTreeView.get_selection();
        refTreeSelection->signal_changed().connect(mem_fun(*this,&MainWindow::OnSelectionChanged));

        wTreeView.set_model(TreeModel_sequencers);
        //initial data
        //InitTreeData();
        /*is called from main()*/

        UpdateVisibleColumns();

    }// </editor-fold>

    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect(mem_fun(*this,&MainWindow::OnKeyPress));
    signal_key_release_event().connect(mem_fun(*this,&MainWindow::OnKeyRelease));

    //icons settings
    if (harmonySEQ_logo_48 != NULL) set_icon(harmonySEQ_logo_48);
    if (metronome_icon_24 != NULL){
        metronometool_icon.set(metronome_icon_24);
        MetronomeTool.set_icon_widget(metronometool_icon);
    }

    show_all_children(1);
}

MainWindow::~MainWindow()
{

    delete wPopupMenu;
}
 
void MainWindow::UpdateTitle(){
    char temp[300];
    if (Files::file_name == ""){
        if (Files::file_modified)
           sprintf(temp, "harmonySEQ %s - %s [*]", VERSION,_("Untitled")) ;
        else
           sprintf(temp, "harmonySEQ %s - %s", VERSION,_("Untitled")) ;
    }else{
        if (Files::file_modified)
           sprintf(temp, "harmonySEQ %s - %s [*]", VERSION,Files::file_name.c_str()) ;
        else
            sprintf(temp, "harmonySEQ %s - %s", VERSION,Files::file_name.c_str());
    }
    set_title(temp);
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
/*
int MainWindow::GetSelectedSequencerID(){
     Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();
    if(!iter) return -1;
    Gtk::TreeModel::Row row = *iter;
    int id = row[m_columns_sequencers.col_ID];
    return id;

}

 */
seqHandle MainWindow::GetSelectedSequencerHandle(){
     Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();
    if(!iter) return -1;
    Gtk::TreeModel::Row row = *iter;
    return row[m_columns_sequencers.col_handle];
}

Gtk::TreeModel::iterator MainWindow::GetSelectedSequencerIter(){
    return *(wTreeView.get_selection())->get_selected();
}

void
MainWindow::OnMutedToggleToggled(const Glib::ustring& path)
{

    Gtk::TreeModel::iterator iter = TreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    seqHandle h = row[m_columns_sequencers.col_handle];

   seqH(h)->SetOn(!row[m_columns_sequencers.col_muted]);

    if(seqWidget.selectedSeq == h) seqWidget.UpdateOnOff();
   
   if(seqH(h)->my_row) RefreshRow(seqH(h)->my_row);

   //Files::SetFileModified(1); do not detect mutes
}

void
MainWindow::OnNameEdited(const Glib::ustring& path, const Glib::ustring& newtext)
{

    Gtk::TreeModel::iterator iter = TreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    seqHandle h = row[m_columns_sequencers.col_handle];
    seqH(h)->SetName(newtext);
   if(seqH(h)->my_row) RefreshRow(seqH(h)->my_row);

    if(seqWidget.selectedSeq == h) seqWidget.UpdateName();

    UpdateEventWidget();
    Files::SetFileModified(1);
}

Gtk::TreeModel::Row MainWindow::AddSequencerRow(int x)
{
    *dbg << "wooho! sequener " << x << " was just added, and we have to add it now to a new row in the model!" << ENDL;
    Gtk::TreeModel::iterator iter = TreeModel_sequencers->append();
    Gtk::TreeModel::Row row = *(iter);
    row[m_columns_sequencers.col_handle] = seqV(x)->MyHandle;
    row[m_columns_sequencers.col_name] = seqVector[x]->GetName();
    row[m_columns_sequencers.col_muted] = seqVector[x]->GetOn();
    row[m_columns_sequencers.col_channel] = seqVector[x]->GetChannel();
    row[m_columns_sequencers.col_pattern] = seqVector[x]->GetActivePatternNumber();
    row[m_columns_sequencers.col_res] = seqVector[x]->resolution;
    row[m_columns_sequencers.col_len] = seqVector[x]->GetLength();
    row[m_columns_sequencers.col_chord] = seqVector[x]->chord.GetName();
    if(seqVector[x]->GetOn()){
        row[m_columns_sequencers.col_colour] = "green1";
    }else if (seqVector[x]->GetPlayOncePhase() == 2 || seqVector[x]->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = "yellow1";
    }else if(seqVector[x]->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = "gold";
    }else{
        row[m_columns_sequencers.col_colour] = "white";
    }
    seqVector[x]->my_row = row;
    return row;
}



void MainWindow::InitTreeData(){
    *dbg << "loading initial data to the treeview\n";
    TreeModel_sequencers->clear();
    Gtk::TreeModel::Row row;
    int rowcount = 0;
    for (unsigned int x = 0; x < seqVector.size(); x++) {
        if (!seqV(x)) continue; //seems it was removed
        Gtk::TreeModel::iterator iter = TreeModel_sequencers->append();
        row = *(iter);
        row[m_columns_sequencers.col_handle] = seqV(x)->MyHandle;
        row[m_columns_sequencers.col_muted] = seqV(x)->GetOn();
        row[m_columns_sequencers.col_name] = seqV(x)->GetName();
        row[m_columns_sequencers.col_channel] = seqV(x)->GetChannel();
        row[m_columns_sequencers.col_res] = seqV(x)->resolution;
        row[m_columns_sequencers.col_pattern] = seqV(x)->GetActivePatternNumber();
        row[m_columns_sequencers.col_len] = seqV(x)->GetLength();
        row[m_columns_sequencers.col_chord] = seqV(x)->chord.GetName();
        seqV(x)->my_row = row;
    if(seqV(x)->GetOn()){
        row[m_columns_sequencers.col_colour] = "green1";
    }else if (seqV(x)->GetPlayOncePhase() == 2 || seqV(x)->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = "yellow1";
    }else if(seqV(x)->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = "gold";
    }else{
        row[m_columns_sequencers.col_colour] = "white";
    }
        rowcount++;

    }

}

void MainWindow::RefreshRow(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(TreeModel_sequencers->get_iter(rowref.get_path()));
    RefreshRow(row);
}

void MainWindow::RefreshRow(Gtk::TreeRow row){
    if(!row) return;
    
    //*dbg << "Refreshing ROW, the handle ";
    seqHandle h = row[m_columns_sequencers.col_handle];
    //*dbg << h << ENDL;
    Sequencer* seq = seqH(h);
    
    row[m_columns_sequencers.col_muted] = seq->GetOn();
    row[m_columns_sequencers.col_name] = seq->GetName();
    row[m_columns_sequencers.col_channel] = seq->GetChannel();
    row[m_columns_sequencers.col_res] = seq->resolution;
    row[m_columns_sequencers.col_pattern] = seq->GetActivePatternNumber();
    row[m_columns_sequencers.col_len] = seq->GetLength();
    row[m_columns_sequencers.col_chord] = seq->chord.GetName();
    if(seq->GetOn()){
        row[m_columns_sequencers.col_colour] = "green1";
    }else if (seq->GetPlayOncePhase() == 2 || seq->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = "yellow1";
    }else if(seq->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = "gold";
    }else{
        row[m_columns_sequencers.col_colour] = "white";
    }
}

void MainWindow::OnRemoveClicked(){
    seqHandle h  = GetSelectedSequencerHandle();
    int id = HandleToID(h);
    Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();
    *dbg << "removing row of handle " << h << " and ID " << id << ENDL;

    //removing the row
    seq_list_drag_in_progress = 0; //important
    TreeModel_sequencers->erase(iter);

    //and the corresponding sequencer
    delete seqVector[id];
    seqVector.erase(seqVector.begin()+id);

    //update hande map data:
    UpdateSeqHandlesAfterDeleting(id);

    eventsWidget.UpdateAll();

    Files::SetFileModified(1);
}

void MainWindow::OnAddSeqClicked(){

    seq_list_drag_in_progress = 0; //important
    Gtk::TreeModel::Row row = spawn_sequencer();
    
    wTreeView.get_selection()->select(row);

    Files::SetFileModified(1);
}

void MainWindow::OnCloneClicked(){
    Gtk::TreeModel::iterator iter = *(wTreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;
    int id = HandleToID(row[m_columns_sequencers.col_handle]);

    row = clone_sequencer(id);
    wTreeView.get_selection()->select(row);

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
    Gtk::Widget* pPassToggle = m_refUIManager->get_widget("/MenuBar/MenuTools/PassMidiEvents");
    Gtk::CheckMenuItem& PassToggle = dynamic_cast<Gtk::CheckMenuItem&> (*pPassToggle);
    passing_midi = PassToggle.get_active();
}

void MainWindow::OnPlayOnEditClicked(){
    Gtk::Widget* pPlayOnEdit = m_refUIManager->get_widget("/MenuBar/MenuTools/PlayOnEdit");
    Gtk::CheckMenuItem& PlayOnEdit = dynamic_cast<Gtk::CheckMenuItem&>(*pPlayOnEdit);
    Config::Interaction::PlayOnEdit = PlayOnEdit.get_active();
    Config::SaveToFile();
}

void MainWindow::UpdatePlayOnEditToggle(){
    Gtk::Widget* pPlayOnEdit = m_refUIManager->get_widget("/MenuBar/MenuTools/PlayOnEdit");
    Gtk::CheckMenuItem& PlayOnEdit = dynamic_cast<Gtk::CheckMenuItem&>(*pPlayOnEdit);
    PlayOnEdit.set_active(Config::Interaction::PlayOnEdit);
}

void MainWindow::UpdatePassMidiToggle(){
    Gtk::Widget* pPassToggle = m_refUIManager->get_widget("/MenuBar/MenuTools/PassMidiEvents");
    Gtk::CheckMenuItem& PassToggle = dynamic_cast<Gtk::CheckMenuItem&> (*pPassToggle);
    PassToggle.set_active(passing_midi);
}

bool MainWindow::OnKeyPress(GdkEventKey* event){
    //*dbg << "triggered " << event->keyval << "\n";
    std::map<int,Glib::ustring>::iterator iter;
    iter = keymap_itos.find(event->keyval);
    if(iter != keymap_itos.end()){
        *dbg << "Pressed key '" << iter->second << "'.\n";

    }else
        *dbg << "Unknown key pressed\n";

    if (event->keyval == 65507){ //Ctrl (left)
        if (!CtrlKeyDown) CtrlKeyDown = true;
    } else if (event->keyval == 65505){ // Shift(left)
        if (!ShiftKeyDown) ShiftKeyDown = true;
    }
    
    FindAndProcessEvents(Event::KEYBOARD,event->keyval);

    return 1;
}
bool MainWindow::OnKeyRelease(GdkEventKey* event){

    if (event->keyval == 65507){ //Ctrl (left)
        if (CtrlKeyDown) CtrlKeyDown = false;
    } else if (event->keyval == 65505){ // Shift(left)
        if (ShiftKeyDown) ShiftKeyDown = false;
    }
    return 1;
}
void MainWindow::OnSelectionChanged(){
    Gtk::TreeModel::iterator iter = wTreeView.get_selection()->get_selected();
    if(iter){
        //something is selected
        Gtk::Widget* pRemoveTool = m_refUIManager->get_widget("/ToolBar/RemoveTool");
        pRemoveTool->set_sensitive(1);
        Gtk::Widget* pDuplicateTool = m_refUIManager->get_widget("/ToolBar/DuplicateTool");
        pDuplicateTool->set_sensitive(1);

        seqWidget.SelectSeq((*iter)[m_columns_sequencers.col_handle]);
        wFrameNotebook.set_current_page(1);
    } else {
        //selection is empty
        Gtk::Widget* pRemoveTool = m_refUIManager->get_widget("/ToolBar/RemoveTool");
        pRemoveTool->set_sensitive(0);
        Gtk::Widget* pDuplicateTool = m_refUIManager->get_widget("/ToolBar/DuplicateTool");
        pDuplicateTool->set_sensitive(0);
        
        seqWidget.SelectNothing();
        wFrameNotebook.set_current_page(0);
    }

}

void MainWindow::UpdatePlayPauseTool(){
    Gtk::Widget* pPlayPauseTool = m_refUIManager->get_widget("/ToolBar/PlayPauseTool");
    Gtk::ToolButton& PlayPauseTool = dynamic_cast<Gtk::ToolButton&> (*pPlayPauseTool);
    switch (midi->GetPaused()){
        case true:
            PlayPauseTool.set_label(_("Play"));
            PlayPauseTool.set_stock_id(Gtk::Stock::MEDIA_PLAY);
            break;
        case false:
            PlayPauseTool.set_label(_("Pause"));
            PlayPauseTool.set_stock_id(Gtk::Stock::MEDIA_PAUSE);
            break;
    }

}

void MainWindow::OnPlayPauseClicked(){
    
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
    aboutbox.set_copyright("Copyright © 2010,2011 Rafał Cieślak");
    aboutbox.set_comments(_("A MIDI sequencing application helpful for music composers and live artists."));
    //TRANSLATORS:The GNU GPL v.3
     aboutbox.set_license(_("HarmonySEQ is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n"
                                                           "(at your option) any later version.\n\nHarmonySEQ is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A P"
                                                            "ARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>." ));
    aboutbox.set_website("http://harmonyseq.wordpress.com");
    aboutbox.set_website_label(_("harmonySEQ website"));
    std::vector<Glib::ustring> authors;
    //TRANSLATORS:First caption in authors list
    authors.push_back(_("Main author:"));
    authors.push_back("       Rafał Cieślak <rafalcieslak256@gmail.com>");
    authors.push_back("");
    authors.push_back(_("Consultants:"));
    authors.push_back("   Joanna Łopuch");
    authors.push_back("   Louigi Verona <http://www.louigiverona.ru>");
    authors.push_back("   Krzysztof Platis");
    authors.push_back("   Henia Netzel");
    aboutbox.set_authors(authors);
    //TRANSLATORS: The list of translators to be placed in about-box
    aboutbox.set_translator_credits(_("translator-credits"));
    
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

    if (Files::file_modified)
        if (!Ask(_("The file has unsaved changes."), _("Are sure you want to loose them?"))) {
            return;
        }

    Files::file_name = "";
    

    //clear everything.
    ClearEvents();
    ClearSequencers();
    ResetSeqHandles();
    UpdateEventWidget();
    InitTreeData();

    tempo = 120.0;
    tempo_button.set_value(120.0);

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
}

void MainWindow::OnMenuSaveClicked(){

    if (Files::file_name == ""){ //if it's the first save, behave just if we were saving as
        OnMenuSaveAsClicked();
        return;
    }

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

bool MainWindow::OnTreviewButtonPress(GdkEventButton* event){
   Gtk::TreePath path;
   wTreeView.get_path_at_pos(event->x,event->y,path);

  if (path != NULL) //right-clicked on a seq, not on the empty space
  if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
  {
    wPopupMenu->popup(event->button, event->time);
  }
   
  return false;
}

void MainWindow::OnPopupRemove(){
    //should do same action as remove tool
    OnRemoveClicked();
}

void MainWindow::OnPopupDuplicate(){
    //should do same action as duplicate tool
    OnCloneClicked();
}

void MainWindow::OnPopupPlayOnce(){
    seqH(GetSelectedSequencerHandle())->SetPlayOncePhase(1); //will be played once
    Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();
    RefreshRow(*iter);
    if (seqWidget.selectedSeq == GetSelectedSequencerHandle()) seqWidget.UpdateOnOffColour();

}

void MainWindow::OnPreferencesClicked(){
    settingswindow->present();
}

void MainWindow::UpdateVisibleColumns(){
        Gtk::TreeView::Column* pColumn;
        int col_iter = 0;
        pColumn = wTreeView.get_column(col_iter); //Handle
        pColumn->set_visible(debugging);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Name
        pColumn->set_visible(1);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //ON/OFF
        pColumn->set_visible(1);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Channel
        pColumn->set_visible(Config::VisibleColumns::Channel);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Pattern
        pColumn->set_visible(Config::VisibleColumns::Pattern);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Res
        pColumn->set_visible(Config::VisibleColumns::Resolution);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Len
        pColumn->set_visible(Config::VisibleColumns::Length);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Chord
        pColumn->set_visible(Config::VisibleColumns::Chord);
}

void MainWindow::OnMetronomeToggleClicked(){
    Gtk::Widget* pMetronome = m_refUIManager->get_widget("/ToolBar/Metronome");
    Gtk::ToggleToolButton& Metronome = dynamic_cast<Gtk::ToggleToolButton&> (*pMetronome);
    metronome = Metronome.get_active();

}

void MainWindow::OnTreeviewDragBegin(const Glib::RefPtr<Gdk::DragContext>& ct){
    seq_list_drag_in_progress= 1;

}

void MainWindow::OnTreeviewDragEnd(const Glib::RefPtr<Gdk::DragContext>& ct){
    seq_list_drag_in_progress = 0;

}

void MainWindow::OnTreeModelRowInserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
    if (seq_list_drag_in_progress == 1){
        //great! drag'n'drop inserted a row!
        //the point is that it first inserts a row, and then deletes it.
        row_inserted_by_drag  = iter;
    }
}

void MainWindow::OnTreeModelRowDeleted(const Gtk::TreeModel::Path& path){
    if (seq_list_drag_in_progress == 1){
       //great! drag'n'drop removed a row!

        //if a row was deleted, then we need to update the moved sequencer's row entry.
        int h = (*row_inserted_by_drag)[m_columns_sequencers.col_handle];
        seqH(h)->my_row = *row_inserted_by_drag;

        //also, we need to REORDER sequencers in the window
        //The ID of the moved sequencer
        int ID = HandleToID(h);
        //The position it was moved to:
        int ID2 = 0;
        //Here we compare the handles assigned to the inserted row, and the 0th row. If equal, it means it was inserted at the beggining, otherwise we cal calculate the position by counting id of above sequencer.
        if (h != (*TreeModel_sequencers->get_iter("0"))[m_columns_sequencers.col_handle]){
            //Get the id of sequencer with the row above
            row_inserted_by_drag--;
            ID2 = HandleToID ((*row_inserted_by_drag)[m_columns_sequencers.col_handle]);
            //The point in the line below, is the fact that when we move a sequencer downwards, the position isn't equal to the above's id, as one of the above (the one we moved) was removed.z
            if (ID > ID2) ID2++;
        }else{
            ID2 = 0;
        }
        *dbg << "Moved " << ID << "-" << ID2 << ENDL;

        //OK, now we know where from and to we moved a seq, we can switch the seq's in vector.
        if (ID == ID2) return;
        if (ID < ID2) //moved downwards
        {
            Sequencer* temp;
            temp = seqVector[ID];
            for (int i = ID; i <= ID2; i++){
                if (i != ID2) //not the last one, so copy from next
                    seqVector[i] = seqVector[i+1];
                else
                    seqVector[i] = temp;
            }

        }else{ //moved upwards
            Sequencer* temp;
            temp = seqVector[ID];
            for(int i = ID; i >= ID2; i--){
                if (i != ID2)//not the last one, so copy from prevoius
                    seqVector[i] = seqVector[i-1];
                else
                    seqVector[i] = temp;
            }

        }

        //Finally, update seqHandles
        UpdateSeqHandlesAfterMoving(ID,ID2);
        RefreshRow(seqH(h)->my_row);
    }
}

void MainWindow::OnSeqEdited(seqHandle h){
    RefreshRow(seqH(h)->my_row);
}

void MainWindow::UpdateEventWidget(){
    eventsWidget.UpdateAll();
}
