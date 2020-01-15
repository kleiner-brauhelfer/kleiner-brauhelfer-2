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
    NullFieldsProxyModel(const QList<int>& emptyFieldsCol, QObject* parent = nullptr) :
        ProxyModel(parent),
        mEmptyFieldsCol(emptyFieldsCol)
    {
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
        mSudIds.insert(bh->modelSud()->data(row, ModelSud::ColID).toInt(), bh->modelSud()->data(row, ModelSud::ColSudname).toString());

    mTestFncs = {
        [this](){return this->test2(bh->modelRasten(), {ModelRasten::ColID, ModelRasten::ColSudID, ModelRasten::ColName, ModelRasten::ColTemp, ModelRasten::ColDauer});},
        [this](){return this->test2(bh->modelMalzschuettung(), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID, ModelMalzschuettung::ColName, ModelMalzschuettung::ColProzent});},
        [this](){return this->test2(bh->modelHopfengaben(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID, ModelHopfengaben::ColName, ModelHopfengaben::ColProzent});},
        [this](){return this->test2(bh->modelHefegaben(), {ModelHefegaben::ColID, ModelHefegaben::ColSudID, ModelHefegaben::ColName});},
        [this](){return this->test2(bh->modelWeitereZutatenGaben(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID, ModelWeitereZutatenGaben::ColName});},
        [this](){return this->test2(bh->modelSchnellgaerverlauf(), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID, ModelSchnellgaerverlauf::ColZeitstempel});},
        [this](){return this->test2(bh->modelHauptgaerverlauf(), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID, ModelHauptgaerverlauf::ColZeitstempel});},
        [this](){return this->test2(bh->modelNachgaerverlauf(), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID, ModelNachgaerverlauf::ColZeitstempel});},
        [this](){return this->test2(bh->modelBewertungen(), {ModelBewertungen::ColID, ModelBewertungen::ColSudID, ModelBewertungen::ColDatum});},
        [this](){return this->test2(bh->modelAnhang(), {ModelAnhang::ColID, ModelAnhang::ColSudID, ModelAnhang::ColPfad});},
        [this](){return this->test2(bh->modelEtiketten(), {ModelEtiketten::ColID, ModelEtiketten::ColSudID});},

        [this](){return this->test3(bh->modelGeraete(), {ModelGeraete::ColBezeichnung, ModelGeraete::ColAusruestungAnlagenID});},

        [this](){return this->test1(bh->modelSud(), {ModelSud::ColID, ModelSud::ColSudname});},
        [this](){return this->test1(bh->modelMalz(), {ModelMalz::ColID, ModelMalz::ColBeschreibung});},
        [this](){return this->test1(bh->modelHopfen(), {ModelHopfen::ColID, ModelHopfen::ColBeschreibung});},
        [this](){return this->test1(bh->modelHefe(), {ModelHefe::ColID, ModelHefe::ColBeschreibung});},
        [this](){return this->test1(bh->modelWeitereZutaten(), {ModelWeitereZutaten::ColID, ModelWeitereZutaten::ColBeschreibung});},
        [this](){return this->test1(bh->modelAusruestung(), {ModelAusruestung::ColID, ModelAusruestung::ColName});},
        [this](){return this->test1(bh->modelGeraete(), {ModelGeraete::ColID, ModelGeraete::ColAusruestungAnlagenID, ModelGeraete::ColBezeichnung});},
        [this](){return this->test1(bh->modelWasser(), {ModelWasser::ColID, ModelWasser::ColName});},
        [this](){return this->test1(bh->modelRasten(), {ModelRasten::ColID, ModelRasten::ColSudID, ModelRasten::ColName, ModelRasten::ColTemp, ModelRasten::ColDauer});},
        [this](){return this->test1(bh->modelMalzschuettung(), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID, ModelMalzschuettung::ColName});},
        [this](){return this->test1(bh->modelHopfengaben(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID, ModelHopfengaben::ColName});},
        [this](){return this->test1(bh->modelHefegaben(), {ModelHefegaben::ColID, ModelHefegaben::ColSudID, ModelHefegaben::ColName});},
        [this](){return this->test1(bh->modelWeitereZutatenGaben(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID, ModelWeitereZutatenGaben::ColName});},
        [this](){return this->test1(bh->modelSchnellgaerverlauf(), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID, ModelSchnellgaerverlauf::ColZeitstempel, ModelSchnellgaerverlauf::ColRestextrakt, ModelSchnellgaerverlauf::ColAlc, ModelSchnellgaerverlauf::ColTemp});},
        [this](){return this->test1(bh->modelHauptgaerverlauf(), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID, ModelHauptgaerverlauf::ColZeitstempel, ModelHauptgaerverlauf::ColRestextrakt, ModelHauptgaerverlauf::ColAlc, ModelHauptgaerverlauf::ColTemp});},
        [this](){return this->test1(bh->modelNachgaerverlauf(), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID, ModelNachgaerverlauf::ColZeitstempel, ModelNachgaerverlauf::ColDruck, ModelNachgaerverlauf::ColTemp, ModelNachgaerverlauf::ColCO2});},
        [this](){return this->test1(bh->modelBewertungen(), {ModelBewertungen::ColID, ModelBewertungen::ColSudID});},
        [this](){return this->test1(bh->modelAnhang(), {ModelAnhang::ColID, ModelAnhang::ColSudID, ModelAnhang::ColPfad});},
        [this](){return this->test1(bh->modelEtiketten(), {ModelEtiketten::ColID, ModelEtiketten::ColSudID, ModelEtiketten::ColPfad});},
        [this](){return this->test1(bh->modelTags(), {ModelTags::ColID, ModelTags::ColSudID, ModelTags::ColKey});}
    };
    mItTestFncs = mTestFncs.begin();
    next();
    adjustSize();
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
        ui->tableViewIds->setVisible(false);
        ui->btnDelete->setVisible(false);
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

bool DlgDatabaseCleaner::test1(SqlTableModel *model, const QList<int> &fields)
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
        for (int col : fields)
            ui->tableView->setColumnHidden(col, false);
        setTableIds();
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::test2(SqlTableModel* model, const QList<int> &fields)
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
        for (int col : fields)
            ui->tableView->setColumnHidden(col, false);
        setTableIds(mSudIds);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::test3(SqlTableModel* model, const QList<int> &fields)
{
    QMap<int, QString> ids;
    for (int row = 0; row < bh->modelAusruestung()->rowCount(); ++row)
        ids.insert(bh->modelAusruestung()->data(row, ModelAusruestung::ColID).toInt(), bh->modelAusruestung()->data(row, ModelAusruestung::ColName).toString());
    InvalidIdProxyModel* proxy = new InvalidIdProxyModel(ids, model->fieldIndex("AusruestungAnlagenID"), ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültige Anlage ID in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);
        for (int col : fields)
            ui->tableView->setColumnHidden(col, false);
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
