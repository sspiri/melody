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
    tabs->setContextMenuPolicy(Qt::CustomContextMenu);

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

    connect(new QShortcut{Qt::Key_Return, this}, &QShortcut::activated, this, (void(main_window::*)())&main_window::play);
    connect(new QShortcut{Qt::Key_Space, this}, &QShortcut::activated, player, &player_widget::pause_resume);

    connect(player->pause_button, &QPushButton::clicked, [this]{
        if(player->player->state() == QMediaPlayer::StoppedState)
            play();
    });

    connect(tabs, &QTabWidget::tabCloseRequested, this, &main_window::close_tab);
    connect(tabs, &QTabWidget::customContextMenuRequested, this, &main_window::tab_context_menu);

    connect(player->player, &QMediaPlayer::mediaStatusChanged, this, &main_window::select_next_row);
    connect(search->edit, &QLineEdit::textChanged, this, &main_window::on_search);

    connect(new QShortcut{Qt::Key_Delete, this}, &QShortcut::activated, [this]{
        auto* list = (files_list*)tabs->currentWidget();

        if(list)
            list->remove_selected_tracks();
    });
}


void main_window::open_playlist(){
    QStringList music_location = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
    QString filepath = QFileDialog::getOpenFileName(this, "Select file", music_location.size() > 0 ? music_location[0] : QDir::homePath());

    if(filepath.size()){
        QString filename = QFileInfo{filepath}.fileName();
        files_list* list;

        if(filepath.endsWith(".PLAYLIST", Qt::CaseInsensitive))
            list = create_tab(filename.mid(0, filename.lastIndexOf('.')));

        else{
            list = new files_list{settings.config_dir.absoluteFilePath(filename), search, this};

            tabs->addTab(list, filename);
            tabs->setCurrentWidget(list);

            connect(list, &QTableView::doubleClicked, this, (void(main_window::*)(const QModelIndex& ))&main_window::play);
        }

        tabs->setCurrentWidget(list);

        auto* reader = new playlist_reader{filepath, this};
        reader->construct_playlist();

        auto* progress = new progress_widget{reader, reader->filepaths.size()};
        iterators[list] = {reader, progress};

        load_files(list);
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
    auto* list = new files_list{settings.config_dir.absoluteFilePath(name + ".playlist"), search, this};

    tabs->addTab(list, name);
    tabs->setCurrentWidget(list);

    connect(list, &QTableView::doubleClicked, this, (void(main_window::*)(const QModelIndex& ))&main_window::play);

    return list;
}


void main_window::browse_tracks(){
    auto* list = (files_list*)tabs->currentWidget();

    if(iterators.find(list) != iterators.end()){
        QMessageBox::critical(this, "Error", "Please wait while all tracks are fully loaded.");
        return;
    }

    if(list){
        QStringList music_location = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        QStringList files = QFileDialog::getOpenFileNames(this, "Select one or more files", music_location.size() > 0 ? music_location[0] : QDir::homePath());

        if(files.size()){
            fs::create_directories(settings.config_dir.absolutePath().toStdString());

            auto* thread = new playlist_writer{list->filepath, files, this};
            auto* progress = new progress_widget{thread, number_of_regular_files(files)};

            iterators[list] = {thread, progress};
            load_files(list);
        }
    }

    else
        QMessageBox::critical(this, "Error", "First select or create a playlist.");
}


void main_window::browse_library(){
    auto* list = (files_list*)tabs->currentWidget();

    if(iterators.find(list) != iterators.end()){
        QMessageBox::critical(this, "Error", "Please wait while all tracks are fully loaded.");
        return;
    }

    if(list){
        QStringList music_location = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
        QString filepath = QFileDialog::getExistingDirectory(this, "Select directory", music_location.size() > 0 ? music_location[0] : QDir::homePath());

        if(filepath.size()){
            fs::create_directories(settings.config_dir.absolutePath().toStdString());

            auto nfiles = number_of_files(filepath.toStdString(), fs::directory_options::skip_permission_denied, fs::file_type::regular);

            auto* thread = new playlist_writer{list->filepath, {filepath}, this};
            auto* progress = new progress_widget{thread, nfiles};

            iterators[list] = {thread, progress};
            load_files(list);
        }
    }

    else
        QMessageBox::critical(this, "Error", "First select or create a playlist.");
}


void main_window::load_files(files_list* list){
    iterators[list].progress->setAttribute(Qt::WA_DeleteOnClose);

    connect(iterators[list].iterator, &playlist_writer::new_track, list, &files_list::add_track, Qt::BlockingQueuedConnection);
    connect(iterators[list].iterator, &playlist_writer::new_track, iterators[list].progress, &progress_widget::process, Qt::BlockingQueuedConnection);
    connect(iterators[list].iterator, &playlist_writer::error, iterators[list].progress->error_output, &QPlainTextEdit::appendPlainText, Qt::BlockingQueuedConnection);

    connect(iterators[list].iterator, &QThread::finished, [this, list]{ iterators.erase(list); });
    connect(iterators[list].iterator, &QThread::finished, iterators[list].progress, &QWidget::close);
    connect(iterators[list].iterator, &QThread::finished, iterators[list].iterator, &QThread::deleteLater);

    iterators[list].progress->show();
    iterators[list].iterator->start();
}


void main_window::play(){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        auto items = list->selectionModel()->selectedRows();

        if(items.size()){
            player->player->setPlaylist(list->playlist);
            list->playlist->setCurrentIndex(list->sorted_model->mapToSource(items[0]).row());
            player->play();
        }
    }
}


