#include "dlgrohstoffvorlage.h"
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QSortFilterProxyModel>
#include "settings.h"
#include "model/dsvtablemodel.h"
#include "brauhelfer.h"

#define R_Malz 1
#define R_Hopfen 2
#define R_Hefe 3
#define R_WZutaten 4

extern Settings* gSettings;

DlgRohstoffVorlage::DlgRohstoffVorlage(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgRohstoffVorlage)
{
    ui->setupUi(this);
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
    case R_Malz:
        fileName = "Malz.csv";
        break;
    case R_Hopfen:
        fileName = "Hopfen.csv";
        break;
    case R_Hefe:
        fileName = "Hefe.csv";
        break;
    case R_WZutaten:
        fileName = "WeitereZutaten.csv";
        break;
    }
    if (withPath)
        return gSettings->dataDir() + fileName;
    else
        return fileName;
}

void DlgRohstoffVorlage::viewImpl(int art)
{
    mRohstoffart = art;
    QFile file(getFileName(true));
    if (file.exists())
    {
        DsvTableModel* model = new DsvTableModel(this);
        model->loadFromFile(file.fileName(), true, ';');
        QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel();
        proxyModel->setFilterKeyColumn(0);
        proxyModel->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
        proxyModel->setSourceModel(model);
        ui->tableView->setModel(proxyModel);
        ui->tableView->resizeColumnsToContents();
        connect(ui->tableView->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this, SLOT(slot_save()));
        connect(ui->tableView->model(), SIGNAL(layoutChanged()), this, SLOT(slot_save()));
        connect(ui->tableView->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(slot_save()));
        connect(ui->tableView->model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(slot_save()));
        mValues.clear();
    }
}

void DlgRohstoffVorlage::ViewMalzauswahl()
{
    viewImpl(R_Malz);
}

void DlgRohstoffVorlage::ViewHopfenauswahl()
{
    viewImpl(R_Hopfen);
}

void DlgRohstoffVorlage::ViewHefeauswahl()
{
    viewImpl(R_Hefe);
}

void DlgRohstoffVorlage::ViewWeitereZutatenauswahl()
{
    viewImpl(R_WZutaten);
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
        if (mRohstoffart == R_Malz)
        {
            mValues.insert(ModelMalz::ColBeschreibung, index.sibling(index.row(), 0).data());
            mValues.insert(ModelMalz::ColFarbe, index.sibling(index.row(), 1).data().toDouble());
            mValues.insert(ModelMalz::ColMaxProzent, index.sibling(index.row(), 2).data().toInt());
            mValues.insert(ModelMalz::ColAnwendung, index.sibling(index.row(), 3).data());
		}
        else if (mRohstoffart == R_Hopfen)
        {
            mValues.insert(ModelHopfen::ColBeschreibung, index.sibling(index.row(), 0).data());
            QString typ = index.sibling(index.row(), 1).data().toString().toLower();
            if (typ == "aroma")
                mValues.insert(ModelHopfen::ColTyp, 1);
            else if (typ == "bitter")
                mValues.insert(ModelHopfen::ColTyp, 2);
            else if (typ == "universal")
                mValues.insert(ModelHopfen::ColTyp, 3);
            mValues.insert(ModelHopfen::ColAlpha, index.sibling(index.row(), 2).data().toDouble());
            mValues.insert(ModelHopfen::ColEigenschaften, index.sibling(index.row(), 3).data());
		}
        else if (mRohstoffart == R_Hefe)
        {
            mValues.insert(ModelHefe::ColBeschreibung, index.sibling(index.row(), 0).data());
            QString typ = index.sibling(index.row(), 1).data().toString().toLower();
            if (typ == "og")
                mValues.insert(ModelHefe::ColTypOGUG, 1);
            else
                mValues.insert(ModelHefe::ColTypOGUG, 2);
            typ = index.sibling(index.row(), 2).data().toString().toLower();
            if (typ == "trocken")
                mValues.insert(ModelHefe::ColTypTrFl, 1);
            else
                mValues.insert(ModelHefe::ColTypTrFl, 2);
            mValues.insert(ModelHefe::ColVerpackungsmenge, index.sibling(index.row(), 3).data());
            mValues.insert(ModelHefe::ColWuerzemenge, index.sibling(index.row(), 4).data().toInt());
            mValues.insert(ModelHefe::ColTemperatur, index.sibling(index.row(), 5).data());
            mValues.insert(ModelHefe::ColEigenschaften, index.sibling(index.row(), 6).data());
            typ = index.sibling(index.row(), 7).data().toString().toLower();
            if (typ == "hoch")
                mValues.insert(ModelHefe::ColSED, 1);
            else if (typ == "mittel")
                mValues.insert(ModelHefe::ColSED, 2);
            else if (typ == "niedrig")
                mValues.insert(ModelHefe::ColSED, 3);
            mValues.insert(ModelHefe::ColEVG, index.sibling(index.row(), 8).data());
        }
        else if (mRohstoffart == R_WZutaten)
        {
            mValues.insert(ModelWeitereZutaten::ColBeschreibung, index.sibling(index.row(), 0).data());
            QString typ = index.sibling(index.row(), 1).data().toString().toLower();
            if (typ == "honig")
                mValues.insert(ModelWeitereZutaten::ColTyp, EWZ_Typ_Honig);
            else if (typ == "zucker")
                mValues.insert(ModelWeitereZutaten::ColTyp, EWZ_Typ_Zucker);
            else if (typ == "gewürz")
                mValues.insert(ModelWeitereZutaten::ColTyp, EWZ_Typ_Gewuerz);
            else if (typ == "frucht")
                mValues.insert(ModelWeitereZutaten::ColTyp, EWZ_Typ_Frucht);
            else if (typ == "sonstiges")
                mValues.insert(ModelWeitereZutaten::ColTyp, EWZ_Typ_Sonstiges);
            mValues.insert(ModelWeitereZutaten::ColAusbeute, index.sibling(index.row(), 2).data().toDouble());
            mValues.insert(ModelWeitereZutaten::ColEBC, index.sibling(index.row(), 3).data().toDouble());
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
    static_cast<DsvTableModel*>(model->sourceModel())->save(getFileName(true), ';');
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
        viewImpl(mRohstoffart);
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
        static_cast<DsvTableModel*>(model->sourceModel())->save(fileName, ';');
    }
}

void DlgRohstoffVorlage::on_btn_Restore_clicked()
{
    QFile file(getFileName(true));
    QFile::remove(file.fileName());
    QFile file2(":/data/" + getFileName(false));
    if (file2.copy(file.fileName()))
        file.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
    viewImpl(mRohstoffart);
}
