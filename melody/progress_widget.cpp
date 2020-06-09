#include "progress_widget.hpp"

#include <QApplication>
#include <QVBoxLayout>


progress_widget::progress_widget(track_iterator* op, std::uintmax_t n)
    : nfiles{n}, operation{op}{

    resize(600, 500);

    success_output->setPlaceholderText("Success");
    error_output->setPlaceholderText("Errors");

    success_output->setWordWrapMode(QTextOption::NoWrap);
    success_output->setReadOnly(true);

    error_output->setWordWrapMode(QTextOption::NoWrap);
    error_output->setReadOnly(true);

    splitter->addWidget(success_output);
    splitter->addWidget(error_output);

    splitter->setOrientation(Qt::Vertical);
    splitter->setStretchFactor(0, 2);

    progress->setRange(0, 100);

    set_layout();
}


progress_widget::~progress_widget(){
    if(operation && operation->isRunning()){
        operation->cancel();

        do{
            QApplication::processEvents();
        } while(!operation->wait(0));
    }
}


void progress_widget::set_layout(){
    auto* box = new QVBoxLayout{};

    box->addWidget(splitter);
    box->addWidget(progress);

    setLayout(box);
}
