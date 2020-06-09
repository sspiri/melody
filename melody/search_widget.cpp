#include "search_widget.hpp"
#include <QHBoxLayout>


search_widget::search_widget(QWidget* parent) : QWidget{parent}{
    set_layout();
}


void search_widget::set_layout(){
    auto* box = new QHBoxLayout{};
    box->addWidget(edit);
    setLayout(box);
}

