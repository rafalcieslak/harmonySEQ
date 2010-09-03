
#include "MainWindow.h"
#include "debug.h"
#include "error.h"
#include "global.h"
#include "MidiDriver.h"
#include "Sequencer.h"
extern vector<Sequencer *> sequencers;
extern int mainnote;
extern debug* dbg;
extern error* err;
extern int running;
extern int tempo;
extern MidiDriver* midi;

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

        m_TreeView.append_column(_("ID"), m_columns.col_ID);
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
        col_count = m_TreeView.append_column(_("Channel"), m_columns.col_channel);

        //Gtk::TreeView::Column* pColumn = m_TreeView.get_column(col_count-1);
        //pColumn->add_attribute(cell_spin->property_adjustment(),m_columns.col_channel);

        *dbg << "cell connection successful\n";

        /* sorting seems to crash strange things
        Gtk::TreeView::Column* pColumn = m_TreeView.get_column(0);
        pColumn->set_sort_column(m_columns.col_ID);

        pColumn = m_TreeView.get_column(1);
        pColumn->set_sort_column(m_columns.col_name);

        pColumn = m_TreeView.get_column(2);
        pColumn->set_sort_column(m_columns.col_muted);

         */
        //may cause segfaults, beware!
        for (guint i = 0; i < 3; i++)
        {
            Gtk::TreeView::Column* pColumn = m_TreeView.get_column(i);
            pColumn->set_reorderable();
        }

        //drag and drop enabling
        m_TreeView.enable_model_drag_source();
        m_TreeView.enable_model_drag_dest();

        //catching row selection signal
        m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this, &MainWindow::OnTreeviewRowActivated));


        *dbg << "loading initial data to the treeview/n";
        //initial data
        InitTreeData();


    }// </editor-fold>
    vbox1.pack_start(hbox_down, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_add, Gtk::PACK_SHRINK);
    button_add.set_label(_("Add"));
    button_add.signal_clicked().connect(mem_fun(*this, &MainWindow::OnButtonAddClicked));
    hbox_down.pack_end(button_save, Gtk::PACK_SHRINK);
    button_save.set_label(_("Save"));
    button_save.signal_clicked().connect(mem_fun(*this, &MainWindow::SaveToFile));
    hbox_down.pack_end(button_open, Gtk::PACK_SHRINK);
    button_open.set_label(_("Open"));
    button_open.signal_clicked().connect(mem_fun(*this, &MainWindow::LoadFromFile));

    show_all_children(1);


}

MainWindow::~MainWindow()
{

}

void
MainWindow::SetMainNote(int note)
{
    *dbg << "setting note\n";
    main_note.set_value(note);

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
}

void
MainWindow::SequencerAdded(int n)
{
    *dbg << "wooho! sequener " << n << " was just added, and we have to add it now to a new row in the list!" << ENDL;
    Gtk::TreeModel::iterator iter = m_refTreeModel->append();
    Gtk::TreeModel::Row row = *(iter);
    row[m_columns.col_ID] = n;
    row[m_columns.col_name] = sequencers[n]->GetName();
    row[m_columns.col_muted] = sequencers[n]->GetOn();
    row[m_columns.col_apply_mainnote] = sequencers[n]->GetApplyMainNote();
    row[m_columns.col_channel] = sequencers[n]->GetChannel();
    sequencers[n]->row_in_main_window = iter;

}

void MainWindow::OnButtonAddClicked(){
    spawn_sequencer();
    
}

void MainWindow::InitTreeData(){
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
        *dbg << "channel " << sequencers[x]->GetChannel() << ENDL;
        row[m_columns.col_channel] = sequencers[x]->GetChannel();
        sequencers[x]->row_in_main_window = iter;
        rowcount++;

    }

}

void MainWindow::RefreshRow(Gtk::TreeModel::iterator it){
    *dbg << "refreshing row" << ENDL;
    Gtk::TreeModel::Row row = *it;

    int x = row[m_columns.col_ID];
    row[m_columns.col_muted] = sequencers[x]->GetOn();
    row[m_columns.col_name] = sequencers[x]->GetName();
    row[m_columns.col_apply_mainnote] = sequencers[x]->GetApplyMainNote();
    row[m_columns.col_channel] = sequencers[x]->GetChannel();

}

