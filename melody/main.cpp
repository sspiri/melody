#include <QApplication>
#include "main_window.hpp"


int main(int argc, char** argv){
    QApplication melody{argc, argv};

    main_window window;
    window.resize(1200, 800);
    window.show();

    return melody.exec();
}
