#include "dlgdatabasecleaner.h"
#include "ui_dlgdatabasecleaner.h"
#include <QStandardItemModel>
#include "settings.h"
#include "brauhelfer.h"

extern Settings* gSettings;
extern Brauhelfer* bh;

class NullFieldsProxyModel : public ProxyModel
{
public:
    NullFieldsProxyModel(const QStringList& emptyFields, QObject* parent = nullptr) :
        ProxyModel(parent),
        mEmptyFields(emptyFields)
    {
    }
    void setSourceModel(QAbstractItemModel* model) Q_DECL_OVERRIDE
    {
        mEmptyFieldsCol.clear();
        ProxyModel::setSourceModel(model);
        if(SqlTableModel* m = dynamic_cast<SqlTableModel*>(model))
        {
            for (const QString& field : mEmptyFields)
                mEmptyFieldsCol.append(m->fieldIndex(field));
        }
    }
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole)
        {
            QVariant data = index.data();
            if (!data.isValid() || data.isNull())
                return gSettings->ErrorBase;
            if (data.type() == QVariant::Type::String && data.toString().isEmpty())
                return gSettings->ErrorBase;
        }
        return ProxyModel::data(index, role);
    }
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const Q_DECL_OVERRIDE
    {
        bool accept = ProxyModel::filterAcceptsRow(source_row, source_parent);
        if (!accept)
            return false;
        for (int col : mEmptyFieldsCol)
        {
            QModelIndex index = sourceModel()->index(source_row, col, source_parent);
            if (!index.isValid())
                return true;
            QVariant data = index.data();
            if (!data.isValid() || data.isNull())
                return true;
            if (data.type() == QVariant::Type::String)
                return data.toString().isEmpty();
        }
        return false;
    }
private:
    QStringList mEmptyFields;
    QList<int> mEmptyFieldsCol;
};

class InvalidIdProxyModel : public ProxyModel
{
public:
    InvalidIdProxyModel(const QMap<int, QString>& ids, int colId, QObject* parent = nullptr) :
        ProxyModel(parent),
        mIds(ids),
        mColId(colId)
    {
    }
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole && index.column() == mColId)
        {
            int id = index.data().toInt();
            if (!mIds.contains(id))
                return gSettings->ErrorBase;
        }
        return ProxyModel::data(index, role);
    }
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const Q_DECL_OVERRIDE
    {
        bool accept = ProxyModel::filterAcceptsRow(source_row, source_parent);
        if (!accept)
            return false;
        QModelIndex index = sourceModel()->index(source_row, mColId, source_parent);
        int id = index.data().toInt();
        return !mIds.contains(id);
    }
private:
    QMap<int, QString> mIds;
    int mColId;
};

DlgDatabaseCleaner::DlgDatabaseCleaner(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDatabaseCleaner)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
        mSudIds.insert(bh->modelSud()->data(row, "ID").toInt(), bh->modelSud()->data(row, "Sudname").toString());

    mTestFncs = {
        [this](){return this->test2(bh->modelRasten(), {"ID", "SudID", "Name", "Temp", "Dauer"});},
        [this](){return this->test2(bh->modelMalzschuettung(), {"ID", "SudID", "Name", "Prozent"});},
        [this](){return this->test2(bh->modelHopfengaben(), {"ID", "SudID", "Name", "Prozent"});},
        [this](){return this->test2(bh->modelHefegaben(), {"ID", "SudID", "Name"});},
        [this](){return this->test2(bh->modelWeitereZutatenGaben(), {"ID", "SudID", "Name"});},
        [this](){return this->test2(bh->modelSchnellgaerverlauf(), {"ID", "SudID", "Zeitstempel"});},
        [this](){return this->test2(bh->modelHauptgaerverlauf(), {"ID", "SudID", "Zeitstempel"});},
        [this](){return this->test2(bh->modelNachgaerverlauf(), {"ID", "SudID", "Zeitstempel"});},
        [this](){return this->test2(bh->modelBewertungen(), {"ID", "SudID", "Datum"});},
        [this](){return this->test2(bh->modelAnhang(), {"ID", "SudID", "Pfad"});},
        [this](){return this->test2(bh->modelFlaschenlabel(), {"ID", "SudID", "Tagname"});},

        [this](){return this->test3(bh->modelGeraete(), {"Bezeichnung", "AusruestungAnlagenID"});},

        [this](){return this->test1(bh->modelSud(), {"ID", "Sudname"});},
        [this](){return this->test1(bh->modelMalz(), {"ID", "Beschreibung"});},
        [this](){return this->test1(bh->modelHopfen(), {"ID", "Beschreibung"});},
        [this](){return this->test1(bh->modelHefe(), {"ID", "Beschreibung"});},
        [this](){return this->test1(bh->modelWeitereZutaten(), {"ID", "Beschreibung"});},
        [this](){return this->test1(bh->modelAusruestung(), {"ID", "Name"});},
        [this](){return this->test1(bh->modelGeraete(), {"ID", "AusruestungAnlagenID", "Bezeichnung"});},
        [this](){return this->test1(bh->modelWasser(), {"ID", "Name"});},
        [this](){return this->test1(bh->modelRasten(), {"ID", "SudID", "Name", "Temp", "Dauer"});},
        [this](){return this->test1(bh->modelMalzschuettung(), {"ID", "SudID", "Name"});},
        [this](){return this->test1(bh->modelHopfengaben(), {"ID", "SudID", "Name"});},
        [this](){return this->test1(bh->modelHefegaben(), {"ID", "SudID", "Name"});},
        [this](){return this->test1(bh->modelWeitereZutatenGaben(), {"ID", "SudID", "Name"});},
        [this](){return this->test1(bh->modelSchnellgaerverlauf(), {"ID", "SudID", "Zeitstempel", "SW", "Alc", "Temp"});},
        [this](){return this->test1(bh->modelHauptgaerverlauf(), {"ID", "SudID", "Zeitstempel", "SW", "Alc", "Temp"});},
        [this](){return this->test1(bh->modelNachgaerverlauf(), {"ID", "SudID", "Zeitstempel", "Druck", "Temp", "CO2"});},
        [this](){return this->test1(bh->modelBewertungen(), {"ID", "SudID"});},
        [this](){return this->test1(bh->modelAnhang(), {"ID", "SudID", "Pfad"});},
        [this](){return this->test1(bh->modelFlaschenlabel(), {"ID", "SudID", "Auswahl"});},
        [this](){return this->test1(bh->modelFlaschenlabelTags(), {"ID", "SudID", "Tagname"});}
    };
    mItTestFncs = mTestFncs.begin();
    next();
}

