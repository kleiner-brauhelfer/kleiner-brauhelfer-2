#include "dlgtableview.h"
#include "ui_dlgtableview.h"
#include "sqltablemodel.h"
#include "proxymodel.h"
#include "settings.h"

extern Settings* gSettings;

void DlgTableView::restoreView(bool full)
{
    Q_UNUSED(full)
    gSettings->beginGroup("DlgTableView");
    gSettings->setValue("tableState", QByteArray());
    gSettings->endGroup();
}


DlgTableView::DlgTableView(SqlTableModel *model, QList<TableView::ColumnDefinition> columns, QMap<int, QVariant> defaultValues, int filterColumn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTableView),
    mDefaultValues(defaultValues)
{
    ui->setupUi(this);
    if (gSettings->theme() == Settings::Theme::Dark)
    {
        const QList<QAbstractButton*> buttons = findChildren<QAbstractButton*>();
        for (QAbstractButton* button : buttons)
        {
            QString name = button->whatsThis();
            QIcon icon = button->icon();
            if (!icon.isNull() && !name.isEmpty())
            {
                icon.addFile(QString(":/images/dark/%1.svg").arg(name), QSize(), QIcon::Normal, QIcon::Off);
                button->setIcon(icon);
            }
        }
    }

    ProxyModel* proxy = new ProxyModel(this);
    proxy->setFilterKeyColumn(filterColumn);
    proxy->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    proxy->setSourceModel(model);
    ui->tableView->setModel(proxy);

    if (!columns.empty())
    {
        ui->tableView->cols = columns;
        ui->tableView->build();
        ui->tableView->setDefaultContextMenu();
    }

    adjustSize();
    gSettings->beginGroup("DlgTableView");
    QSize size = gSettings->value("size").toSize();
    if (size.isValid())
        resize(size);
    ui->tableView->restoreState(gSettings->value("tableState").toByteArray());
    gSettings->endGroup();
}

DlgTableView::~DlgTableView()
{
    gSettings->beginGroup("DlgTableView");
    gSettings->setValue("size", geometry().size());
    gSettings->setValue("tableState", ui->tableView->horizontalHeader()->saveState());
    gSettings->endGroup();
    delete ui;
}

void DlgTableView::on_tbFilter_textChanged(const QString &txt)
{
    ProxyModel *proxy = static_cast<ProxyModel*>(ui->tableView->model());
    proxy->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(txt), QRegularExpression::CaseInsensitiveOption));
}

void DlgTableView::on_btnAdd_clicked()
{
    ProxyModel* proxy = static_cast<ProxyModel*>(ui->tableView->model());
    ui->tbFilter->clear();
    int row = proxy->append(mDefaultValues);
    if (row >= 0)
    {
        ui->tableView->setCurrentIndex(proxy->index(row, ui->tableView->horizontalHeader()->logicalIndex(0)));
        ui->tableView->scrollTo(ui->tableView->currentIndex());
        ui->tableView->edit(ui->tableView->currentIndex());
    }
}

void DlgTableView::on_btnDelete_clicked()
{
    QModelIndexList indices = ui->tableView->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        ui->tableView->model()->removeRow(index.row());
}

void DlgTableView::on_btnMoveUp_clicked()
{
    QModelIndexList indices = ui->tableView->selectionModel()->selectedRows();
    if (indices.size() > 0)
    {
        ProxyModel* proxy = static_cast<ProxyModel*>(ui->tableView->model());
        if (proxy->swap(indices[0].row(), indices[0].row() - 1))
            ui->tableView->setCurrentIndex(proxy->index(indices[0].row() - 1, ui->tableView->horizontalHeader()->logicalIndex(0)));
    }
}

void DlgTableView::on_moveDown_clicked()
{
    QModelIndexList indices = ui->tableView->selectionModel()->selectedRows();
    if (indices.size() > 0)
    {
        ProxyModel* proxy = static_cast<ProxyModel*>(ui->tableView->model());
        if (proxy->swap(indices[0].row(), indices[0].row() + 1))
            ui->tableView->setCurrentIndex(proxy->index(indices[0].row() + 1, ui->tableView->horizontalHeader()->logicalIndex(0)));
    }
}
