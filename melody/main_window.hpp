#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP


#include <unordered_map>
#include <QMainWindow>
#include <QCloseEvent>

#include "search_widget.hpp"
#include "files_list.hpp"
#include "player_widget.hpp"
#include "progress_widget.hpp"
#include "settings_t.hpp"


class main_window : public QMainWindow{
public:
    main_window();

protected:
    void closeEvent(QCloseEvent* event) override{
        if(event->isAccepted()){
            cancel_all();
            QMainWindow::closeEvent(event);
        }

        else
            QMainWindow::closeEvent(event);
    }

private slots:
    void open_playlist();
    void create_playlist();

    void remove_selected_tracks();
    void remove_tracks();

    void browse_library();
    void browse_tracks();

    void play();
    void play_next();
    void play_previous();
    void play(const QModelIndex& index);

    void select_next_row(unsigned step);

    void close_tab(int index);
    void tab_context_menu(const QPoint& point);

    void on_search();

    void show_error_message(const QString& message);

private:
    struct iter_pair{
        track_iterator* iterator;
        progress_widget* progress;
    };

    settings_t settings;

    QWidget* central{new QWidget{this}};
    search_widget* search{new search_widget{this}};
    QTabWidget* tabs{new QTabWidget{this}};
    player_widget* player{new player_widget{this}};

    std::unordered_map<files_list*, iter_pair> iterators{};

    void set_layout();
    void set_connections();

    files_list* create_tab(const QString& name);

    void load_files(files_list* list);
    void cancel_all();

    void load_playlists();
};


#endif
