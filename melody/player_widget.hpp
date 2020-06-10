#ifndef PLAYER_WIDGET_HPP
#define PLAYER_WIDGET_HPP


#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCommonStyle>
#include <QTimer>
#include <QMediaPlayer>


class player_widget : public QWidget{
    Q_OBJECT

signals:
    void error(const QString& );
    void finished();

public:
    player_widget(QWidget* parent);

    QCommonStyle style;

    QMediaPlayer* player{new QMediaPlayer{this}};

    QPushButton* pause_button{new QPushButton{style.standardIcon(QStyle::SP_MediaPlay), "", this}},
               * stop_button{new QPushButton{style.standardIcon(QStyle::SP_MediaStop), "", this}};

    QSlider* duration_slider{new QSlider{Qt::Horizontal, this}},
           * volume_slider{new QSlider{Qt::Horizontal, this}};

    QLabel* duration{new QLabel{"--:--", this}},
          * volume{new QLabel{QString::number(player->volume())}};

public slots:
    void play();
    void pause_resume();
    void stop();

private slots:
    void media_status_changed(int value);
    void set_duration_string(qint64 value);
    void seek();

private:
    QTimer* slider_timer{new QTimer{this}},
          * duration_timer{new QTimer{this}};

    void set_layout();
    void set_connections();
};


#endif
