#include "dlgtableview.h"
#include "ui_dlgtableview.h"
#include "sqltablemodel.h"
#include "proxymodel.h"

DlgTableView::DlgTableView(SqlTableModel *model, QList<TableView::ColumnDefinition> columns, int filterColumn, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgTableView)
{
    ui->setupUi(this);

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
}

DlgTableView::~DlgTableView()
{
    delete ui;
}

void DlgTableView::on_tbFilter_textChanged(const QString &txt)
{
    ProxyModel *proxy = static_cast<ProxyModel*>(ui->tableView->model());
    proxy->setFilterRegExp(QRegExp(txt, Qt::CaseInsensitive, QRegExp::FixedString));
}

void DlgTableView::on_btnAdd_clicked()
{
    ProxyModel* proxy = static_cast<ProxyModel*>(ui->tableView->model());
    ui->tbFilter->clear();
    int row = proxy->append();
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
