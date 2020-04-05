#include "dlgrohstoffvorlage.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include "settings.h"
#include "model/dsvtablemodel.h"
#include "brauhelfer.h"
#include "helper/obrama.h"

extern Settings* gSettings;

DlgRohstoffVorlage::DlgRohstoffVorlage(Art art, QWidget *parent) :
	QDialog(parent),
    ui(new Ui::DlgRohstoffVorlage),
    mRohstoffart(art)
{
    ui->setupUi(this);
    adjustSize();
    gSettings->beginGroup("DlgRohstoffVorlage");
    resize(gSettings->value("size").toSize());
    gSettings->endGroup();

    if (isOBraMa())
    {
        OBraMa obrama;
        switch (mRohstoffart)
        {
        case MalzOBraMa:
            obrama.getTable("fermentables", "_full", gSettings->dataDir(2) + "obrama/fermentables_full.csv");
            break;
        case HopfenOBraMa:
            obrama.getTable("hops", "_full", gSettings->dataDir(2) + "obrama/hops_full.csv");
            break;
        case HefeOBraMa:
            obrama.getTable("yeasts", "_full", gSettings->dataDir(2) + "obrama/yeasts_full.csv");
            break;
        case WZutatenOBraMa:
            obrama.getTable("adjuncts", "", gSettings->dataDir(2) + "obrama/adjuncts.csv");
            break;
        default:
            break;
        }
        ui->labelQuelle->setText(tr("Quelle: obrama.mueggelland.de"));
        ui->widgetEdit->setVisible(false);
    }
    else
    {
        ui->labelQuelle->setText(tr("Quellen: www.mueggelland.de | www.brewferm.be | www.wyeastlab.com | www.fermentis.com | www.danstaryeast.com | ww.weyermann.de"));
    }

    setModel();
}

DlgRohstoffVorlage::~DlgRohstoffVorlage()
{
    gSettings->beginGroup("DlgRohstoffVorlage");
    gSettings->setValue("size", geometry().size());
    gSettings->endGroup();
	delete ui;
}

QString DlgRohstoffVorlage::getFileName(bool withPath) const
{
    QString fileName;
    switch (mRohstoffart)
    {
    case Malz:
        fileName = "Malz.csv";
        break;
    case Hopfen:
        fileName = "Hopfen.csv";
        break;
    case Hefe:
        fileName = "Hefe.csv";
        break;
    case WZutaten:
        fileName = "WeitereZutaten.csv";
        break;
    case MalzOBraMa:
        fileName = "obrama/fermentables_full.csv";
        break;
    case HopfenOBraMa:
        fileName = "obrama/hops_full.csv";
        break;
    case HefeOBraMa:
        fileName = "obrama/yeasts_full.csv";
        break;
    case WZutatenOBraMa:
        fileName = "obrama/adjuncts.csv";
        break;
    }
    return withPath ? gSettings->dataDir(2) + fileName : fileName;
}

void DlgRohstoffVorlage::setModel()
{
    QFile file(getFileName(true));
    if (file.exists())
    {
        DsvTableModel* model = new DsvTableModel(this);
        model->loadFromFile(file.fileName(), true, isOBraMa() ? ',' : ';');

        QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
        proxyModel->setFilterKeyColumn(0);
        proxyModel->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
        proxyModel->setSourceModel(model);
        ui->tableView->setModel(proxyModel);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->resizeRowsToContents();
        if (isOBraMa())
        {
            QHeaderView *header = ui->tableView->horizontalHeader();
            ui->tableView->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
            for (int c = 0; c < model->columnCount(); c++)
                ui->tableView->setColumnHidden(c, true);
            int col;
            switch (mRohstoffart)
            {
            case MalzOBraMa:
                col = model->fieldIndex("name");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 0);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                proxyModel->setFilterKeyColumn(col);
                col = model->fieldIndex("alias_name");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 1);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Aliasname"));
                col = model->fieldIndex("color");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 2);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Farbe [EBC]"));
                col = model->fieldIndex("max_in_batch");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 3);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Max. Anteil [%]"));
                col = model->fieldIndex("notes");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 4);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                break;
            case HopfenOBraMa:
                col = model->fieldIndex("name");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 0);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                proxyModel->setFilterKeyColumn(col);
                col = model->fieldIndex("category");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 1);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Typ"));
                col = model->fieldIndex("alpha");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 2);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alpha [%]"));
                col = model->fieldIndex("aroma");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 3);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                break;
            case HefeOBraMa:
                col = model->fieldIndex("name");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 0);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                proxyModel->setFilterKeyColumn(col);
                col = model->fieldIndex("category");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 1);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Kategorie"));
                col = model->fieldIndex("sub_category");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 2);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Trocken/Flüssig"));
                col = model->fieldIndex("notes");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 3);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                col = model->fieldIndex("use_for");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 4);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Bierstil"));
                break;
            case WZutatenOBraMa:
                col = model->fieldIndex("name");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 0);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                proxyModel->setFilterKeyColumn(col);
                col = model->fieldIndex("category");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 1);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Typ"));
                col = model->fieldIndex("use_for");
                ui->tableView->setColumnHidden(col, false);
                header->moveSection(header->visualIndex(col), 2);
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Verwendung"));
                break;
            default:
                break;
            }
        }
        else
        {
            connect(ui->tableView->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this, SLOT(slot_save()));
            connect(ui->tableView->model(), SIGNAL(layoutChanged()), this, SLOT(slot_save()));
            connect(ui->tableView->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(slot_save()));
            connect(ui->tableView->model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(slot_save()));
        }
        mValues.clear();
    }
}

