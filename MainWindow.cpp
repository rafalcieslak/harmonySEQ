
#include "MainWindow.h"
#include "debug.h"
#include "global.h"
#include "MidiDriver.h"
#include "Sequencer.h"
extern vector<Sequencer *> sequencers;
extern int mainnote;
extern debug* dbg;
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



        //initial data
        Gtk::TreeModel::Row row;
        int rowcount = 0;
        for (unsigned int x = 0; x < sequencers.size(); x++)
        {
            if (!sequencers[x]) continue; //seems it was removed
            row = *(m_refTreeModel->append());
            row[m_columns.col_ID] = x;
            row[m_columns.col_muted] = sequencers[x]->muted;
            row[m_columns.col_name] = sequencers[x]->name;
            sequencers[x]->row_in_main_window = rowcount;
            rowcount++;

        }

        m_TreeView.append_column(_("ID"), m_columns.col_ID);
        int col_count = m_TreeView.append_column_editable(_("Name"), m_columns.col_name);
        Gtk::CellRenderer* cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererText& txt = dynamic_cast<Gtk::CellRendererText&> (*cell);
        txt.signal_edited().connect(mem_fun(*this, &MainWindow::OnNameEdited));
        col_count = m_TreeView.append_column_editable(_("Muted"), m_columns.col_muted);
        cell = m_TreeView.get_column_cell_renderer(col_count - 1);
        Gtk::CellRendererToggle& tgl = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl.signal_toggled().connect(mem_fun(*this, &MainWindow::OnMutedToggleToggled));

        //Gtk::CellRenderer* cell = Gtk::manage(new Gtk::CellRendererToggle);
        // there may be something missing
        // like connecting the value with the toggle
        // see http://library.gnome.org/devel/gtkmm-tutorial/stable/sec-treeview-examples.html.en
        // if required

        Gtk::TreeView::Column* pColumn = m_TreeView.get_column(0);
        pColumn->set_sort_column(m_columns.col_ID);

        pColumn = m_TreeView.get_column(1);
        pColumn->set_sort_column(m_columns.col_name);

        pColumn = m_TreeView.get_column(2);
        pColumn->set_sort_column(m_columns.col_muted);

        //may cause segfaults, careful!
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
    }// </editor-fold>
    vbox1.pack_start(hbox_down, Gtk::PACK_SHRINK);
    hbox_down.pack_end(button_add, Gtk::PACK_SHRINK);
    button_add.set_label(_("Add"));
    button_add.signal_clicked().connect(mem_fun(*this, &MainWindow::OnButtonAddClicked));

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
            sequencers[row[m_columns.col_ID]]->gui_window->show();
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
    sequencers[row[m_columns.col_ID]]->muted = row[m_columns.col_muted];
}

void
MainWindow::OnNameEdited(const Glib::ustring& path, const Glib::ustring& newtext)
{

    Gtk::TreeModel::iterator iter = m_refTreeModel->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    sequencers[row[m_columns.col_ID]]->name = newtext;
    sequencers[row[m_columns.col_ID]]->gui_window->set_title(newtext);
}

void
MainWindow::SequencerAdded(int n)
{
    *dbg << "wooho! sequener " << n << " was just added, and we have to add it now to a new row in the list!" << ENDL;
    Gtk::TreeModel::iterator iter = m_refTreeModel->append();
    Gtk::TreeModel::Row row = *(iter);
    row[m_columns.col_ID] = n;
    row[m_columns.col_name] = sequencers[n]->name;
    row[m_columns.col_muted] = sequencers[n]->muted;
    Gtk::TreeModel::Path path(iter);
    sequencers[n]->row_in_main_window = atoi(path.to_string().c_str());

}

void MainWindow::OnButtonAddClicked(){
    spawn_sequencer();
    
}