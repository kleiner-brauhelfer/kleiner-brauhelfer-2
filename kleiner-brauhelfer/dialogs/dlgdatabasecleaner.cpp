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
    for (int row = 0; row < bh->modelAusruestung()->rowCount(); ++row)
        mAnlagenIds.insert(bh->modelAusruestung()->data(row, ModelAusruestung::ColID).toInt(), bh->modelAusruestung()->data(row, ModelAusruestung::ColName).toString());

    mTestFncs = {
        [this](){return this->testInvalidId(bh->modelRasten(), {ModelRasten::ColID, ModelRasten::ColSudID, ModelRasten::ColName, ModelRasten::ColTemp, ModelRasten::ColDauer}, 1);},
        [this](){return this->testInvalidId(bh->modelMalzschuettung(), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID, ModelMalzschuettung::ColName, ModelMalzschuettung::ColProzent}, 1);},
        [this](){return this->testInvalidId(bh->modelHopfengaben(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID, ModelHopfengaben::ColName, ModelHopfengaben::ColProzent}, 1);},
        [this](){return this->testInvalidId(bh->modelHefegaben(), {ModelHefegaben::ColID, ModelHefegaben::ColSudID, ModelHefegaben::ColName}, 1);},
        [this](){return this->testInvalidId(bh->modelWeitereZutatenGaben(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID, ModelWeitereZutatenGaben::ColName}, 1);},
        [this](){return this->testInvalidId(bh->modelSchnellgaerverlauf(), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID, ModelSchnellgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testInvalidId(bh->modelHauptgaerverlauf(), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID, ModelHauptgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testInvalidId(bh->modelNachgaerverlauf(), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID, ModelNachgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testInvalidId(bh->modelBewertungen(), {ModelBewertungen::ColID, ModelBewertungen::ColSudID, ModelBewertungen::ColDatum}, 1);},
        [this](){return this->testInvalidId(bh->modelAnhang(), {ModelAnhang::ColID, ModelAnhang::ColSudID, ModelAnhang::ColPfad}, 1);},
        [this](){return this->testInvalidId(bh->modelEtiketten(), {ModelEtiketten::ColID, ModelEtiketten::ColSudID}, 1);},

        [this](){return this->testInvalidId(bh->modelGeraete(), {ModelGeraete::ColBezeichnung, ModelGeraete::ColAusruestungAnlagenID}, 2);},

        [this](){return this->testNullField(bh->modelSud(), {ModelSud::ColID, ModelSud::ColSudname}, 1);},
        [this](){return this->testNullField(bh->modelMalz(), {ModelMalz::ColID, ModelMalz::ColBeschreibung}, 0);},
        [this](){return this->testNullField(bh->modelHopfen(), {ModelHopfen::ColID, ModelHopfen::ColBeschreibung}, 0);},
        [this](){return this->testNullField(bh->modelHefe(), {ModelHefe::ColID, ModelHefe::ColBeschreibung}, 0);},
        [this](){return this->testNullField(bh->modelWeitereZutaten(), {ModelWeitereZutaten::ColID, ModelWeitereZutaten::ColBeschreibung}, 0);},
        [this](){return this->testNullField(bh->modelAusruestung(), {ModelAusruestung::ColID, ModelAusruestung::ColName}, 0);},
        [this](){return this->testNullField(bh->modelGeraete(), {ModelGeraete::ColID, ModelGeraete::ColAusruestungAnlagenID, ModelGeraete::ColBezeichnung}, 2);},
        [this](){return this->testNullField(bh->modelWasser(), {ModelWasser::ColID, ModelWasser::ColName}, 0);},
        [this](){return this->testNullField(bh->modelRasten(), {ModelRasten::ColID, ModelRasten::ColSudID, ModelRasten::ColName, ModelRasten::ColTemp, ModelRasten::ColDauer}, 1);},
        [this](){return this->testNullField(bh->modelMalzschuettung(), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID, ModelMalzschuettung::ColName}, 1);},
        [this](){return this->testNullField(bh->modelHopfengaben(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID, ModelHopfengaben::ColName}, 1);},
        [this](){return this->testNullField(bh->modelHefegaben(), {ModelHefegaben::ColID, ModelHefegaben::ColSudID, ModelHefegaben::ColName}, 1);},
        [this](){return this->testNullField(bh->modelWeitereZutatenGaben(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID, ModelWeitereZutatenGaben::ColName}, 1);},
        [this](){return this->testNullField(bh->modelSchnellgaerverlauf(), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID, ModelSchnellgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testNullField(bh->modelHauptgaerverlauf(), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID, ModelHauptgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testNullField(bh->modelNachgaerverlauf(), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID, ModelNachgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testNullField(bh->modelBewertungen(), {ModelBewertungen::ColID, ModelBewertungen::ColSudID}, 1);},
        [this](){return this->testNullField(bh->modelAnhang(), {ModelAnhang::ColID, ModelAnhang::ColSudID, ModelAnhang::ColPfad}, 1);},
        [this](){return this->testNullField(bh->modelEtiketten(), {ModelEtiketten::ColID, ModelEtiketten::ColSudID, ModelEtiketten::ColPfad}, 1);},
        [this](){return this->testNullField(bh->modelTags(), {ModelTags::ColID, ModelTags::ColSudID, ModelTags::ColKey}, 1);}
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
        ui->lblTitle->setText("Keine (weitere) Probleme gefunden.");
        ui->lblModel->setText("");
        ui->lblModel->setVisible(false);
        ui->tableView->setModel(nullptr);
        ui->tableViewIds->setVisible(false);
        ui->btnDelete->setVisible(false);
        ui->btnWeiter->setText(tr("Schließen"));
        return;
    }

    bool ret = (*mItTestFncs)();
    ++mItTestFncs;
    if (ret)
        next();
}

void DlgDatabaseCleaner::setTableIds(int type)
{
    if (type == 0)
    {
        ui->tableViewIds->setModel(nullptr);
        ui->tableViewIds->setVisible(false);
    }
    else
    {
        QStandardItemModel* table_model = nullptr;
        const QMap<int, QString>* ids = nullptr;
        if (type == 1)
        {
            ids = &mSudIds;
            table_model = new QStandardItemModel(ids->count(), 2);
            table_model->setHeaderData(0, Qt::Horizontal, tr("Sud ID"));
            table_model->setHeaderData(1, Qt::Horizontal, tr("Sudname"));
        }
        else
        {
            ids = &mAnlagenIds;
            table_model = new QStandardItemModel(ids->count(), 2);
            table_model->setHeaderData(0, Qt::Horizontal, tr("Anlage ID"));
            table_model->setHeaderData(1, Qt::Horizontal, tr("Anlage"));
        }
        int row = 0;
        for (int key : ids->keys())
        {
            table_model->setItem(row, 0, new QStandardItem(QString::number(key)));
            table_model->setItem(row, 1, new QStandardItem((*ids)[key]));
            row++;
        }
        ui->tableViewIds->setModel(table_model);
        ui->tableViewIds->setVisible(true);
    }
}

bool DlgDatabaseCleaner::testNullField(SqlTableModel *model, const QList<int> &fields, int type)
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
        connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged()));
        ui->tableView->selectAll();
        ui->tableView->setFocus();
        setTableIds(type);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testInvalidId(SqlTableModel* model, const QList<int> &fields, int type)
{
    InvalidIdProxyModel* proxy = new InvalidIdProxyModel(type == 1 ? mSudIds : mAnlagenIds,
                                                         model->fieldIndex(type == 1 ? "SudID" : "AusruestungAnlagenID"),
                                                         ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(type == 1 ? tr("Ungültige Sud ID in Tabelle:") : tr("Ungültige Anlage ID in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int col = 0; col < model->columnCount(); ++col)
            ui->tableView->setColumnHidden(col, true);
        for (int col : fields)
            ui->tableView->setColumnHidden(col, false);
        connect(ui->tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged()));
        ui->tableView->selectAll();
        ui->tableView->setFocus();
        setTableIds(type);
        return false;
    }
    return true;
}

void DlgDatabaseCleaner::selectionChanged()
{
    ui->btnDelete->setEnabled(ui->tableView->selectionModel()->selectedRows().size() > 0);
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
    selectionChanged();
}
