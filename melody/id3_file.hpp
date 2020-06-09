#ifndef COLLECTION_ID3_FILE_HPP
#define COLLECTION_ID3_FILE_HPP


#include <stdexcept>
#include <string>

#include <taglib/fileref.h>
#include <taglib/tpropertymap.h>
#include <taglib/tag.h>



namespace tl = TagLib;


class id3_file{
public:
    tl::PropertyMap properties;

    explicit id3_file(const std::string& fp)
        : fileref{fp.c_str(), true, tl::AudioProperties::Fast}{

        if(fileref.isNull())
            throw std::runtime_error{"Error: Cannot read audio properties: [" + fp + ']'};

        properties = fileref.tag()->properties();
    }

    const tl::AudioProperties& audio_properties() const{
        return *fileref.audioProperties();
    }

    void save(){
        if(properties != fileref.tag()->properties()){
            fileref.tag()->setProperties(properties);

            if(!fileref.save())
                throw std::runtime_error{'[' + std::string(fileref.file()->name()) + "]: Cannot open for writing"};
        }
    }

    void clear(){
        properties = {};
    }

    void reset(){
        properties = fileref.tag()->properties();
    }

private:
    tl::FileRef fileref;
};


#endif