DlgDatabaseCleaner::~DlgDatabaseCleaner()
{
    delete ui;
}

void DlgDatabaseCleaner::next()
{
    if (mItTestFncs == mTestFncs.end())
    {
        ui->lblTitle->setText("");
        ui->lblModel->setText("");
        ui->tableView->setModel(nullptr);
        ui->btnWeiter->setText(tr("Fertig"));
        return;
    }

    bool ret = (*mItTestFncs)();
    ++mItTestFncs;
    if (ret)
        next();
}

void DlgDatabaseCleaner::setTableIds(const QMap<int, QString>& ids)
{
    if (ids.isEmpty())
    {
        ui->tableViewIds->setModel(nullptr);
        ui->tableViewIds->setVisible(false);
    }
    else
    {
        QStandardItemModel* table_model = new QStandardItemModel(ids.count(), 2);
        table_model->setHeaderData(0, Qt::Horizontal, tr("ID"));
        table_model->setHeaderData(1, Qt::Horizontal, tr("Name"));
        int row = 0;
        for (int key : ids.keys())
        {
            table_model->setItem(row, 0, new QStandardItem(QString::number(key)));
            table_model->setItem(row, 1, new QStandardItem(ids[key]));
            row++;
        }
        ui->tableViewIds->setModel(table_model);
        ui->tableViewIds->setVisible(true);
    }
}

bool DlgDatabaseCleaner::test1(SqlTableModel *model, const QStringList &fields)
{
    NullFieldsProxyModel* proxy = new NullFieldsProxyModel(fields, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültige Felder in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);
        for (const QString& field : fields)
            ui->tableView->setColumnHidden(model->fieldIndex(field), false);
        setTableIds();
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::test2(SqlTableModel* model, const QStringList &fields)
{
    InvalidIdProxyModel* proxy = new InvalidIdProxyModel(mSudIds, model->fieldIndex("SudID"), ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültige Sud ID in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);
        for (const QString& field : fields)
            ui->tableView->setColumnHidden(model->fieldIndex(field), false);
        setTableIds(mSudIds);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::test3(SqlTableModel* model, const QStringList& fields)
{
    QMap<int, QString> ids;
    for (int row = 0; row < bh->modelAusruestung()->rowCount(); ++row)
        ids.insert(bh->modelAusruestung()->data(row, "ID").toInt(), bh->modelAusruestung()->data(row, "Name").toString());
    InvalidIdProxyModel* proxy = new InvalidIdProxyModel(ids, model->fieldIndex("AusruestungAnlagenID"), ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültige Anlage ID in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);
        for (const QString& field : fields)
            ui->tableView->setColumnHidden(model->fieldIndex(field), false);
        setTableIds(ids);
        return false;
    }
    return true;
}

void DlgDatabaseCleaner::on_btnWeiter_clicked()
{
    if (mItTestFncs == mTestFncs.end())
        close();
    else
        next();
}

void DlgDatabaseCleaner::on_btnDelete_clicked()
{
    SqlTableModel *model = static_cast<SqlTableModel*>(ui->tableView->model());
    QModelIndexList indices = ui->tableView->selectionModel()->selectedRows();
    std::sort(indices.begin(), indices.end(), [](const QModelIndex & a, const QModelIndex & b){ return a.row() > b.row(); });
    for (const QModelIndex& index : indices)
        model->removeRow(index.row());
}
