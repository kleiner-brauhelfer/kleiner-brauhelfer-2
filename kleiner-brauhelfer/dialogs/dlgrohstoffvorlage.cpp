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
            obrama.getTable(QStringLiteral("fermentables"), QStringLiteral("_full"), gSettings->dataDir(2) + "obrama/fermentables_full.csv");
            break;
        case HopfenOBraMa:
            obrama.getTable(QStringLiteral("hops"), QStringLiteral("_full"), gSettings->dataDir(2) + "obrama/hops_full.csv");
            break;
        case HefeOBraMa:
            obrama.getTable(QStringLiteral("yeasts"), QStringLiteral("_full"), gSettings->dataDir(2) + "obrama/yeasts_full.csv");
            break;
        case WZutatenOBraMa:
            obrama.getTable(QStringLiteral("adjuncts"),  QStringLiteral(""),  gSettings->dataDir(2) + "obrama/adjuncts.csv");
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

void DlgRohstoffVorlage::restoreView()
{
    DlgAbstract::restoreView(staticMetaObject.className());
}

QString DlgRohstoffVorlage::getFileName(bool withPath) const
{
    QString fileName;
    switch (mRohstoffart)
    {
    case Malz:
        fileName = QStringLiteral("Malz.csv");
        break;
    case Hopfen:
        fileName = QStringLiteral("Hopfen.csv");
        break;
    case Hefe:
        fileName = QStringLiteral("Hefe.csv");
        break;
    case WZutaten:
        fileName = QStringLiteral("WeitereZutaten.csv");
        break;
    case MalzOBraMa:
        fileName = QStringLiteral("obrama/fermentables_full.csv");
        break;
    case HopfenOBraMa:
        fileName = QStringLiteral("obrama/hops_full.csv");
        break;
    case HefeOBraMa:
        fileName = QStringLiteral("obrama/yeasts_full.csv");
        break;
    case WZutatenOBraMa:
        fileName = QStringLiteral("obrama/adjuncts.csv");
        break;
    case Wasserprofil:
        fileName = QStringLiteral("Wasserprofile.csv");
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
                col = model->fieldIndex(QStringLiteral("name"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("alias_name"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Aliasname"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("color"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Farbe") + "\n(EBC)");
                col = model->fieldIndex(QStringLiteral("max_in_batch"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Max. Anteil") + "\n(%)");
                col = model->fieldIndex(QStringLiteral("notes"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                filterColumns.append(col);
                break;
            case HopfenOBraMa:
                col = model->fieldIndex(QStringLiteral("name"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("category"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Typ"));
                col = model->fieldIndex(QStringLiteral("alpha"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alpha") + "\n(%)");
                col = model->fieldIndex(QStringLiteral("aroma"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("replacement"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alternativen"));
                filterColumns.append(col);
                break;
            case HefeOBraMa:
                col = model->fieldIndex(QStringLiteral("name"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("category"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Kategorie"));
                col = model->fieldIndex(QStringLiteral("sub_category"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Trocken/Flüssig"));
                col = model->fieldIndex(QStringLiteral("notes"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Eigenschaften"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("use_for"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Bierstil"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("replacement"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Alternativen"));
                filterColumns.append(col);
                break;
            case WZutatenOBraMa:
                col = model->fieldIndex(QStringLiteral("name"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Name"));
                filterColumns.append(col);
                col = model->fieldIndex(QStringLiteral("category"));
                table->appendCol({col, true, false, 0, nullptr});
                model->setHeaderData(col, Qt::Orientation::Horizontal, tr("Typ"));
                col = model->fieldIndex(QStringLiteral("use_for"));
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
            mValues.insert(ModelMalz::ColPotential, index.sibling(index.row(), model->fieldIndex(QStringLiteral("potential"))).data().toDouble());
            mValues.insert(ModelMalz::ColFarbe, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            fVal = index.sibling(index.row(), header->logicalIndex(3)).data().toDouble();
            if (fVal > 0)
                mValues.insert(ModelMalz::ColMaxProzent, qRound(fVal));
            mValues.insert(ModelMalz::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(4)).data());
            break;

        case Hopfen:
            mValues.insert(ModelHopfen::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == QStringLiteral("aroma"))
                mValues.insert(ModelHopfen::ColTyp, 1);
            else if (typ == QStringLiteral("bitter"))
                mValues.insert(ModelHopfen::ColTyp, 2);
            else if (typ == QStringLiteral("universal") || typ == QStringLiteral("mischung"))
                mValues.insert(ModelHopfen::ColTyp, 3);
            mValues.insert(ModelHopfen::ColAlpha, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelHopfen::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            break;

        case HopfenOBraMa:
            mValues.insert(ModelHopfen::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == QStringLiteral("aroma"))
                mValues.insert(ModelHopfen::ColTyp, 1);
            else if (typ == QStringLiteral("bitter"))
                mValues.insert(ModelHopfen::ColTyp, 2);
            else if (typ == QStringLiteral("universal") || typ == QStringLiteral("mischung"))
                mValues.insert(ModelHopfen::ColTyp, 3);
            mValues.insert(ModelHopfen::ColAlpha, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelHopfen::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            mValues.insert(ModelHopfen::ColAlternativen, index.sibling(index.row(), header->logicalIndex(4)).data());
            break;

        case Hefe:
            mValues.insert(ModelHefe::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == QStringLiteral("og") || typ == QStringLiteral("obergärig"))
                mValues.insert(ModelHefe::ColTypOGUG, 1);
            else if (typ == QStringLiteral("ug") || typ == QStringLiteral("untergärig"))
                mValues.insert(ModelHefe::ColTypOGUG, 2);
            typ = index.sibling(index.row(), header->logicalIndex(2)).data().toString().toLower();
            if (typ == QStringLiteral("trocken"))
                mValues.insert(ModelHefe::ColTypTrFl, 1);
            else if (typ == QStringLiteral("flüssig"))
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
            if (typ == QStringLiteral("og") || typ == QStringLiteral("obergärig"))
                mValues.insert(ModelHefe::ColTypOGUG, 1);
            else if (typ == QStringLiteral("ug") || typ == QStringLiteral("untergärig"))
                mValues.insert(ModelHefe::ColTypOGUG, 2);
            typ = index.sibling(index.row(), header->logicalIndex(2)).data().toString().toLower();
            if (typ == QStringLiteral("trocken"))
                mValues.insert(ModelHefe::ColTypTrFl, 1);
            else if (typ == QStringLiteral("flüssig"))
                mValues.insert(ModelHefe::ColTypTrFl, 2);
            str1 = index.sibling(index.row(), model->fieldIndex(QStringLiteral("temperature_min"))).data().toString();
            str2 = index.sibling(index.row(), model->fieldIndex(QStringLiteral("temperature_max"))).data().toString();
            mValues.insert(ModelHefe::ColTemperatur, QStringLiteral("%1 - %2").arg(str1, str2));
            mValues.insert(ModelHefe::ColEigenschaften, index.sibling(index.row(), header->logicalIndex(3)).data());
            mValues.insert(ModelHefe::ColSedimentation, index.sibling(index.row(), model->fieldIndex(QStringLiteral("flocculation"))).data());
            str1 = index.sibling(index.row(), model->fieldIndex(QStringLiteral("attenuation_min"))).data().toString();
            str2 = index.sibling(index.row(), model->fieldIndex(QStringLiteral("attenuation_max"))).data().toString();
            mValues.insert(ModelHefe::ColEVG, QStringLiteral("%1 - %2").arg(str1, str2));
            mValues.insert(ModelHefe::ColAlternativen, index.sibling(index.row(), header->logicalIndex(5)).data());
            break;

        case WZutaten:
            mValues.insert(ModelWeitereZutaten::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == QStringLiteral("honig"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Honig));
            else if (typ == QStringLiteral("zucker"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Zucker));
            else if (typ == QStringLiteral("gewürz") || typ == QStringLiteral("gewürze"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Gewuerz));
            else if (typ == QStringLiteral("frucht"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Frucht));
            else if (typ == QStringLiteral("sonstiges"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Sonstiges));
            else if (typ == QStringLiteral("kraut") || typ == QStringLiteral("kräuter"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Kraut));
            else if (typ == QStringLiteral("wasseraufbereitung"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Wasseraufbereiung));
            else if (typ == QStringLiteral("klärmittel"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Klaermittel));
            mValues.insert(ModelWeitereZutaten::ColAusbeute, index.sibling(index.row(), header->logicalIndex(2)).data().toDouble());
            mValues.insert(ModelWeitereZutaten::ColFarbe, index.sibling(index.row(), header->logicalIndex(3)).data().toDouble());
            break;

        case WZutatenOBraMa:
            mValues.insert(ModelWeitereZutaten::ColName, index.sibling(index.row(), header->logicalIndex(0)).data());
            typ = index.sibling(index.row(), header->logicalIndex(1)).data().toString().toLower();
            if (typ == QStringLiteral("honig"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Honig));
            else if (typ == QStringLiteral("zucker"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Zucker));
            else if (typ == QStringLiteral("gewürz") || typ == QStringLiteral("gewürze"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Gewuerz));
            else if (typ == QStringLiteral("frucht"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Frucht));
            else if (typ == QStringLiteral("sonstiges"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Sonstiges));
            else if (typ == QStringLiteral("kraut") || typ == QStringLiteral("kräuter"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Kraut));
            else if (typ == QStringLiteral("wasseraufbereitung"))
                mValues.insert(ModelWeitereZutaten::ColTyp, static_cast<int>(Brauhelfer::ZusatzTyp::Wasseraufbereiung));
            else if (typ == QStringLiteral("klärmittel"))
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
