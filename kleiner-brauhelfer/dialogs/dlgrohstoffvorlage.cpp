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
        return gSettings->settingsDir() + fileName;
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
        proxyModel->setSourceModel(model);
        ui->tableView->setModel(proxyModel);
        ui->tableView->resizeColumnsToContents();
        connect(ui->tableView->model(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this, SLOT(slot_save()));
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

QVariantMap DlgRohstoffVorlage::values() const
{
    return mValues;
}

void DlgRohstoffVorlage::on_buttonBox_accepted()
{
    QModelIndex index = ui->tableView->currentIndex();
    if (index.isValid())
    {
        if (mRohstoffart == R_Malz)
        {
            mValues.insert("Beschreibung", index.siblingAtColumn(0).data());
            mValues.insert("Farbe", index.siblingAtColumn(1).data().toDouble());
            mValues.insert("MaxProzent", index.siblingAtColumn(2).data().toInt());
            mValues.insert("Anwendung", index.siblingAtColumn(3).data());
		}
        else if (mRohstoffart == R_Hopfen)
        {
            mValues.insert("Beschreibung", index.siblingAtColumn(0).data());
            QString typ = index.siblingAtColumn(1).data().toString().toLower();
            if (typ == "aroma")
                mValues.insert("Typ", 1);
            else if (typ == "bitter")
                mValues.insert("Typ", 2);
            else if (typ == "universal")
                mValues.insert("Typ", 3);
            mValues.insert("Alpha", index.siblingAtColumn(2).data().toDouble());
            mValues.insert("Eigenschaften", index.siblingAtColumn(3).data());
		}
        else if (mRohstoffart == R_Hefe)
        {
            mValues.insert("Beschreibung", index.siblingAtColumn(0).data());
            QString typ = index.siblingAtColumn(1).data().toString().toLower();
            if (typ == "og")
                mValues.insert("TypOGUG", 1);
            else
                mValues.insert("TypOGUG", 2);
            typ = index.siblingAtColumn(2).data().toString().toLower();
            if (typ == "trocken")
                mValues.insert("TypTrFl", 1);
            else
                mValues.insert("TypTrFl", 2);
            mValues.insert("Verpackungsmenge", index.siblingAtColumn(3).data());
            mValues.insert("Wuerzemenge", index.siblingAtColumn(4).data().toInt());
            mValues.insert("Temperatur", index.siblingAtColumn(5).data());
            mValues.insert("Eigenschaften", index.siblingAtColumn(6).data());
            typ = index.siblingAtColumn(7).data().toString().toLower();
            if (typ == "hoch")
                mValues.insert("SED", 1);
            else if (typ == "mittel")
                mValues.insert("SED", 2);
            else if (typ == "niedrig")
                mValues.insert("SED", 3);
            mValues.insert("EVG", index.siblingAtColumn(8).data());
        }
        else if (mRohstoffart == R_WZutaten)
        {
            mValues.insert("Beschreibung", index.siblingAtColumn(0).data());
            QString typ = index.siblingAtColumn(1).data().toString().toLower();
            if (typ == "honig")
                mValues.insert("Typ", EWZ_Typ_Honig);
            else if (typ == "zucker")
                mValues.insert("Typ", EWZ_Typ_Zucker);
            else if (typ == "gewürz")
                mValues.insert("Typ", EWZ_Typ_Gewuerz);
            else if (typ == "frucht")
                mValues.insert("Typ", EWZ_Typ_Frucht);
            else if (typ == "sonstiges")
                mValues.insert("Typ", EWZ_Typ_Sonstiges);
            mValues.insert("Ausbeute", index.siblingAtColumn(2).data().toDouble());
            mValues.insert("EBC", index.siblingAtColumn(3).data().toDouble());
        }
	}
    accept();
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
