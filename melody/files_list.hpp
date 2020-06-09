#ifndef FILES_LIST_HPP
#define FILES_LIST_HPP


#include "search_widget.hpp"
#include "columns_list.hpp"
#include "track_t.hpp"

#include <QCommonStyle>
#include <QMediaPlaylist>


class files_list : public columns_list{
public slots:
    void hide_rows(const std::pair<search_widget::type_t, QString>& info);
    void add_track(const track_t& track);

    void remove_selected_tracks();
    void remove_tracks();

public:
    explicit files_list(QWidget* parent);

    std::ptrdiff_t index{-1};
    QMediaPlaylist* playlist{new QMediaPlaylist{this}};

private:
    QCommonStyle style;
};


#endif
