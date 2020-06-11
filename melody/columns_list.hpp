#ifndef QT_COLLECTION_COLUMNS_LIST_HPP
#define QT_COLLECTION_COLUMNS_LIST_HPP


#include <functional>

#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>


class columns_list : public QTableView{
public:
    QStandardItemModel* std_model{new QStandardItemModel{this}};
    QSortFilterProxyModel* sorted_model{new QSortFilterProxyModel{this}};

    columns_list(QWidget* parent = nullptr) : QTableView{parent}{
        setup_columns_list(0, {});
    }

    columns_list(const QStringList& labels, QWidget* parent = nullptr) : QTableView{parent}{
        setup_columns_list(0, labels);
    }

    columns_list(int rows, const QStringList& labels, QWidget* parent = nullptr) : QTableView{parent}{
        setup_columns_list(rows, labels);
    }

    virtual ~columns_list() {}

private slots:
    void open_context_menu(const QPoint& ){
        menu->exec(QCursor::pos());
    }

    int get_index_by_label(const QString& label){

        for(int n{}; n < horizontalHeader()->count(); ++n){
            if(sorted_model->headerData(n, Qt::Horizontal).toString() == label)
                return n;
        }

        return -1;
    }

private:
    QMenu* menu{new QMenu{this}};

    void setup_context_menu(){
        auto create_action = [this](const QString& label){
            auto* action = menu->addAction(label);
            action->setCheckable(true);
            action->setChecked(true);

            connect(action, &QAction::triggered, [this, label, action]{
                if(action->isChecked())
                    horizontalHeader()->showSection(get_index_by_label(label));
                else
                    horizontalHeader()->hideSection(get_index_by_label(label));
            });
        };

        for(int n{1}; n < horizontalHeader()->count(); ++n)
            create_action(sorted_model->headerData(n, Qt::Horizontal).toString());
    }

    void setup_columns_list(int rows, const QStringList& labels){
        sorted_model->setSourceModel(std_model);
        setModel(sorted_model);

        std_model->setRowCount(rows);
        std_model->setColumnCount(labels.size());

        horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &columns_list::open_context_menu);

        for(int n{}; n < labels.size(); ++n)
            sorted_model->setHeaderData(n, Qt::Horizontal, labels[n], Qt::DisplayRole);

        setup_context_menu();

        setSelectionBehavior(QTableView::SelectRows);

        verticalHeader()->setVisible(false);
        horizontalHeader()->setVisible(true);
        horizontalHeader()->setSectionsMovable(true);

        setAlternatingRowColors(true);
        setSortingEnabled(true);
        setShowGrid(false);
    }
};



#endif
