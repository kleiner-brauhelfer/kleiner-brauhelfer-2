#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#ifdef QT_PRINTSUPPORT_LIB
#include <QPrinter>
#endif

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
    void clearCols();
    void appendCol(const ColumnDefinition& col);
    void setCols(const QList<ColumnDefinition>& cols);
    void setCol(int idx, bool visible, bool canHide);

public slots:
    void restoreDefaultState();
    bool restoreState(const QByteArray &state);
    void copyToClipboard(bool selectionOnly = false) const;
    void copyToClipboardSelection() const;
  #ifdef QT_PRINTSUPPORT_LIB
    void printPreview(bool selectionOnly = false) const;
    void printPreviewSelection() const;
    void printerPaintRequested(QPrinter *printer, bool selectionOnly, const QString &title) const;
  #endif

private slots:
    void setColumnVisible(bool visible);
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    QString indexValue(const QModelIndex& index) const;

private:
    QList<ColumnDefinition> mCols;
    QByteArray mDefaultState;
};

#endif // TABLEVIEW_H
