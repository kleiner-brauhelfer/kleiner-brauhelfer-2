#include "tableview.h"
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QTextStream>
#include <QClipboard>
#ifdef QT_PRINTSUPPORT_LIB
#include <QTextDocument>
#include <QDateTime>
#include <QInputDialog>
#include <QPrintPreviewDialog>
#endif
#include "settings.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

extern Settings* gSettings;

TableView::TableView(QWidget *parent) :
    QTableView(parent),
    mContextMenuCopy(true),
    mContextMenuPrint(true)
{
    setEditTriggers(QAbstractItemView::DoubleClicked |
                    QAbstractItemView::SelectedClicked |
                    QAbstractItemView::EditKeyPressed |
                    QAbstractItemView::AnyKeyPressed);
}

void TableView::setContextMenuFeatures(bool copy, bool print)
{
    mContextMenuCopy = copy;
    mContextMenuPrint = print;
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
        mustHave.reserve(mCols.size());
        canHave.reserve(mCols.size());
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
        connect(this, &QWidget::customContextMenuRequested, this, &TableView::onCustomContextMenuRequested);
        connect(header, &QWidget::customContextMenuRequested, this, &TableView::onCustomContextMenuRequested);
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
            connect(action, &QAction::triggered, this, &TableView::setColumnVisible);
            menu.addAction(action);
        }
    }
    menu.addSeparator();
    action = new QAction(tr("Zurücksetzen"), &menu);
    connect(action, &QAction::triggered, this, &TableView::restoreDefaultState);
    menu.addAction(action);
    if (mContextMenuCopy)
    {
        menu.addSeparator();
        action = new QAction(tr("Alles kopieren"), &menu);
        connect(action, &QAction::triggered, this, &TableView::copyToClipboard);
        menu.addAction(action);
        action = new QAction(tr("Auswahl kopieren"), &menu);
        connect(action, &QAction::triggered, this, &TableView::copyToClipboardSelection);
        menu.addAction(action);
    }
    if (mContextMenuPrint)
    {
      #ifdef QT_PRINTSUPPORT_LIB
        action = new QAction(tr("Alles drucken"), &menu);
        connect(action, &QAction::triggered, this, &TableView::printPreview);
        menu.addAction(action);
        action = new QAction(tr("Auswahl drucken"), &menu);
        connect(action, &QAction::triggered, this, &TableView::printPreviewSelection);
        menu.addAction(action);
      #endif
    }
}

void TableView::clearCols()
{
    mCols.clear();
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
        copyToClipboardSelection();
    }
}

QString TableView::indexValue(const QModelIndex& index) const
{
    QString value;
    QStyledItemDelegate* delegate = qobject_cast<QStyledItemDelegate*>(itemDelegateForIndex(index));
    if (delegate)
        value = delegate->displayText(index.data(Qt::DisplayRole), locale());
    if (value.isEmpty())
        value = index.data(Qt::DisplayRole).toString();
    return value;
}

void TableView::copyToClipboard(bool selectionOnly) const
{
    QString clipboardText;
    QTextStream out(&clipboardText);

    // visible columns
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
        out << value;
        if (col != cols.last())
            out << "\t";
    }
    out << "\n";

    // rows
    if (selectionOnly)
    {
        QModelIndexList rows = selectionModel()->selectedRows();
        if (!rows.isEmpty())
        {
            for (QModelIndex& row : rows)
            {
                for (int col : qAsConst(cols))
                {
                    QString value = indexValue(row.sibling(row.row(), col));
                    out << value;
                    if (col != cols.last())
                        out << "\t";
                }
                out << "\n";
            }
        }
        else
        {
            int prevRow = -1;
            for (QModelIndex& row : selectionModel()->selectedIndexes())
            {
                if (prevRow == row.row())
                    continue;
                prevRow = row.row();
                for (int col : qAsConst(cols))
                {
                    QString value = indexValue(row.sibling(row.row(), col));
                    out << value;
                    if (col != cols.last())
                        out << "\t";
                }
                out << "\n";
            }
        }
    }
    else
    {
        for (int row = 0; row < model()->rowCount(); ++row)
        {
            for (int col : qAsConst(cols))
            {
                QString value = indexValue(model()->index(row, col));
                out << value;
                if (col != cols.last())
                    out << "\t";
            }
            out << "\n";
        }
    }

    QApplication::clipboard()->setText(clipboardText);
}

void TableView::copyToClipboardSelection() const
{
    copyToClipboard(true);
}

#ifdef QT_PRINTSUPPORT_LIB

void TableView::printerPaintRequested(QPrinter *printer, bool selectionOnly, const QString& title) const
{
    QWidget* parent = qobject_cast<QWidget*>(sender());
    if(parent)
        parent->setEnabled(false);

    QString html;
    QTextStream out(&html);

    // visible columns
    QList<int> cols;
    for (int col = 0; col < model()->columnCount(); ++col)
    {
        int colLogical = horizontalHeader()->logicalIndex(col);
        if (!isColumnHidden(colLogical))
            cols << colLogical;
    }

    out << "<html><body>";
    if (!title.isEmpty())
        out << "<h1>" << title << "</h1>";
    out << "<table border=0 cellspacing=0 cellpadding=2>";

    // header
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int col : qAsConst(cols))
    {
        QString value = model()->headerData(col, Qt::Horizontal).toString();
        out << "<th>" << value << "</th>";
    }
    out << "</tr></thead>";

    // rows
    if (selectionOnly)
    {
        QModelIndexList rows = selectionModel()->selectedRows();
        if (!rows.isEmpty())
        {
            for (QModelIndex& row : rows)
            {
                out << "<tr>";
                for (int col : qAsConst(cols))
                {
                    QString value = indexValue(row.sibling(row.row(), col));
                    out << "<td>" << value << "</td>";
                }
                out << "</tr>";
            }
        }
        else
        {
            int prevRow = -1;
            for (QModelIndex& row : selectionModel()->selectedIndexes())
            {
                if (prevRow == row.row())
                    continue;
                prevRow = row.row();
                out << "<tr>";
                for (int col : qAsConst(cols))
                {
                    QString value = indexValue(row.sibling(row.row(), col));
                    out << "<td>" << value << "</td>";
                }
                out << "</tr>";
            }
        }
    }
    else
    {
        for (int row = 0; row < model()->rowCount(); row++)
        {
            out << "<tr>";
            for (int col : qAsConst(cols))
            {
                QString value = indexValue(model()->index(row, col));
                out << "<td>" << value << "</td>";
            }
            out << "</tr>";
        }
    }

    out << "</table></body></html>";

    QTextDocument doc;
    doc.setHtml(html);
    doc.print(printer);

    if(parent)
        parent->setEnabled(true);
}

void TableView::printPreview(bool selectionOnly) const
{
    QString title = QInputDialog::getText(nullptr, tr("Drucken"),
                                          tr("Überschrift:"), QLineEdit::Normal,
                                          QDateTime::currentDateTime().toString(QStringLiteral("dd.MM.yyyy")));
    QPrinter* printer = gSettings->createPrinter();
    QPrintPreviewDialog dlg(printer);
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, [=]{ printerPaintRequested(printer, selectionOnly, title); });
    dlg.exec();
    gSettings->savePrinter(printer);
    delete printer;
}

void TableView::printPreviewSelection() const
{
    printPreview(true);
}

#endif
