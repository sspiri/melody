#include "files_list.hpp"
#include "filesystem.hpp"
#include "utility.hpp"


#include<iostream>


files_list::files_list(search_widget* s, QWidget* parent)
    : columns_list{{"Title", "Artist", "Genre", "Album", "Duration"}, parent}, search{s} {}


void files_list::add_track(const track_t& track){
    setSortingEnabled(false);
    setRowCount(rowCount() + 1);

    std::string title = track.title.toStdString();

    if(!title.size()){
        title = fs::path{track.filepath.toStdString()}.filename().string();
        auto pos = title.rfind('.');

        if(pos != title.npos)
            title.erase(title.begin() + pos, title.end());
    }

    auto* item = new QTableWidgetItem{style.standardIcon(QStyle::SP_MediaPlay), title.c_str()};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setData(Qt::UserRole, rowCount() - 1);
    setItem(rowCount() - 1, 0, item);

    item = new QTableWidgetItem{track.artist};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setData(Qt::UserRole, rowCount() - 1);
    setItem(rowCount() - 1, 1, item);


    item = new QTableWidgetItem{track.genre};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setData(Qt::UserRole, rowCount() - 1);
    setItem(rowCount() - 1, 2, item);

    item = new QTableWidgetItem{track.album};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setData(Qt::UserRole, rowCount() - 1);
    setItem(rowCount() - 1, 3, item);

    std::string duration_string;

    if(track.duration < 10000)
        duration_string = "00:0" + std::to_string(track.duration / 1000);
    else if(track.duration < 60000)
        duration_string = "00:" + std::to_string(track.duration / 1000);
    else
        duration_string = human_readable_duration(std::chrono::milliseconds(track.duration));

    item = new QTableWidgetItem{duration_string.c_str()};
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    setItem(rowCount() - 1, 4, item);

    resizeColumnsToContents();
    hide_row(rowCount() - 1);

    playlist->addMedia(QUrl::fromLocalFile(track.filepath));
    setSortingEnabled(true);
}


void files_list::remove_selected_tracks(){
    auto tracks = selectedItems();

    if(tracks.size()){
        for(const auto* track : tracks){
            playlist->removeMedia(row(track));
            removeRow(row(track));
        }
    }
}


void files_list::remove_tracks(){
    playlist->clear();
    setRowCount(0);
    index = -1;
}


void files_list::hide_row(int row){
    auto info = search->get_search_options();

    switch(info.first){
    case search_widget::type_t::all:
        if(item(row, 0)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
           && item(row, 1)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
           && item(row, 2)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
           && item(row, 3)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0){

            hideRow(row);
        }

        else
            showRow(row);
        break;

    case search_widget::type_t::title:
        if(item(row, 0)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(row);
        else
            showRow(row);
        break;

    case search_widget::type_t::artist:
        if(item(row, 1)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(row);
        else
            showRow(row);
        break;

    case search_widget::type_t::genre:
        if(item(row, 2)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(row);
        else
            showRow(row);
        break;

    case search_widget::type_t::album:
        if(item(row, 3)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
            hideRow(row);
        else
            showRow(row);
        break;
    }
}


void files_list::hide_rows(const search_widget::info_t& info){
    switch(info.first){
    case search_widget::type_t::all:
        for(int n{}; n < rowCount(); ++n){
            if(item(n, 0)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
               && item(n, 1)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
               && item(n, 2)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0
               && item(n, 3)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0){

                hideRow(n);
            }

            else
                showRow(n);
        }
        break;

    case search_widget::type_t::title:
        for(int n{}; n < rowCount(); ++n){
            if(item(n, 0)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(n);
            else
                showRow(n);
        }
        break;

    case search_widget::type_t::artist:
        for(int n{}; n < rowCount(); ++n){
            if(item(n, 1)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(n);
            else
                showRow(n);
        }
        break;

    case search_widget::type_t::genre:
        for(int n{}; n < rowCount(); ++n){
            if(item(n, 2)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(n);
            else
                showRow(n);
        }
        break;

    case search_widget::type_t::album:
        for(int n{}; n < rowCount(); ++n){
            if(item(n, 3)->text().indexOf(info.second, 0, Qt::CaseInsensitive) < 0)
                hideRow(n);
            else
                showRow(n);
        }
        break;
    }
}
