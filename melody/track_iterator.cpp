#include "track_iterator.hpp"
#include <cmath>



void track_iterator::run(){
    try{
        generate();
    }

    catch(const quit_exception& ) {}

    catch(const std::exception& err){
        emit error(err.what());
    }
}


void track_iterator::add_track(const QString& filepath){
    try{
        id3_file file{filepath.toStdString()};
        track_t track;

        track.filepath = filepath;
        track.duration = (qint64)std::ceil(file.audio_properties().lengthInMilliseconds());
        track.artist = file.properties["ARTIST"].toString("; ").toCString();
        track.title = file.properties["TITLE"].toString("; ").toCString();
        track.genre = file.properties["GENRE"].toString("; ").toCString();
        track.album = file.properties["ALBUM"].toString("; ").toCString();

        emit new_track(track);
    }

    catch(const std::exception& err){
        emit error(err.what());
    }
}