void MainWindow::SaveToFile(){
    *dbg << "saiving to file!\n";
    Gtk::FileChooserDialog dialog(_("Choose a file to save..."),Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*this);
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,Gtk::RESPONSE_OK);

    Glib::KeyFile kf;

    int result = dialog.run();
    Glib::ustring filename = dialog.get_filename();
    char temp[300];
    ofstream output_file;

    switch (result){
        case Gtk::RESPONSE_OK:


            output_file.open(filename.c_str(),ios_base::trunc);
            if(!output_file.good()){
                sprintf(temp,_("ERROR - error while opening file %s to write.\n"),filename.c_str());
                *err << temp;

            }
            sprintf(temp,"harmonyseq file, generated by harmonySEQ %s. Manual editing not suggested, since the program does not check whether the data here is correct - may cause crashes.",VERSION);
            kf.set_comment(temp);
            kf.set_double(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_TEMPO,tempo);
            kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_MAINNOTE,mainnote);
            kf.set_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_SEQ_NUM,sequencers.size());

            for (unsigned int x = 0; x < sequencers.size(); x++){
                sprintf(temp,FILE_GROUP_TEMPLATE_SEQ,x);
                kf.set_string(temp,FILE_KEY_SEQ_NAME,sequencers[x]->GetName());
                kf.set_boolean(temp,FILE_KEY_SEQ_ON,sequencers[x]->GetOn());
                kf.set_integer(temp,FILE_KEY_SEQ_CHANNEL,sequencers[x]->GetChannel());
                kf.set_boolean(temp,FILE_KEY_SEQ_APPLY_MAIN_NOTE,sequencers[x]->GetApplyMainNote());
                kf.set_integer_list(temp,FILE_KEY_SEQ_SEQUENCE,sequencers[x]->sequence);
                kf.set_integer_list(temp,FILE_KEY_SEQ_NOTES,sequencers[x]->notes);

            }


            *dbg << kf.to_data();
            output_file << kf.to_data().c_str();

            output_file.close();
            break;
        case Gtk::RESPONSE_CANCEL:
            
            break;

        default:
            *dbg << "unknown response returned!\n";
        break;
    }

}

void MainWindow::LoadFromFile(){
    *dbg << "loading file!\n";
    Gtk::FileChooserDialog dialog(_("Choose a file to open..."),Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);
    dialog.add_button(Gtk::Stock::CANCEL,Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE,Gtk::RESPONSE_OK);

    Glib::KeyFile kf;

    int result = dialog.run();
    Glib::ustring filename = dialog.get_filename();
    char temp[100];
    int number;

    
    switch (result){
        case Gtk::RESPONSE_OK:
            if(!kf.load_from_file(filename)){
                sprintf(temp,_("ERROR - error while trying to read %s\n"),filename.c_str());
                *err << temp;
                break;
            }

            tempo = kf.get_double(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_TEMPO);
            mainnote = kf.get_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_MAINNOTE);
            number = kf.get_integer(FILE_GROUP_SYSTEM,FILE_KEY_SYSTEM_SEQ_NUM);

            sequencers.clear();//woa hua hua hua!

            for (int x = 0; x < number; x++){
                sprintf(temp,FILE_GROUP_TEMPLATE_SEQ,x);
                sequencers.push_back(new Sequencer());

                *dbg << "now loading data...\n";
                sequencers[x]->SetName(kf.get_string(temp,FILE_KEY_SEQ_NAME));
                sequencers[x]->SetOn(kf.get_boolean(temp,FILE_KEY_SEQ_ON));
                sequencers[x]->SetChannel(kf.get_integer(temp,FILE_KEY_SEQ_CHANNEL));
                sequencers[x]->SetApplyMainNote(kf.get_boolean(temp,FILE_KEY_SEQ_APPLY_MAIN_NOTE));

                sequencers[x]->sequence.clear();
                std::vector<int> sequence = kf.get_integer_list(temp,FILE_KEY_SEQ_SEQUENCE);
                for (unsigned int n = 0; n < sequence.size(); n++){
                    sequencers[x]->sequence.push_back(sequence[n]);

                }

                sequencers[x]->notes.clear();
                *dbg << "now loading notes...\n";
                std::vector<int> notes = kf.get_integer_list(temp,FILE_KEY_SEQ_NOTES);
                for (unsigned int n = 0; n < notes.size(); n++){
                    *dbg << notes[n] << ENDL;
                    sequencers[x]->notes.push_back(notes[n]);

                }
                sequencers[x]->UpdateGui();
            }

            InitTreeData();
            SetMainNote(mainnote);
            tempo_button.set_value(tempo);

            break;
        case Gtk::RESPONSE_CANCEL:

            break;

        default:
            *dbg << "unknown response returned!\n";
        break;



    }




}