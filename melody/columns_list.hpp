#ifndef QT_COLLECTION_COLUMNS_LIST_HPP
#define QT_COLLECTION_COLUMNS_LIST_HPP


#include <functional>

#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>


class columns_list : public QTableWidget{
public:
    columns_list(QWidget* parent = nullptr) : QTableWidget{parent}{
        setup_columns_list({});
    }

    columns_list(const QStringList& labels, QWidget* parent = nullptr)
        : QTableWidget{0, labels.size(), parent}{

        setup_columns_list(labels);
    }

    columns_list(int rows, const QStringList& labels, QWidget* parent = nullptr)
        : QTableWidget{rows, labels.size(), parent}{

        setup_columns_list(labels);
    }

    virtual ~columns_list() {}

private slots:
    void open_context_menu(const QPoint& ){
        menu->exec(QCursor::pos());
    }

    int get_index_by_label(const QString& label){
        for(int n{}; n < horizontalHeader()->count(); ++n){
            if(horizontalHeaderItem(n)->text() == label)
                return n;
        }

        return -1;
    }

private:
    QMenu* menu{new QMenu{this}};

    void setup_context_menu(){
        auto create_action = [this](const QString& label){
            auto* action = menu->addAction("Show " + label);
            action->setCheckable(true);
            action->setChecked(true);

            connect(action, &QAction::triggered, [this, label, action]{
                if(action->isChecked())
                    horizontalHeader()->showSection(get_index_by_label(label));
                else
                    horizontalHeader()->hideSection(get_index_by_label(label));
            });
        };

        create_action("Artist");
        create_action("Genre");
        create_action("Album");
        create_action("Duration");
    }

    void setup_columns_list(const QStringList& labels){
        horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &columns_list::open_context_menu);
        setup_context_menu();

        setSelectionBehavior(QTableWidget::SelectRows);
        setHorizontalHeaderLabels(labels);

        verticalHeader()->setVisible(false);
        horizontalHeader()->setVisible(true);
        horizontalHeader()->setSectionsMovable(true);

        setSortingEnabled(true);
        setShowGrid(false);
    }
};



#endif
