QT += quick widgets multimedia
CONFIG += c++17
unix:QMAKE_CXXFLAGS += -std=c++17
unix:LIBS += -ltag -lstdc++fs

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        files_list.cpp \
        main.cpp \
        main_window.cpp \
        player_widget.cpp \
        progress_widget.cpp \
        search_widget.cpp \
        track_iterator.cpp

RESOURCES +=

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    columns_list.hpp \
    files_list.hpp \
    filesystem.hpp \
    id3_file.hpp \
    main_window.hpp \
    name_dialog.hpp \
    player_widget.hpp \
    playlist_reader.hpp \
    playlist_writer.hpp \
    progress_widget.hpp \
    search_widget.hpp \
    settings_t.hpp \
    track_iterator.hpp \
    track_t.hpp \
    utility.hpp

win32:DEFINES += "TAGLIB_STATIC"
win32:LIBS += -L$$PWD/../../../taglib/lib/ -ltag

win32:INCLUDEPATH += $$PWD/../../../taglib/include
win32:DEPENDPATH += $$PWD/../../../taglib/include