bool DlgRohstoffVorlage::isOBraMa() const
{
    return mRohstoffart == MalzOBraMa || mRohstoffart == HopfenOBraMa ||
           mRohstoffart == HefeOBraMa || mRohstoffart == WZutatenOBraMa;
}

QMap<int, QVariant> DlgRohstoffVorlage::values() const
{
    return mValues;
}

void DlgRohstoffVorlage::on_lineEditFilter_textChanged(const QString &txt)
{
    QSortFilterProxyModel *model = qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
    model->setFilterFixedString(txt);
}

void DlgRohstoffVorlage::on_buttonBox_accepted()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        QString typ, str1, str2;
        double fVal;
        double iVal;
        QHeaderView *header = ui->tableView->horizontalHeader();
        QSortFilterProxyModel *proxy = qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
        DsvTableModel *model = static_cast<DsvTableModel*>(proxy->sourceModel());
        switch (mRohstoffart)
        {
        case Malz:
            mValues.insert(ModelMalz::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            mValues.insert(ModelMalz::ColFarbe, index.sibling(index.row(), header->logicalIndex(1)).data().toDouble());
            iVal = index.sibling(index.row(), header->logicalIndex(2)).data().toInt();
            if (iVal > 0)
                mValues.insert(ModelMalz::ColMaxProzent, iVal);
            mValues.insert(ModelMalz::ColAnwendung, index.sibling(index.row(), header->logicalIndex(3)).data());
            break;

        case MalzOBraMa:
            mValues.insert(ModelMalz::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            mValues.insert(ModelMalz::ColFarbe, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            fVal = index.sibling(index.row(), header->logicalIndex(3)).data().toDouble();
            if (fVal > 0)
                mValues.insert(ModelMalz::ColMaxProzent, qRound(fVal));
            mValues.insert(ModelMalz::ColAnwendung, index.sibling(index.row(), header->logicalIndex(4)).data());
            break;

        case Hopfen:
            mValues.insert(ModelHopfen::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "aroma")
                mValues.insert(ModelHopfen::ColTyp, 1);
            else if (typ == "bitter")
                mValues.insert(ModelHopfen::ColTyp, 2);
            else if (typ == "universal" || typ == "mischung")
                mValues.insert(ModelHopfen::ColTyp, 3);
            mValues.insert(ModelHopfen::ColAlpha, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelHopfen::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            break;

        case HopfenOBraMa:
            mValues.insert(ModelHopfen::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "aroma")
                mValues.insert(ModelHopfen::ColTyp, 1);
            else if (typ == "bitter")
                mValues.insert(ModelHopfen::ColTyp, 2);
            else if (typ == "universal" || typ == "mischung")
                mValues.insert(ModelHopfen::ColTyp, 3);
            mValues.insert(ModelHopfen::ColAlpha, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelHopfen::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            break;

        case Hefe:
            mValues.insert(ModelHefe::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "og" || typ == "obergärig")
                mValues.insert(ModelHefe::ColTypOGUG, 1);
            else if (typ == "ug" || typ == "untergärig")
                mValues.insert(ModelHefe::ColTypOGUG, 2);
            typ = index.sibling(index.row(), header->logicalIndex(2)).data().toString().toLower();
            if (typ == "trocken")
                mValues.insert(ModelHefe::ColTypTrFl, 1);
            else if (typ == "flüssig")
                mValues.insert(ModelHefe::ColTypTrFl, 2);
            mValues.insert(ModelHefe::ColVerpackungsmenge, index.sibling(index.row(), header->logicalIndex(3)).data());
            mValues.insert(ModelHefe::ColWuerzemenge, index.sibling(index.row(), header->logicalIndex(4)).data().toInt());
            mValues.insert(ModelHefe::ColTemperatur, index.sibling(index.row(), header->logicalIndex(5)).data());
            mValues.insert(ModelHefe::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(6)).data());
            typ = index.sibling(index.row(), header->logicalIndex(7)).data().toString().toLower();
            if (typ == "hoch")
                mValues.insert(ModelHefe::ColSED, 1);
            else if (typ == "mittel")
                mValues.insert(ModelHefe::ColSED, 2);
            else if (typ == "niedrig" || typ == "gering")
                mValues.insert(ModelHefe::ColSED, 3);
            mValues.insert(ModelHefe::ColEVG, index.sibling(index.row(), header->logicalIndex(8)).data());
            break;

        case HefeOBraMa:
            mValues.insert(ModelHefe::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "og" || typ == "obergärig")
                mValues.insert(ModelHefe::ColTypOGUG, 1);
            else if (typ == "ug" || typ == "untergärig")
                mValues.insert(ModelHefe::ColTypOGUG, 2);
            typ = index.sibling(index.row(), header->logicalIndex(2)).data().toString().toLower();
            if (typ == "trocken")
                mValues.insert(ModelHefe::ColTypTrFl, 1);
            else if (typ == "flüssig")
                mValues.insert(ModelHefe::ColTypTrFl, 2);
            str1 = index.sibling(index.row(), model->fieldIndex("temperature_min")).data().toString();
            str2 = index.sibling(index.row(), model->fieldIndex("temperature_max")).data().toString();
            mValues.insert(ModelHefe::ColTemperatur, QString("%1 - %2").arg(str1).arg(str2));
            mValues.insert(ModelHefe::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            typ = index.sibling(index.row(), model->fieldIndex("flocculation")).data().toString().toLower();
            if (typ == "hoch")
                mValues.insert(ModelHefe::ColSED, 1);
            else if (typ == "mittel")
                mValues.insert(ModelHefe::ColSED, 2);
            else if (typ == "niedrig" || typ == "gering")
                mValues.insert(ModelHefe::ColSED, 3);
            str1 = index.sibling(index.row(), model->fieldIndex("attenuation_min")).data().toString();
            str2 = index.sibling(index.row(), model->fieldIndex("attenuation_max")).data().toString();
            mValues.insert(ModelHefe::ColEVG, QString("%1 - %2").arg(str1).arg(str2));
            break;

        case WZutaten:
            mValues.insert(ModelWeitereZutaten::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "honig")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Honig));
            else if (typ == "zucker")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Zucker));
            else if (typ == "gewürz" || typ == "gewürze")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Gewuerz));
            else if (typ == "frucht")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Frucht));
            else if (typ == "sonstiges")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Sonstiges));
            else if (typ == "kraut" || typ == "kräuter")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Kraut));
            else if (typ == "wasseraufbereitung")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Wasseraufbereiung));
            else if (typ == "klärmittel")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Klaermittel));
            mValues.insert(ModelWeitereZutaten::ColAusbeute, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelWeitereZutaten::ColEBC, index.sibling(index.row(), header->logicalIndex(3)).data().toDouble());
            break;

        case WZutatenOBraMa:
            mValues.insert(ModelWeitereZutaten::ColBeschreibung, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "honig")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Honig));
            else if (typ == "zucker")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Zucker));
            else if (typ == "gewürz" || typ == "gewürze")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Gewuerz));
            else if (typ == "frucht")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Frucht));
            else if (typ == "sonstiges")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Sonstiges));
            else if (typ == "kraut" || typ == "kräuter")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Kraut));
            else if (typ == "wasseraufbereitung")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Wasseraufbereiung));
            else if (typ == "klärmittel")
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Klaermittel));
            break;
        }
        accept();
	}
    else
    {
        reject();
    }
}

