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
    for (const auto& col : qAsConst(mCols))
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

bool TableView::restoreState(const QByteArray &state)
{
    bool ret = horizontalHeader()->restoreState(state);
    if (!ret)
        return false;
    if (!mCols.empty())
    {
        QList<int> mustHave;
        QList<int> canHave;
        for (const ColumnDefinition& col : qAsConst(mCols))
        {
            canHave.append(col.col);
            if (!col.canHide)
                mustHave.append(col.col);
        }

        bool restoreDefault = false;
        for (int col = 0; col < horizontalHeader()->count(); col++)
        {
            if (!horizontalHeader()->isSectionHidden(col))
            {
                if (!canHave.contains(col))
                {
                    restoreDefault = true;
                    break;
                }
                if (mustHave.contains(col))
                {
                    mustHave.removeOne(col);
                }
            }
        }
        if (!mustHave.empty())
            restoreDefault = true;
        if (restoreDefault)
            restoreDefaultState();
    }
    return true;
}

void TableView::setDefaultContextMenu()
{
    bool addMenu = false;
    for (const auto& col : qAsConst(mCols))
    {
        if (col.canHide)
        {
            addMenu = true;
            break;
        }
    }
    if (addMenu)
    {
        QHeaderView *header = horizontalHeader();
        setContextMenuPolicy(Qt::CustomContextMenu);
        header->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
        connect(header, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
    }
}

void TableView::buildContextMenu(QMenu& menu) const
{
    QAction *action;
    for (const auto& col : mCols)
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

void TableView::appendCol(const ColumnDefinition& col)
{
    mCols.append(col);
}

void TableView::setCols(const QList<ColumnDefinition>& cols)
{
    mCols = cols;
}

void TableView::setCol(int idx, bool visible, bool canHide)
{
    if (idx < 0 || idx >= mCols.count())
        return;
    mCols[idx].visible = visible;
    mCols[idx].canHide = canHide;
    setColumnHidden(mCols[idx].col, !mCols[idx].visible);
}

void TableView::setColumnVisible(bool visible)
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action)
        setColumnHidden(action->data().toInt(), !visible);
}

void TableView::onCustomContextMenuRequested(const QPoint &pos)
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
        for (int col : qAsConst(cols))
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
                for (int col : qAsConst(cols))
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
                for (int col : qAsConst(cols))
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
