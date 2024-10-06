#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class TreeView  : public QTreeView
{
    Q_OBJECT

public:
    explicit TreeView(QWidget *parent = nullptr);
};

#endif // TREEVIEW_H
