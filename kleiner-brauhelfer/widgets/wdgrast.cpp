#include "wdgrast.h"
#include "ui_wdgrast.h"
#include "brauhelfer.h"

extern Brauhelfer* bh;

WdgRast::WdgRast(int index, QWidget *parent) :
	QWidget(parent),
    ui(new Ui::WdgRast),
    mIndex(index),
    mEnabled(true)
{
	ui->setupUi(this);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelRasten(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(updateValues()));
}

WdgRast::~WdgRast()
{
	delete ui;
}

bool WdgRast::isEnabled() const
{
    return mEnabled;
}

QVariant WdgRast::data(const QString &fieldName) const
{
    return bh->sud()->modelRasten()->data(mIndex, fieldName);
}

bool WdgRast::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelRasten()->setData(mIndex, fieldName, value);
}

QString WdgRast::name() const
{
    return data("RastName").toString();
}

int WdgRast::temperatur() const
{
    return data("RastTemp").toInt();
}

int WdgRast::dauer() const
{
    return data("RastDauer").toInt();
}

void WdgRast::checkEnabled(bool force)
{
    bool enabled = !bh->sud()->getBierWurdeGebraut();
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        ui->cbRast->clear();
        ui->cbRast->addItem(tr("Gummirast (35°-40°)"));
        ui->cbRast->addItem(tr("Weizenrast (45°)"));
        ui->cbRast->addItem(tr("Eiweißrast (57°)"));
        ui->cbRast->addItem(tr("Maltoserast (60°-65°)"));
        ui->cbRast->addItem(tr("Kombirast (66°-69°)"));
        ui->cbRast->addItem(tr("Verzuckerung (70°-75°)"));
        ui->cbRast->addItem(tr("Abmaischen (78°)"));
        ui->cbRast->setEditable(true);
        ui->cbRast->setEnabled(true);
        ui->cbRast->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(true);
        ui->tbTemp->setReadOnly(false);
        ui->tbDauer->setReadOnly(false);
    }
    else
    {
        ui->cbRast->clear();
        ui->cbRast->addItem(name());
        ui->cbRast->setEditable(false);
        ui->cbRast->setEnabled(false);
        ui->cbRast->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(false);
        ui->tbTemp->setReadOnly(true);
        ui->tbDauer->setReadOnly(true);
    }
}

void WdgRast::updateValues()
{
    checkEnabled(false);

    if (!ui->cbRast->hasFocus())
        ui->cbRast->setCurrentText(name());
    if (!ui->tbTemp->hasFocus())
        ui->tbTemp->setValue(temperatur());
    if (!ui->tbDauer->hasFocus())
        ui->tbDauer->setValue(dauer());
}

void WdgRast::on_cbRast_currentTextChanged(const QString &text)
{
    if (ui->cbRast->hasFocus())
        setData("RastName", text);
}

void WdgRast::on_cbRast_currentIndexChanged(int index)
{
    if (ui->cbRast->hasFocus())
    {
        setData("RastName", ui->cbRast->itemText(index));
        switch (index)
        {
        case 0: //Gummirast
            setData("RastTemp", 38);
            setData("RastDauer", 60);
            break;
        case 1: //Weizenrast
            setData("RastTemp", 45);
            setData("RastDauer", 15);
            break;
        case 2: //Eiweißrast
            setData("RastTemp", 57);
            setData("RastDauer", 10);
            break;
        case 3: //Maltoserast
            setData("RastTemp", 63);
            setData("RastDauer", 35);
            break;
        case 4: //Kombirast
            setData("RastTemp", 67);
            setData("RastDauer", 60);
            break;
        case 5: //Verzuckerung
            setData("RastTemp", 72);
            setData("RastDauer", 20);
            break;
        case 6: //Abmaischen
            setData("RastTemp", 78);
            setData("RastDauer", 0);
        }
        setFocus();
        bh->sud()->modelRasten()->sort();
    }
}

void WdgRast::on_tbTemp_valueChanged(int value)
{
    if (ui->tbTemp->hasFocus())
        setData("RastTemp", value);
}

void WdgRast::on_tbTemp_editingFinished()
{
    setFocus();
    bh->sud()->modelRasten()->sort();
}

void WdgRast::on_tbDauer_valueChanged(int value)
{
    if (ui->tbDauer->hasFocus())
        setData("RastDauer", value);
}

void WdgRast::on_btnLoeschen_clicked()
{
    bh->sud()->modelRasten()->removeRow(mIndex);
}
