#ifndef NAME_DIALOG_HPP
#define NAME_DIALOG_HPP


#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>


class name_dialog : public QDialog{
public:
    explicit name_dialog(QWidget* parent) : QDialog{parent}{
        line->setMinimumWidth(300);

        set_connections();
        set_layout();
    }

    QString get_name() const{
        return line->text();
    }

private:
    QLineEdit* line{new QLineEdit{this}};

    QPushButton* ok{new QPushButton{"Ok", this}},
               * cancel{new QPushButton{"Cancel", this}};

    void set_connections(){
        connect(ok, &QPushButton::clicked, [this]{
            if(!line->text().size()){
                QMessageBox::critical(this, "Error", "The playlist name cannot be empty.");
                return;
            }

            if(line->text().indexOf('/') != -1){
                QMessageBox::critical(this, "Error", "The playlist name cannot contain slashes.");
                return;
            }

            accept();
        });

        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    }

    void set_layout(){
        auto* box = new QVBoxLayout{};
        auto* hbox = new QHBoxLayout{};

        hbox->addWidget(new QLabel{"Playlist Name:", this});
        hbox->addWidget(line);

        box->addLayout(hbox);
        hbox = new QHBoxLayout;

        hbox->addStretch();
        hbox->addWidget(ok);
        hbox->addWidget(cancel);

        box->addLayout(hbox);
        setLayout(box);
    }
};


#endif
