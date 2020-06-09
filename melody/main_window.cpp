#include "main_window.hpp"
#include "name_dialog.hpp"
#include "playlist_reader.hpp"
#include "playlist_writer.hpp"
#include "filesystem.hpp"
#include "utility.hpp"

#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QShortcut>
#include <QDir>


std::uintmax_t number_of_regular_files(const QStringList& filepaths){
    std::uintmax_t files_count{};

    for(const auto& fp : filepaths)
        files_count += number_of_files(fp.toStdString(), fs::directory_options::skip_permission_denied, fs::file_type::regular);

    return files_count;
}


main_window::main_window(){
    setCentralWidget(central);

    tabs->setMovable(true);
    tabs->setTabsClosable(true);

    if(settings.config_dir.exists())
        load_playlists();

    if(!tabs->count())
        create_tab("Playlist");

    set_layout();
    set_connections();
}


void main_window::set_layout(){
    auto* box = new QVBoxLayout{};

    auto* hbox = new QHBoxLayout{};

    hbox->addWidget(new QLabel{"Search:", this});
    hbox->addWidget(search);

    box->addLayout(hbox);
    box->addWidget(tabs);
    box->addWidget(player);

    central->setLayout(box);
}


void main_window::set_connections(){
    auto* bar = menuBar();
    auto* menu = bar->addMenu("File");

    connect(menu->addAction("Open playlist"), &QAction::triggered, this, &main_window::open_playlist);
    connect(menu->addAction("Create playlist"), &QAction::triggered, this, &main_window::create_playlist);
    connect(menu->addAction("Remove tracks"), &QAction::triggered, this, &main_window::remove_tracks);

    menu->addSeparator();

    connect(menu->addAction("Add library..."), &QAction::triggered, this, &main_window::browse_library);
    connect(menu->addAction("Add tracks..."), &QAction::triggered, this, &main_window::browse_tracks);

    menu->addSeparator();

    connect(menu->addAction("Quit"), &QAction::triggered, this, &QMainWindow::close);

    connect(player, &player_widget::error, this, &main_window::show_error_message);

    connect(new QShortcut{Qt::Key_Enter, this}, &QShortcut::activated, this, (void(main_window::*)())&main_window::play);
    connect(new QShortcut{Qt::Key_Space, this}, &QShortcut::activated, player, &player_widget::pause);

    connect(tabs, &QTabWidget::tabCloseRequested, this, &main_window::close_tab);

    connect(search->edit, &QLineEdit::textChanged, [this]{
        auto* list = (files_list*)tabs->currentWidget();

        if(list)
            list->hide_rows(search->get_search_options());
    });
}


void main_window::open_playlist(){
    QStringList music_location = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    QString filepath = QFileDialog::getOpenFileName(this, "Select file", music_location.size() > 0 ? music_location[0] : QDir::homePath());

    if(filepath.size()){
        auto* list = create_tab(QFileInfo{filepath}.fileName());
        tabs->setCurrentWidget(list);

        auto* reader = new playlist_reader{filepath, this};
        reader->construct_playlist();

        auto* progress = new progress_widget{reader, reader->filepaths.size()};
        iterators[reader] = progress;

        load_files(reader);
    }
}


void main_window::create_playlist(){
    auto* dialog = new name_dialog{this};

    if(dialog->exec() == QDialog::Accepted)
        create_tab(dialog->get_name());

    dialog->deleteLater();
}


void main_window::remove_tracks(){
    auto* list = (files_list*)tabs->currentWidget();

    if(list)
        list->remove_tracks();
}


files_list* main_window::create_tab(const QString& name){
    auto* list = new files_list{this};
    player->player->setPlaylist(list->playlist);

    tabs->addTab(list, name);
    tabs->setCurrentWidget(list);

    connect(list, &QTableWidget::cellDoubleClicked, this, (void(main_window::*)(int, int))&main_window::play);
    connect(new QShortcut{Qt::Key_Delete, this}, &QShortcut::activated, list, &files_list::remove_selected_tracks);

    return list;
}


