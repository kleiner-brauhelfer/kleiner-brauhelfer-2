#include "dlgdatabasecleaner.h"
#include "ui_dlgdatabasecleaner.h"
#include <QStandardItemModel>
#include "settings.h"
#include "brauhelfer.h"
#include "mainwindow.h"
#include "model/comboboxdelegate.h"

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
#define qAsConst(x) (x)
#endif

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
            if (data.typeId() == QMetaType::QString && data.toString().isEmpty())
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
            if (data.typeId() == QMetaType::QString)
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

class CheckRangeProxyModel : public ProxyModel
{
public:
    CheckRangeProxyModel(int col, int min, int max, QObject* parent = nullptr) :
        ProxyModel(parent),
        mCol(col),
        mMin(min),
        mMax(max)
    {
    }
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE
    {
        if (role == Qt::BackgroundRole && index.column() == mCol)
        {
            int value = index.data().toInt();
            if (value < mMin || value > mMax)
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
        QModelIndex index = sourceModel()->index(source_row, mCol, source_parent);
        int value = index.data().toInt();
        return value < mMin || value > mMax;
    }
private:
    int mCol;
    int mMin;
    int mMax;
};

DlgDatabaseCleaner::DlgDatabaseCleaner(QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgDatabaseCleaner)
{
    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    for (int row = 0; row < bh->modelSud()->rowCount(); ++row)
        mSudIds.insert(bh->modelSud()->data(row, ModelSud::ColID).toInt(), bh->modelSud()->data(row, ModelSud::ColSudname).toString());
    for (int row = 0; row < bh->modelAusruestung()->rowCount(); ++row)
        mAnlagenIds.insert(bh->modelAusruestung()->data(row, ModelAusruestung::ColID).toInt(), bh->modelAusruestung()->data(row, ModelAusruestung::ColName).toString());

    mTestFncs = {
        [this](){return this->testInvalidId(bh->modelMaischplan(), {ModelMaischplan::ColID, ModelMaischplan::ColSudID, ModelMaischplan::ColName, ModelMaischplan::ColTempRast, ModelMaischplan::ColDauerRast}, 1);},
        [this](){return this->testInvalidId(bh->modelMalzschuettung(), {ModelMalzschuettung::ColID, ModelMalzschuettung::ColSudID, ModelMalzschuettung::ColName, ModelMalzschuettung::ColProzent}, 1);},
        [this](){return this->testInvalidId(bh->modelHopfengaben(), {ModelHopfengaben::ColID, ModelHopfengaben::ColSudID, ModelHopfengaben::ColName, ModelHopfengaben::ColProzent}, 1);},
        [this](){return this->testInvalidId(bh->modelHefegaben(), {ModelHefegaben::ColID, ModelHefegaben::ColSudID, ModelHefegaben::ColName}, 1);},
        [this](){return this->testInvalidId(bh->modelWeitereZutatenGaben(), {ModelWeitereZutatenGaben::ColID, ModelWeitereZutatenGaben::ColSudID, ModelWeitereZutatenGaben::ColName}, 1);},
        [this](){return this->testInvalidId(bh->modelWasseraufbereitung(), {ModelWasseraufbereitung::ColID, ModelWasseraufbereitung::ColSudID, ModelWasseraufbereitung::ColName}, 1);},
        [this](){return this->testInvalidId(bh->modelSchnellgaerverlauf(), {ModelSchnellgaerverlauf::ColID, ModelSchnellgaerverlauf::ColSudID, ModelSchnellgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testInvalidId(bh->modelHauptgaerverlauf(), {ModelHauptgaerverlauf::ColID, ModelHauptgaerverlauf::ColSudID, ModelHauptgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testInvalidId(bh->modelNachgaerverlauf(), {ModelNachgaerverlauf::ColID, ModelNachgaerverlauf::ColSudID, ModelNachgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testInvalidId(bh->modelBewertungen(), {ModelBewertungen::ColID, ModelBewertungen::ColSudID, ModelBewertungen::ColDatum}, 1);},
        [this](){return this->testInvalidId(bh->modelAnhang(), {ModelAnhang::ColID, ModelAnhang::ColSudID, ModelAnhang::ColPfad}, 1);},
        [this](){return this->testInvalidId(bh->modelEtiketten(), {ModelEtiketten::ColID, ModelEtiketten::ColSudID}, 1);},

        [this](){return this->testInvalidId(bh->modelGeraete(), {ModelGeraete::ColBezeichnung, ModelGeraete::ColAusruestungAnlagenID}, 2);},

        [this](){return this->testNullField(bh->modelSud(), {ModelSud::ColID, ModelSud::ColSudname}, 1);},
        [this](){return this->testNullField(bh->modelMalz(), {ModelMalz::ColName}, 0);},
        [this](){return this->testNullField(bh->modelHopfen(), {ModelHopfen::ColName}, 0);},
        [this](){return this->testNullField(bh->modelHefe(), {ModelHefe::ColName}, 0);},
        [this](){return this->testNullField(bh->modelWeitereZutaten(), {ModelWeitereZutaten::ColName}, 0);},
        [this](){return this->testNullField(bh->modelAusruestung(), {ModelAusruestung::ColID, ModelAusruestung::ColName}, 0);},
        [this](){return this->testNullField(bh->modelGeraete(), {ModelGeraete::ColAusruestungAnlagenID, ModelGeraete::ColBezeichnung}, 2);},
        [this](){return this->testNullField(bh->modelWasser(), {ModelWasser::ColName}, 0);},
        [this](){return this->testNullField(bh->modelMaischplan(), {ModelMaischplan::ColSudID, ModelMaischplan::ColName, ModelMaischplan::ColTempRast, ModelMaischplan::ColDauerRast}, 1);},
        [this](){return this->testNullField(bh->modelMalzschuettung(), {ModelMalzschuettung::ColSudID, ModelMalzschuettung::ColName}, 1);},
        [this](){return this->testNullField(bh->modelHopfengaben(), {ModelHopfengaben::ColSudID, ModelHopfengaben::ColName}, 1);},
        [this](){return this->testNullField(bh->modelHefegaben(), {ModelHefegaben::ColSudID, ModelHefegaben::ColName}, 1);},
        [this](){return this->testNullField(bh->modelWeitereZutatenGaben(), {ModelWeitereZutatenGaben::ColSudID, ModelWeitereZutatenGaben::ColName}, 1);},
        [this](){return this->testNullField(bh->modelWasseraufbereitung(), {ModelWasseraufbereitung::ColSudID, ModelWasseraufbereitung::ColName}, 1);},
        [this](){return this->testNullField(bh->modelSchnellgaerverlauf(), {ModelSchnellgaerverlauf::ColSudID, ModelSchnellgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testNullField(bh->modelHauptgaerverlauf(), {ModelHauptgaerverlauf::ColSudID, ModelHauptgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testNullField(bh->modelNachgaerverlauf(), {ModelNachgaerverlauf::ColSudID, ModelNachgaerverlauf::ColZeitstempel}, 1);},
        [this](){return this->testNullField(bh->modelBewertungen(), {ModelBewertungen::ColSudID}, 1);},
        [this](){return this->testNullField(bh->modelAnhang(), {ModelAnhang::ColSudID, ModelAnhang::ColPfad}, 1);},
        [this](){return this->testNullField(bh->modelEtiketten(), {ModelEtiketten::ColSudID, ModelEtiketten::ColPfad}, 1);},
        [this](){return this->testNullField(bh->modelTags(), {ModelTags::ColSudID, ModelTags::ColKey}, 1);},

        [this](){return this->testRange1();},
        [this](){return this->testRange2();},
        [this](){return this->testRange3();},
        [this](){return this->testRange4();},
        [this](){return this->testRange5();},
        [this](){return this->testRange6();},
        [this](){return this->testRange7();}
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
        ui->lblTitle->setText(tr("Keine (weitere) Probleme gefunden."));
        ui->lblModel->clear();
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
        for (auto it = ids->constBegin(); it != ids->constEnd(); it++)
        {
            table_model->setItem(row, 0, new QStandardItem(QString::number(it.key())));
            table_model->setItem(row, 1, new QStandardItem(it.value()));
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
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->selectAll();
        ui->tableView->setFocus();
        setTableIds(type);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testInvalidId(SqlTableModel* model, const QList<int> &fields, int type)
{
    InvalidIdProxyModel* proxy = new InvalidIdProxyModel(type == 1 ? mSudIds : mAnlagenIds,
                                                         model->fieldIndex(type == 1 ? QStringLiteral("SudID") : QStringLiteral("AusruestungAnlagenID")),
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
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->selectAll();
        ui->tableView->setFocus();
        setTableIds(type);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange1()
{
    int col = ModelHopfen::ColTyp;
    int min = static_cast<int>(Brauhelfer::HopfenTyp::Unbekannt);
    int max = static_cast<int>(Brauhelfer::HopfenTyp::Universal);
    SqlTableModel* model = bh->modelHopfen();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelHopfen::ColName, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate(MainWindow::HopfenTypname, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(1);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange2()
{
    int col = ModelHefe::ColTypTrFl;
    int min = static_cast<int>(Brauhelfer::HefeTyp::Unbekannt);
    int max = static_cast<int>(Brauhelfer::HefeTyp::Fluessig);
    SqlTableModel* model = bh->modelHefe();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelHefe::ColName, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate(MainWindow::HefeTypname, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(1);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange3()
{
    int col = ModelWeitereZutaten::ColTyp;
    int min = static_cast<int>(Brauhelfer::ZusatzTyp::Honig);
    int max = static_cast<int>(Brauhelfer::ZusatzTyp::Klaermittel);
    SqlTableModel* model = bh->modelWeitereZutaten();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelWeitereZutaten::ColName, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate(MainWindow::ZusatzTypname, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(0);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange4()
{
    int col = ModelWeitereZutaten::ColEinheit;
    int min = static_cast<int>(Brauhelfer::Einheit::Kg);
    int max = static_cast<int>(Brauhelfer::Einheit::ml);
    SqlTableModel* model = bh->modelWeitereZutaten();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelWeitereZutaten::ColName, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate(MainWindow::Einheiten, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(0);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange5()
{
    int col = ModelSud::ColStatus;
    int min = static_cast<int>(Brauhelfer::SudStatus::Rezept);
    int max = static_cast<int>(Brauhelfer::SudStatus::Verbraucht);
    SqlTableModel* model = bh->modelSud();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelSud::ColID, false);
        ui->tableView->setColumnHidden(ModelSud::ColSudname, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate({tr("Rezept"), tr("Gebraut"), tr("Abgefüllt"), tr("Verbraucht")}, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(0);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange6()
{
    int col = ModelSud::ColberechnungsArtHopfen;
    int min = static_cast<int>(Brauhelfer::BerechnungsartHopfen::Keine);
    int max = static_cast<int>(Brauhelfer::BerechnungsartHopfen::IBU);
    SqlTableModel* model = bh->modelSud();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelSud::ColID, false);
        ui->tableView->setColumnHidden(ModelSud::ColSudname, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate({tr("Keine"), tr("Gewicht"), tr("Bittere")}, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(0);
        ui->btnDelete->setVisible(true);
        return false;
    }
    return true;
}

bool DlgDatabaseCleaner::testRange7()
{
    int col = ModelWeitereZutatenGaben::ColEinheit;
    int min = static_cast<int>(Brauhelfer::Einheit::Kg);
    int max = static_cast<int>(Brauhelfer::Einheit::ml);
    SqlTableModel* model = bh->modelWeitereZutatenGaben();
    CheckRangeProxyModel* proxy = new CheckRangeProxyModel(col, min, max, ui->tableView);
    proxy->setSourceModel(model);
    if (proxy->rowCount() != 0)
    {
        ui->lblTitle->setText(tr("Ungültiger Wert in Tabelle:"));
        ui->lblModel->setText(model->tableName());
        ui->tableView->setModel(proxy);
        for (int c = 0; c < model->columnCount(); ++c)
            ui->tableView->setColumnHidden(c, true);
        ui->tableView->setColumnHidden(ModelWeitereZutatenGaben::ColSudID, false);
        ui->tableView->setColumnHidden(ModelWeitereZutatenGaben::ColName, false);
        ui->tableView->setColumnHidden(col, false);
        ui->tableView->setItemDelegateForColumn(col, new ComboBoxDelegate(MainWindow::Einheiten, ui->tableView));
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DlgDatabaseCleaner::selectionChanged);
        ui->tableView->setFocus();
        setTableIds(1);
        ui->btnDelete->setVisible(true);
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
    for (const QModelIndex& index : qAsConst(indices))
        model->removeRow(index.row());
    selectionChanged();
}