void DlgRohstoffVorlage::slot_save()
{
    QSortFilterProxyModel *model = qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
    static_cast<DsvTableModel*>(model->sourceModel())->save(getFileName(true), true, isOBraMa() ? ',' : ';');
}

void DlgRohstoffVorlage::on_buttonBox_rejected()
{
    reject();
}

void DlgRohstoffVorlage::on_btn_Add_clicked()
{
    ui->tableView->model()->insertRow(ui->tableView->model()->rowCount());
    ui->tableView->scrollToBottom();
    ui->tableView->setCurrentIndex(ui->tableView->model()->index(ui->tableView->model()->rowCount() - 1, 0));
    ui->tableView->edit(ui->tableView->currentIndex());
}

void DlgRohstoffVorlage::on_btn_Remove_clicked()
{
    int row = ui->tableView->currentIndex().row();
    if (row >= 0)
        ui->tableView->model()->removeRow(row);
}

void DlgRohstoffVorlage::on_btn_Import_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Rohstoffliste importieren"),
                                                    QDir::currentPath() + "/" + getFileName(false),
                                                    "CSV (*.csv)");
    if (!fileName.isEmpty())
    {
        QFile file(getFileName(true));
        QFile::remove(file.fileName());
        QFile file2(fileName);
        if (file2.copy(file.fileName()))
            file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
        setModel();
    }
}

void DlgRohstoffVorlage::on_btn_Export_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Rohstoffliste exportieren"),
                                                    QDir::currentPath() + "/" + getFileName(false),
                                                    "CSV (*.csv)");
    if (!fileName.isEmpty())
    {
        QSortFilterProxyModel *model = qobject_cast<QSortFilterProxyModel*>(ui->tableView->model());
        static_cast<DsvTableModel*>(model->sourceModel())->save(fileName, true, ';');
    }
}

void DlgRohstoffVorlage::on_btn_Restore_clicked()
{
    QFile file(getFileName(true));
    QFile::remove(file.fileName());
    QFile file2(":/data/Rohstoffe/" + getFileName(false));
    if (file2.copy(file.fileName()))
        file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
    setModel();
}
