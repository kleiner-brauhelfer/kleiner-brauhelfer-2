#include "tableview.h"
#include <QKeyEvent>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QClipboard>

TableView::TableView(QWidget *parent) :
    QTableView(parent)
{
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
