#include <fstream>
#include <set>

#include <QStandardItem>

#include "files_list.hpp"
#include "filesystem.hpp"
#include "utility.hpp"


files_list::files_list(const QString& fp, search_widget* s, QWidget* parent)
    : columns_list{{"Title", "Artist", "Genre", "Album", "Duration", "Location"}, parent}, filepath{fp}, search{s} {}


void files_list::add_track(const track_t& track){
    setSortingEnabled(false);

    std::string title = track.title.toStdString();

    if(!title.size()){
        title = fs::path{track.filepath.toStdString()}.filename().string();
        auto pos = title.rfind('.');

        if(pos != title.npos)
            title.erase(title.begin() + pos, title.end());
    }

    QList<QStandardItem*> row;

    auto* item = new QStandardItem{style.standardIcon(QStyle::SP_MediaPlay), title.c_str()};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    row.push_back(item);

    item = new QStandardItem{track.artist};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    row.push_back(item);

    item = new QStandardItem{track.genre};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    row.push_back(item);

    item = new QStandardItem{track.album};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    row.push_back(item);

    std::string duration_string;

    if(track.duration < 10000)
        duration_string = "00:0" + std::to_string(track.duration / 1000);
    else if(track.duration < 60000)
        duration_string = "00:" + std::to_string(track.duration / 1000);
    else
        duration_string = human_readable_duration(std::chrono::milliseconds(track.duration));

    item = new QStandardItem{duration_string.c_str()};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    row.push_back(item);

    item = new QStandardItem{track.filepath};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    row.push_back(item);

    std_model->appendRow(row);

    if(search->get_search_options() == search_widget::info_t{search_widget::type_t::all, ""})
        resizeColumnsToContents();

    hide_row(std_model->rowCount() - 1);

    playlist->addMedia(QUrl::fromLocalFile(track.filepath));
    setSortingEnabled(true);
}


void files_list::remove_selected_tracks(){
    std::set<QModelIndex> playlist_rows, item_rows;

    for(const auto& index : sorted_model->mapSelectionToSource(selectionModel()->selection()).indexes()){
        if(!index.column())
            playlist_rows.insert(index);
    }

    for(auto it = playlist_rows.rbegin(); it != playlist_rows.rend(); ++it){
        playlist->removeMedia(it->row());
        sorted_model->removeRow(sorted_model->mapFromSource(*it).row());
    }

    save();
}


void files_list::remove_tracks(){
    playlist->clear();
    std_model->setRowCount(0);
}


void files_list::save(){
    std::ofstream stream{filepath.toStdString()};

    if(stream.is_open()){
        for(int n{}; n < std_model->rowCount(); ++n)
            stream << std::quoted(std_model->data(std_model->index(n, 5)).toString().toStdString()) << std::endl;
    }
}


void files_list::hide_row(int row){
    QModelIndex index = sorted_model->index(row, 0);
    QString text = index.data().toString();

    auto info = search->get_search_options();

    switch(info.first){
    case search_widget::type_t::all:
        if(text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0
           && text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0
           && text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0
           && text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0){

            hideRow(index.row());
        }

        else
            showRow(index.row());
        break;

    case search_widget::type_t::title:
        if(text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(index.row());
        else
            showRow(index.row());
        break;

    case search_widget::type_t::artist:
        if(text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(index.row());
        else
            showRow(index.row());
        break;

    case search_widget::type_t::genre:
        if(text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(index.row());
        else
            showRow(index.row());
        break;

    case search_widget::type_t::album:
        if(text.indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(index.row());
        else
            showRow(index.row());
        break;
    }
}


void files_list::hide_rows(){
    auto info = search->get_search_options();

    switch(info.first){
    case search_widget::type_t::all:
        for(int n{}; n < sorted_model->rowCount(); ++n){
            if(sorted_model->index(n, 0).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
               && sorted_model->index(n, 1).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
               && sorted_model->index(n, 2).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
               && sorted_model->index(n, 3).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0){

                hideRow(sorted_model->index(n, 1).row());
            }

            else
                showRow(sorted_model->index(n, 1).row());
        }
        break;

    case search_widget::type_t::title:
        for(int n{}; n < sorted_model->rowCount(); ++n){
            if(sorted_model->index(n, 0).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(sorted_model->index(n, 1).row());
            else
                showRow(sorted_model->index(n, 1).row());
        }
        break;

    case search_widget::type_t::artist:
        for(int n{}; n < sorted_model->rowCount(); ++n){
            if(sorted_model->index(n, 1).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(sorted_model->index(n, 1).row());
            else
                showRow(sorted_model->index(n, 1).row());
        }
        break;

    case search_widget::type_t::genre:
        for(int n{}; n < sorted_model->rowCount(); ++n){
            if(sorted_model->index(n, 2).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(sorted_model->index(n, 1).row());
            else
                showRow(sorted_model->index(n, 1).row());
        }
        break;

    case search_widget::type_t::album:
        for(int n{}; n < sorted_model->rowCount(); ++n){
            if(sorted_model->index(n, 3).data().toString().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(sorted_model->index(n, 1).row());
            else
                showRow(sorted_model->index(n, 1).row());
        }
        break;
    }
}
