#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView
{
    Q_OBJECT

public:
    struct ColumnDefinition
    {
        int col;
        bool visible;
        bool canHide;
        int width;
        QAbstractItemDelegate* itemDelegate;
    };

public:
    TableView(QWidget *parent = nullptr);
    void build();
    void setDefaultContextMenu();
    void buildContextMenu(QMenu& menu) const;

public slots:
    void restoreDefaultState();
    bool restoreState(const QByteArray &state);

private slots:
    void setColumnVisible(bool visible);
    void customContextMenuRequested(const QPoint &pos);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

public:
    QList<ColumnDefinition> cols;
    QByteArray mDefaultState;
};

#endif // TABLEVIEW_H
