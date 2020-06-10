#ifndef QT_COLLECTION_COLUMNS_LIST_HPP
#define QT_COLLECTION_COLUMNS_LIST_HPP


#include <functional>

#include <QTableWidget>
#include <QHeaderView>


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

private:
    void setup_columns_list(const QStringList& labels){
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
