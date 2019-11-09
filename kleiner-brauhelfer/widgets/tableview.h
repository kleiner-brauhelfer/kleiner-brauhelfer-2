#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView
{
public:
    TableView(QWidget *parent = nullptr);
private:
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
};

#endif // TABLEVIEW_H
