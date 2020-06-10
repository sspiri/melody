#include <QApplication>
#include "main_window.hpp"


int main(int argc, char** argv){
    QApplication melody{argc, argv};

    main_window window;
    window.showMaximized();
    window.show();

    return melody.exec();
}
