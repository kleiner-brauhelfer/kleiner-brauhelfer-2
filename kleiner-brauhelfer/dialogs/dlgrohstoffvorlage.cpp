#include "dlgrohstoffvorlage.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include "proxymodel.h"
#include "settings.h"
#include "model/dsvtablemodel.h"
#include "brauhelfer.h"
#include "helper/obrama.h"

extern Settings* gSettings;

DlgRohstoffVorlage::DlgRohstoffVorlage(Art art, QWidget *parent) :
    DlgAbstract(staticMetaObject.className(), parent),
    ui(new Ui::DlgRohstoffVorlage),
    mRohstoffart(art)
{
    ui->setupUi(this);

  #if QT_NETWORK_LIB
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
    }
    else
  #endif
    if (mRohstoffart == Wasserprofil)
    {
        ui->labelQuelle->setVisible(false);
    }
    else
    {
        ui->labelQuelle->setText(tr("Quellen: www.mueggelland.de | www.brewferm.be | www.wyeastlab.com | www.fermentis.com | www.danstaryeast.com | ww.weyermann.de"));
    }

    setModel();

    adjustSize();
}

DlgRohstoffVorlage::~DlgRohstoffVorlage()
{
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
    case Wasserprofil:
        fileName = "Wasserprofile.csv";
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

        ProxyModel* proxyModel = new ProxyModel();
        proxyModel->setFilterKeyColumn(-1);
        proxyModel->setSourceModel(model);
        TableView *table = ui->tableView;
        table->setModel(proxyModel);
        table->resizeColumnsToContents();
        table->resizeRowsToContents();
        if (isOBraMa())
        {
            int col;
            QList<int> filterColumns;
            switch (mRohstoffart)
            {
            case MalzOBraMa:
                col = model->fieldIndex("name");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex("alias_name");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Aliasname"));
                filterColumns.append(col);
                col = model->fieldIndex("color");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Farbe [EBC]"));
                col = model->fieldIndex("max_in_batch");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Max. Anteil [%]"));
                col = model->fieldIndex("notes");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                filterColumns.append(col);
                break;
            case HopfenOBraMa:
                col = model->fieldIndex("name");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex("category");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Typ"));
                col = model->fieldIndex("alpha");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alpha [%]"));
                col = model->fieldIndex("aroma");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                filterColumns.append(col);
                col = model->fieldIndex("replacement");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alternativen"));
                filterColumns.append(col);
                break;
            case HefeOBraMa:
                col = model->fieldIndex("name");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex("category");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Kategorie"));
                col = model->fieldIndex("sub_category");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Trocken/Flüssig"));
                col = model->fieldIndex("notes");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                filterColumns.append(col);
                col = model->fieldIndex("use_for");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Bierstil"));
                filterColumns.append(col);
                col = model->fieldIndex("replacement");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alternativen"));
                filterColumns.append(col);
                break;
            case WZutatenOBraMa:
                col = model->fieldIndex("name");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex("category");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Typ"));
                col = model->fieldIndex("use_for");
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Verwendung"));
                filterColumns.append(col);
                break;
            default:
                break;
            }
            proxyModel->setFilterKeyColumns(filterColumns);
            table->build();
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
    ProxyModel *model = qobject_cast<ProxyModel*>(ui->tableView->model());
    if (model)
        model->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(txt), QRegularExpression::CaseInsensitiveOption));
}

