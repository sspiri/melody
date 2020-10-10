#ifndef PLAYLIST_WRITER_HPP
#define PLAYLIST_WRITER_HPP


#include <fstream>

#include "utility.hpp"
#include "track_iterator.hpp"


class playlist_writer : public track_iterator{
public:
    QStringList filepaths;
    QString save_filepath;

    playlist_writer(const QString& name, const QStringList paths, QObject* parent)
        : track_iterator{parent}, filepaths{paths}, save_filepath{name}{

        stream.exceptions(std::ios::badbit);
    }

    void generate() override{
        stream.open(save_filepath.toStdString(), std::ios::app);
        walk();
        stream.close();
    }

private:
    std::ofstream stream;

    void walk(){
        for(const auto& filepath : filepaths){
            if(is_cancelled())
                throw quit_exception{};

            if(fs::is_regular_file(filepath.toStdString())){
                add_track(filepath);
                stream << std::quoted(filepath.toStdString()) << std::endl;
            }

            else if(fs::is_directory(filepath.toStdString()))
                walk_directory(filepath);
        }
    }

    void walk_directory(const QString& filepath){
        for(fs::recursive_directory_iterator it{filepath.toStdString(), fs::directory_options::skip_permission_denied}, end_it; it != end_it; ++it){
            if(is_cancelled())
                throw quit_exception{};

            if(fs::is_regular_file(it->path())){
                add_track(path2qstring(it->path().c_str()));
                stream << std::quoted(it->path().string()) << std::endl;
            }
        }
    }
};


#endif
