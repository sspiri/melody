#include "player_widget.hpp"
#include "utility.hpp"

#include <QHBoxLayout>


player_widget::player_widget(QWidget* parent) : QWidget{parent}{
    slider->setRange(0, 1000);
    slider->setEnabled(false);

    slider_timer->setSingleShot(false);
    slider_timer->setInterval(100);

    duration_timer->setSingleShot(false);
    duration_timer->setInterval(250);

    set_layout();
    set_connections();
}


void player_widget::set_layout(){
    auto* box = new QHBoxLayout{};

    box->addWidget(pause_button);
    box->addWidget(stop_button);
    box->addWidget(slider);
    box->addWidget(duration);

    setLayout(box);
}


void player_widget::set_connections(){
    connect(pause_button, &QPushButton::clicked, this, &player_widget::pause);
    connect(stop_button, &QPushButton::clicked, this, &player_widget::stop);

    connect(slider_timer, &QTimer::timeout, [this]{
        slider->setValue(1000. / player->duration() * player->position());
    });

    connect(duration_timer, &QTimer::timeout, [this]{
        set_duration_string(player->position());
    });

    connect(player, &QMediaPlayer::stateChanged, this, &player_widget::media_status_changed);

    connect(slider, &QSlider::sliderPressed, [this]{
        slider_timer->stop();
        duration_timer->stop();
    });

    connect(slider, &QSlider::sliderReleased, [this]{
        slider_timer->start();
        duration_timer->start();

        seek();
    });

    connect(slider, &QSlider::sliderMoved, [this]{
        set_duration_string(slider->value() * player->duration() / 1000.);
    });
}


void player_widget::set_duration_string(qint64 value){
    value /= 1000;

    if(value < 10)
        duration->setText(("00:0" + std::to_string(value)).c_str());

    else if(value < 60)
        duration->setText(("00:" + std::to_string(value)).c_str());

    else{
        std::string s = human_readable_duration(std::chrono::seconds(value));
        duration->setText(s.c_str());
    }
}


void player_widget::play(){
    slider->setValue(0);
    slider->setEnabled(true);

    duration->setText("00:00");
    player->play();
}


void player_widget::pause(){
    player->pause();
}


void player_widget::media_status_changed(int value){
    switch(value){
    case QMediaPlayer::PlayingState:
        slider_timer->start();
        duration_timer->start();

        pause_button->setIcon(style.standardIcon(QStyle::SP_MediaPause));
        break;

    case QMediaPlayer::PausedState:
        slider_timer->stop();
        duration_timer->stop();

        pause_button->setIcon(style.standardIcon(QStyle::SP_MediaPlay));
        break;

    case QMediaPlayer::StoppedState:
        slider_timer->stop();
        duration_timer->stop();

        pause_button->setIcon(style.standardIcon(QStyle::SP_MediaPlay));

        slider->setValue(0);
        slider->setEnabled(false);

        duration->setText("--:--");
    }
}


void player_widget::stop(){
    player->stop();
}


void player_widget::seek(){
    player->setPosition(slider->value() * player->duration() / 1000.);
}