void DlgRohstoffVorlage::on_buttonBox_accepted()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        QString typ, str1, str2;
        double fVal;
        double iVal;
        double hydrogencarbonat, calcium, magnesium;
        QHeaderView *header = ui->tableView->horizontalHeader();
        ProxyModel *proxy = qobject_cast<ProxyModel*>(ui->tableView->model());
        DsvTableModel *model = static_cast<DsvTableModel*>(proxy->sourceModel());
        switch (mRohstoffart)
        {
        case Malz:
            mValues.insert(ModelMalz::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            mValues.insert(ModelMalz::ColFarbe, index.sibling(index.row(), header->logicalIndex(1)).data().toDouble());
            iVal = index.sibling(index.row(), header->logicalIndex(2)).data().toInt();
            if (iVal > 0)
                mValues.insert(ModelMalz::ColMaxProzent, iVal);
            mValues.insert(ModelMalz::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            break;

        case MalzOBraMa:
            mValues.insert(ModelMalz::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            mValues.insert(ModelMalz::ColPotential, index.sibling(index.row(), model->fieldIndex("potential")).data().toDouble());
            mValues.insert(ModelMalz::ColFarbe, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            fVal = index.sibling(index.row(), header->logicalIndex(3)).data().toDouble();
            if (fVal > 0)
                mValues.insert(ModelMalz::ColMaxProzent, qRound(fVal));
            mValues.insert(ModelMalz::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(4)).data());
            break;

        case Hopfen:
            mValues.insert(ModelHopfen::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
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
            mValues.insert(ModelHopfen::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == "aroma")
                mValues.insert(ModelHopfen::ColTyp, 1);
            else if (typ == "bitter")
                mValues.insert(ModelHopfen::ColTyp, 2);
            else if (typ == "universal" || typ == "mischung")
                mValues.insert(ModelHopfen::ColTyp, 3);
            mValues.insert(ModelHopfen::ColAlpha, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelHopfen::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            mValues.insert(ModelHopfen::ColAlternativen, index.sibling(index.row(), header->logicalIndex(4)).data());
            break;

        case Hefe:
            mValues.insert(ModelHefe::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
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
            mValues.insert(ModelHefe::ColWuerzemenge, index.sibling(index.row(), header->logicalIndex(4)).data().toInt());
            mValues.insert(ModelHefe::ColTemperatur, index.sibling(index.row(), header->logicalIndex(5)).data());
            mValues.insert(ModelHefe::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(6)).data());
            mValues.insert(ModelHefe::ColSedimentation, index.sibling(index.row(), header->logicalIndex(7)).data());
            mValues.insert(ModelHefe::ColEVG, index.sibling(index.row(), header->logicalIndex(8)).data());
            break;

        case HefeOBraMa:
            mValues.insert(ModelHefe::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
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
            mValues.insert(ModelHefe::ColTemperatur, QString("%1 - %2").arg(str1, str2));
            mValues.insert(ModelHefe::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            mValues.insert(ModelHefe::ColSedimentation, index.sibling(index.row(), model->fieldIndex("flocculation")).data());
            str1 = index.sibling(index.row(), model->fieldIndex("attenuation_min")).data().toString();
            str2 = index.sibling(index.row(), model->fieldIndex("attenuation_max")).data().toString();
            mValues.insert(ModelHefe::ColEVG, QString("%1 - %2").arg(str1, str2));
            mValues.insert(ModelHefe::ColAlternativen, index.sibling(index.row(), header->logicalIndex(5)).data());
            break;

        case WZutaten:
            mValues.insert(ModelWeitereZutaten::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
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
            mValues.insert(ModelWeitereZutaten::ColFarbe, index.sibling(index.row(), header->logicalIndex(3)).data().toDouble());
            break;

        case WZutatenOBraMa:
            mValues.insert(ModelWeitereZutaten::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
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

        case Wasserprofil:
            mValues.insert(ModelWasser::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            mValues.insert(ModelWasser::ColCalcium, index.sibling(index.row(), header->logicalIndex(1)).data());
            mValues.insert(ModelWasser::ColMagnesium, index.sibling(index.row(), header->logicalIndex(2)).data());
            mValues.insert(ModelWasser::ColNatrium, index.sibling(index.row(), header->logicalIndex(3)).data());
            mValues.insert(ModelWasser::ColSulfat, index.sibling(index.row(), header->logicalIndex(4)).data());
            mValues.insert(ModelWasser::ColChlorid, index.sibling(index.row(), header->logicalIndex(5)).data());
            mValues.insert(ModelWasser::ColHydrogencarbonat, index.sibling(index.row(), header->logicalIndex(6)).data());
            hydrogencarbonat = mValues[ModelWasser::ColHydrogencarbonat].toDouble();
            calcium = mValues[ModelWasser::ColCalcium].toDouble();
            magnesium = mValues[ModelWasser::ColMagnesium].toDouble();
            fVal = ModelWasser::Restalkalitaet(hydrogencarbonat, calcium, magnesium);
            mValues.insert(ModelWasser::ColRestalkalitaetAdd, index.sibling(index.row(), header->logicalIndex(7)).data().toDouble() - fVal);
            break;
        }
        accept();
	}
    else
    {
        reject();
    }
}

void DlgRohstoffVorlage::on_buttonBox_rejected()
{
    reject();
}

void DlgRohstoffVorlage::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    on_buttonBox_accepted();
}