void main_window::play(const QModelIndex& index){
    auto* list = (files_list*)tabs->currentWidget();

    if(list){
        player->player->setPlaylist(list->playlist);
        list->playlist->setCurrentIndex(list->sorted_model->mapToSource(index).row());
        player->play();
    }
}


void main_window::show_error_message(const QString& message){
    QMessageBox::critical(this, "Error", message);
}


void main_window::load_playlists(){
    for(fs::directory_iterator it{settings.config_dir.absolutePath().toStdString()}, end_it; it != end_it; ++it){
        QString filename{it->path().filename().c_str()};

        if(fs::is_regular_file(it->path()) && filename.endsWith(".PLAYLIST", Qt::CaseInsensitive)){
            auto* list = create_tab(filename.mid(0, filename.lastIndexOf('.')));

            auto* reader = new playlist_reader{list->filepath, this};
            reader->construct_playlist();

            auto* progress = new progress_widget{reader, reader->filepaths.size()};
            iterators[list] = {reader, progress};

            load_files(list);
        }
    }
}


void main_window::close_tab(int index){
    QString name = tabs->tabText(index);
    auto* list = (files_list*)tabs->widget(index);

    int ret = QMessageBox::warning(this, "Warning", "Do you want to remove this playlist (" + name + ") from local storage?",
                                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if(ret == QMessageBox::Yes){
        fs::remove(list->filepath.toStdString());

        if(iterators.find(list) != iterators.end())
            iterators[list].iterator->cancel();

        tabs->removeTab(index);
    }

    else if(ret == QMessageBox::No){
        if(iterators.find(list) != iterators.end())
            iterators[list].iterator->cancel();

        tabs->removeTab(index);
    }
}


void main_window::tab_context_menu(const QPoint& point){
    QMenu* menu = new QMenu{this};
    int index = tabs->tabBar()->tabAt(point);

    if(index > -1){
        connect(menu->addAction("Rename..."), &QAction::triggered, [this, index]{
            auto* dialog = new name_dialog{this};

            if(dialog->exec() == QDialog::Accepted){
                auto* list = (files_list*)tabs->widget(index);

                QString old_filepath = list->filepath;
                tabs->setTabText(index, dialog->get_name());
                settings.config_dir.rename(old_filepath, tabs->tabText(index) + ".playlist");
            }

            dialog->deleteLater();
        });

        connect(menu->addAction("Remove"), &QAction::triggered, [this, index]{
            close_tab(index);
        });

        menu->exec(QCursor::pos());
    }

    menu->deleteLater();
}


void main_window::on_search(){
    auto* list = (files_list*)tabs->currentWidget();

    if(list)
        list->hide_rows();
}


void main_window::select_next_row(int value){
    if(value == QMediaPlayer::EndOfMedia){
        for(int n{}; n < tabs->count(); ++n){
            auto* list = (files_list*)tabs->widget(n);

            if(list->playlist == player->player->playlist()){
                auto index = list->selectionModel()->currentIndex();

                do{
                    index = list->std_model->index(index.row() + 1, 0);
                } while(index.row() != -1 && list->isRowHidden(index.row()));

                if(index.row() == -1){
                    player->stop();
                    return;
                }

                list->selectRow(index.row());
                auto indexes = list->sorted_model->mapSelectionToSource(list->selectionModel()->selection()).indexes();

                if(indexes.size()){
                    list->playlist->setCurrentIndex(indexes[0].row());
                    player->play();
                }
            }
        }
    }
}


void main_window::cancel_all(){
    for(auto& it : iterators){
        if(it.second.iterator && it.second.iterator->isRunning())
            it.second.iterator->cancel();
    }
}
