#ifndef TRACK_T_HPP
#define TRACK_T_HPP


#include <QString>


struct track_t{
    QString filepath, title, artist, genre, album;
    qint64 duration;
};


#endif
