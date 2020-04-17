#include "tableview.h"
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QClipboard>

TableView::TableView(QWidget *parent) :
    QTableView(parent)
{
}

void TableView::build()
{
    QHeaderView *header = horizontalHeader();
    int visualIndex = 0;
    for (int c = 0; c < model()->columnCount(); ++c)
        setColumnHidden(c, true);
    for (const auto& col : cols)
    {
        setColumnHidden(col.col, !col.visible);
        if (col.itemDelegate)
            setItemDelegateForColumn(col.col, col.itemDelegate);
        if (col.width > 0)
            header->resizeSection(col.col, col.width);
        else if (col.width < 0)
            header->setSectionResizeMode(col.col, QHeaderView::Stretch);
        header->moveSection(header->visualIndex(col.col), visualIndex++);
    }
    mDefaultState = header->saveState();
}

void TableView::restoreDefaultState()
{
    horizontalHeader()->restoreState(mDefaultState);
}

void TableView::setDefaultContextMenu()
{
    QHeaderView *header = horizontalHeader();
    setContextMenuPolicy(Qt::CustomContextMenu);
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(customContextMenuRequested(const QPoint&)));
    connect(header, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(customContextMenuRequested(const QPoint&)));
}

void TableView::buildContextMenu(QMenu& menu)
{
    QAction *action;
    for (const auto& col : cols)
    {
        if (col.canHide)
        {
            action = new QAction(model()->headerData(col.col, Qt::Horizontal).toString(), &menu);
            action->setCheckable(true);
            action->setChecked(!isColumnHidden(col.col));
            action->setData(col.col);
            connect(action, SIGNAL(triggered(bool)), this, SLOT(setColumnVisible(bool)));
            menu.addAction(action);
        }
    }
    menu.addSeparator();
    action = new QAction(tr("Zurücksetzen"), &menu);
    connect(action, SIGNAL(triggered()), this, SLOT(restoreDefaultState()));
    menu.addAction(action);
}

void TableView::setColumnVisible(bool visible)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        setColumnHidden(action->data().toInt(), !visible);
}

void TableView::customContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    buildContextMenu(menu);
    menu.exec(viewport()->mapToGlobal(pos));
}

void TableView::keyPressEvent(QKeyEvent* event)
{
    QTableView::keyPressEvent(event);
    if(event->matches(QKeySequence::Copy))
    {
        QString clipboardText;

        if (!selectionModel()->hasSelection())
            return;

        // get logical indexes of columns
        QList<int> cols;
        for (int col = 0; col < model()->columnCount(); ++col)
        {
            int colLogical = horizontalHeader()->logicalIndex(col);
            if (!isColumnHidden(colLogical))
                cols << colLogical;
        }

        // header
        for (int col : cols)
        {
            QString value = model()->headerData(col, Qt::Horizontal).toString();
            clipboardText.append(value);
            if (col != cols.last())
                clipboardText.append("\t");
        }
        clipboardText.append("\n");

        QModelIndexList rows = selectionModel()->selectedRows();
        if (!rows.isEmpty())
        {
            // iterate rows
            for (QModelIndex& row : rows)
            {
                for (int col : cols)
                {
                    QModelIndex index = row.sibling(row.row(), col);

                    QString value;
                    QStyledItemDelegate* delegate = dynamic_cast<QStyledItemDelegate*>(itemDelegate(index));
                    if (delegate)
                        value = delegate->displayText(index.data(Qt::DisplayRole), locale());
                    if (value.isEmpty())
                        value = index.data(Qt::DisplayRole).toString();
                    clipboardText.append(value);

                    if (col != cols.last())
                        clipboardText.append("\t");
                }
                clipboardText.append("\n");
            }
        }
        else
        {
            // iterate indexes
            int prevRow = -1;
            for (QModelIndex& row : selectionModel()->selectedIndexes())
            {
                if (prevRow == row.row())
                    continue;
                prevRow = row.row();
                for (int col : cols)
                {
                    QModelIndex index = row.sibling(row.row(), col);

                    QString value;
                    QStyledItemDelegate* delegate = dynamic_cast<QStyledItemDelegate*>(itemDelegate(index));
                    if (delegate)
                        value = delegate->displayText(index.data(Qt::DisplayRole), locale());
                    if (value.isEmpty())
                        value = index.data(Qt::DisplayRole).toString();
                    clipboardText.append(value);

                    if (col != cols.last())
                        clipboardText.append("\t");
                }
                clipboardText.append("\n");
            }
        }

        // set clipboard text
        QApplication::clipboard()->setText(clipboardText);
    }
}
