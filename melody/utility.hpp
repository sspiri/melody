#ifndef UTILITY_HPP
#define UTILITY_HPP


#include <sstream>
#include <deque>
#include <iomanip>
#include <chrono>

#include "filesystem.hpp"


struct quit_exception {};


template<typename T, typename Period>
inline std::string human_readable_duration(const std::chrono::duration<T, Period>& duration){
    T s = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    std::deque<T> times;

    times.push_back(s / 31536000);
    s -= times.back() * 31536000;

    times.push_back((s / 86400) % 24);
    s -= times.back() * 86400;

    times.push_back((s / 3600) % 60);
    s -= times.back() * 3600;

    times.push_back((s / 60) % 60);
    s -= times.back() * 60;

    times.push_back(s % 60);

    while(!times.front())
        times.pop_front();

    std::stringstream ss;

    for(auto&& time : times){
        if(ss.str().size())
            ss << ':';

        ss << std::setw(2) << std::setfill('0') << time;
    }

    return ss.str();
}


inline std::uintmax_t number_of_files(const std::filesystem::path& path, std::filesystem::directory_options options, std::filesystem::file_type type){
    namespace fs = std::filesystem;

    std::uintmax_t result{};

    if(fs::is_directory(path)){
        for(fs::recursive_directory_iterator it{path, options}, end_it; it != end_it; ++it){
            if(fs::status(it->path()).type() == type)
                ++result;
        }
    }

    return result + (fs::status(path).type() == type);
}


#endif
