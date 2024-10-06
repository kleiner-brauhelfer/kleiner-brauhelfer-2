#include "treeview.h"

TreeView::TreeView(QWidget *parent) :
    QTreeView(parent)
{
    setHorizontalScrollMode(ScrollMode::ScrollPerPixel);
    setVerticalScrollMode(ScrollMode::ScrollPerPixel);
}
