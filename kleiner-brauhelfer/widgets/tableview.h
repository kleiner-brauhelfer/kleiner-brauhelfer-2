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
    void appendCol(const ColumnDefinition& col);
    void setCols(const QList<ColumnDefinition>& cols);
    void setCol(int idx, bool visible, bool canHide);

public slots:
    void restoreDefaultState();
    bool restoreState(const QByteArray &state);

private slots:
    void setColumnVisible(bool visible);
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

private:
    QList<ColumnDefinition> mCols;
    QByteArray mDefaultState;
};

#endif // TABLEVIEW_H
