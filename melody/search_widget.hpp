#ifndef SEARCH_WIDGET_HPP
#define SEARCH_WIDGET_HPP


#include <QWidget>
#include <QLineEdit>
#include <QPushButton>


class search_widget : public QWidget{
public:
    enum class type_t : char{
        all, title, artist, genre, album
    };

    explicit search_widget(QWidget* parent);

    QLineEdit* edit{new QLineEdit{this}};

    std::pair<type_t, QString> get_search_options() const{
        QString s = edit->text();
        int pos = s.indexOf(':');

        if(pos < 0)
            return {type_t::all, s.trimmed()};

        QString left = s.mid(0, pos).trimmed();
        QString right = s.mid(++pos).trimmed();

        if(!left.compare("TITLE", Qt::CaseInsensitive))
            return {type_t::title, std::move(right)};

        if(!left.compare("ARTIST", Qt::CaseInsensitive))
            return {type_t::artist, std::move(right)};

        if(!left.compare("GENRE", Qt::CaseInsensitive))
            return {type_t::genre, std::move(right)};

        if(!left.compare("ALBUM", Qt::CaseInsensitive))
            return {type_t::album, std::move(right)};

        return {type_t::all, s.trimmed()};
    }

private:
    void set_layout();
};

#endif
