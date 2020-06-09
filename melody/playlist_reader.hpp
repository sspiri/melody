#ifndef PLAYLIST_READER_HPP
#define PLAYLIST_READER_HPP


#include <fstream>
#include "track_iterator.hpp"


class playlist_reader : public track_iterator{
public:
    std::vector<QString> filepaths;
    QString load_filepath;

    playlist_reader(const QString& name, QObject* parent)
        : track_iterator{parent}, load_filepath{name}{

        stream.exceptions(std::ios::badbit);
    }

    void construct_playlist(){
        filepaths.clear();
        std::string line;

        stream.open(load_filepath.toStdString());

        while(stream >> std::quoted(line))
            filepaths.push_back(line.c_str());

        stream.close();
    }

    void generate() override{
        for(const auto& filepath : filepaths){
            if(is_cancelled())
                throw quit_exception{};

            add_track(filepath);
        }
    }

private:
    std::ifstream stream;
};

#endif
