#ifndef TRACK_ITERATOR_HPP
#define TRACK_ITERATOR_HPP


#include <QThread>

#include "id3_file.hpp"
#include "track_t.hpp"
#include "settings_t.hpp"
#include "utility.hpp"


class track_iterator : public QThread{
    Q_OBJECT

signals:
    void error(const QString& );
    void new_track(const track_t& );

public:
    explicit track_iterator(QObject* parent)
        : QThread{parent} {}

    virtual ~track_iterator() {}

    void add_track(const QString& filepath);

    void cancel(){
        cancel_op = true;
    }

    bool is_cancelled() const{
        return cancel_op;
    }

    virtual void generate() = 0;

private:
    bool cancel_op{};

    void run() override;
};


#endif
