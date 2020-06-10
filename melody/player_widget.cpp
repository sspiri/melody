#include "player_widget.hpp"
#include "utility.hpp"

#include <QHBoxLayout>


player_widget::player_widget(QWidget* parent) : QWidget{parent}{
    duration_slider->setRange(0, 1000);
    duration_slider->setEnabled(false);

    volume_slider->setRange(0, 100);
    volume_slider->setValue(player->volume());
    volume_slider->setMaximumWidth(100);

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
    box->addWidget(duration_slider);
    box->addWidget(duration);
    box->addWidget(volume_slider);
    box->addWidget(volume);

    setLayout(box);
}


void player_widget::set_connections(){
    connect(pause_button, &QPushButton::clicked, this, &player_widget::pause_resume);
    connect(stop_button, &QPushButton::clicked, this, &player_widget::stop);

    connect(slider_timer, &QTimer::timeout, [this]{
        duration_slider->setValue(1000. / player->duration() * player->position());
    });

    connect(duration_timer, &QTimer::timeout, [this]{
        set_duration_string(player->position());
    });

    connect(player, &QMediaPlayer::stateChanged, this, &player_widget::media_status_changed);

    connect(duration_slider, &QSlider::sliderPressed, [this]{
        slider_timer->stop();
        duration_timer->stop();
    });

    connect(duration_slider, &QSlider::sliderReleased, [this]{
        slider_timer->start();
        duration_timer->start();

        seek();
    });

    connect(duration_slider, &QSlider::sliderMoved, [this]{
        set_duration_string(duration_slider->value() * player->duration() / 1000.);
    });

    connect(volume_slider, &QSlider::sliderReleased, [this]{
        player->setVolume(volume_slider->value());
        volume->setText(QString::number(volume_slider->value()));
    });

    connect(volume_slider, &QSlider::sliderMoved, [this]{
        player->setVolume(volume_slider->value());
        volume->setText(QString::number(volume_slider->value()));
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
    duration_slider->setValue(0);
    duration_slider->setEnabled(true);

    duration->setText("00:00");
    player->play();
}


void player_widget::pause_resume(){
    if(player->state() == QMediaPlayer::PlayingState)
        player->pause();

    else if(player->state() == QMediaPlayer::PausedState)
        player->play();
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

        duration_slider->setValue(0);
        duration_slider->setEnabled(false);

        duration->setText("--:--");
    }
}


void player_widget::stop(){
    player->stop();
}


void player_widget::seek(){
    player->setPosition(duration_slider->value() * player->duration() / 1000.);
}