void main_window::browse_tracks(){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        QStringList music_location = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        QStringList files = QFileDialog::getOpenFileNames(this, "Select one or more files", music_location.size() > 0 ? music_location[0] : QDir::homePath());

        if(files.size()){
            fs::create_directories(settings.config_dir.absolutePath().toStdString());

            QString tabname = tabs->tabText(tabs->currentIndex());
            QString save_filepath = settings.config_dir.absoluteFilePath(tabname);

            auto* thread = new playlist_writer{save_filepath, files, this};
            auto* progress = new progress_widget{thread, number_of_regular_files(files)};

            iterators[thread] = progress;
            load_files(thread);
        }
    }
}


void main_window::browse_library(){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        QStringList music_location = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        QString filepath = QFileDialog::getExistingDirectory(this, "Select directory", music_location.size() > 0 ? music_location[0] : QDir::homePath());

        if(filepath.size()){
            fs::create_directories(settings.config_dir.absolutePath().toStdString());

            QString tabname = tabs->tabText(tabs->currentIndex());
            QString save_filepath = settings.config_dir.absoluteFilePath(tabname);

            auto nfiles = number_of_files(filepath.toStdString(), fs::directory_options::skip_permission_denied, fs::file_type::regular);

            auto* thread = new playlist_writer{save_filepath, {filepath}, this};
            auto* progress = new progress_widget{thread, nfiles};

            iterators[thread] = progress;
            load_files(thread);
        }
    }
}


void main_window::load_files(track_iterator* thread){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        iterators[thread]->setAttribute(Qt::WA_DeleteOnClose);

        connect(thread, &playlist_writer::new_track, list, &files_list::add_track, Qt::BlockingQueuedConnection);
        connect(thread, &playlist_writer::new_track, iterators[thread], &progress_widget::process, Qt::BlockingQueuedConnection);
        connect(thread, &playlist_writer::error, iterators[thread]->error_output, &QPlainTextEdit::appendPlainText, Qt::BlockingQueuedConnection);

        connect(thread, &QThread::finished, [this, thread]{ iterators.erase(thread); });
        connect(thread, &QThread::finished, iterators[thread], &QWidget::close);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        iterators[thread]->show();
        thread->start();
    }
}


void main_window::play(){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        auto items = list->selectedItems();

        if(items.size()){
            player->player->setPlaylist(list->playlist);
            list->playlist->setCurrentIndex(list->row(items[0]));
            player->play();
        }
    }
}


void main_window::play(int row, int ){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        player->player->setPlaylist(list->playlist);
        list->playlist->setCurrentIndex(row);
        player->play();
    }
}


void main_window::show_error_message(const QString& message){
    QMessageBox::critical(this, "Error", message);
}


void main_window::load_playlists(){
    for(fs::directory_iterator it{settings.config_dir.absolutePath().toStdString()}, end_it; it != end_it; ++it){
        if(fs::is_regular_file(it->path())){
            auto* list = create_tab(it->path().filename().c_str());

            QString tabname = tabs->tabText(tabs->indexOf(list));
            QString filepath = settings.config_dir.absoluteFilePath(tabname);

            auto* reader = new playlist_reader{filepath, this};
            reader->construct_playlist();

            auto* progress = new progress_widget{reader, reader->filepaths.size()};
            iterators[reader] = progress;

            load_files(reader);
        }
    }
}


void main_window::close_tab(int index){
    QString name = tabs->tabText(index);

    int ret = QMessageBox::warning(this, "Warning", "Do you want to remove this playlist (" + name + ") from local storage?",
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if(ret == QMessageBox::Yes){
        fs::remove(settings.config_dir.absoluteFilePath(name).toStdString());
        tabs->removeTab(index);
    }

    else if(ret == QMessageBox::No)
        tabs->removeTab(index);
}


void main_window::cancel_all(){
    for(auto& it : iterators){
        if(it.first && it.first->isRunning())
            it.first->cancel();
    }
}
