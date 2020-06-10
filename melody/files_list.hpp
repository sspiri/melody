#ifndef FILES_LIST_HPP
#define FILES_LIST_HPP


#include "search_widget.hpp"
#include "columns_list.hpp"
#include "track_t.hpp"

#include <QCommonStyle>
#include <QMediaPlaylist>


class files_list : public columns_list{
public slots:
    void hide_rows(const search_widget::info_t& info);
    void add_track(const track_t& track);

    void remove_selected_tracks();
    void remove_tracks();

public:
    files_list(search_widget* s, QWidget* parent);

    std::ptrdiff_t index{-1};
    QMediaPlaylist* playlist{new QMediaPlaylist{this}};

private:
    QCommonStyle style;
    search_widget* search;

    void hide_row(int row);
};


#endif
