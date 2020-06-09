#ifndef PROGRESS_WIDGET_HPP
#define PROGRESS_WIDGET_HPP


#include <QProgressBar>
#include <QPlainTextEdit>
#include <QSplitter>

#include "track_iterator.hpp"


class progress_widget : public QWidget{
public slots:
    void process(const track_t& track){
        assert(nth < nfiles);
        progress->setValue(100. / nfiles * ++nth);
        success_output->appendPlainText(track.filepath);
    }

public:
    progress_widget(track_iterator* op, std::uintmax_t n);

    ~progress_widget();

    QSplitter* splitter{new QSplitter{this}};

    QPlainTextEdit* success_output{new QPlainTextEdit{this}},
                  * error_output{new QPlainTextEdit{this}};

    QProgressBar* progress{new QProgressBar{this}};


private:
    std::size_t nfiles, nth{};
    track_iterator* operation;

    void set_layout();
};


#endif
