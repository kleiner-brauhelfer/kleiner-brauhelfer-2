#include "wdgrast.h"
#include "ui_wdgrast.h"
#include "brauhelfer.h"
#include "settings.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

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
    connect(bh->sud()->modelRasten(), SIGNAL(modified()), this, SLOT(updateValues()));
}

WdgRast::~WdgRast()
{
	delete ui;
}

bool WdgRast::isEnabled() const
{
    return mEnabled;
}

QVariant WdgRast::data(int col) const
{
    return bh->sud()->modelRasten()->data(mIndex, col);
}

bool WdgRast::setData(int col, const QVariant &value)
{
    return bh->sud()->modelRasten()->setData(mIndex, col, value);
}

QString WdgRast::name() const
{
    return data(ModelRasten::ColName).toString();
}

int WdgRast::temperatur() const
{
    return data(ModelRasten::ColTemp).toInt();
}

int WdgRast::dauer() const
{
    return data(ModelRasten::ColDauer).toInt();
}

void WdgRast::checkEnabled(bool force)
{
    bool enabled = bh->sud()->getStatus() == Sud_Status_Rezept;
    if (gSettings->ForceEnabled)
        enabled = true;
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    ui->cbRast->clear();
    if (mEnabled)
    {
        ui->cbRast->addItem(tr("Gummirast (35°-40°)"));
        ui->cbRast->addItem(tr("Weizenrast (45°)"));
        ui->cbRast->addItem(tr("Eiweissrast (57°)"));
        ui->cbRast->addItem(tr("Maltoserast (60°-65°)"));
        ui->cbRast->addItem(tr("Kombirast (66°-69°)"));
        ui->cbRast->addItem(tr("Verzuckerung (70°-75°)"));
        ui->cbRast->addItem(tr("Abmaischen (78°)"));

    }
    else
    {
        ui->cbRast->addItem(name());
    }
    ui->cbRast->setEditable(mEnabled);
    ui->cbRast->setEnabled(mEnabled);
    ui->cbRast->setCurrentIndex(-1);
    ui->btnLoeschen->setVisible(mEnabled);
    ui->tbTemp->setReadOnly(!mEnabled);
    ui->tbDauer->setReadOnly(!mEnabled);
    ui->btnNachOben->setVisible(mEnabled);
    ui->btnNachUnten->setVisible(mEnabled);
}

void WdgRast::updateValues(bool full)
{
    checkEnabled(full);

    if (!ui->cbRast->hasFocus())
        ui->cbRast->setCurrentText(name());
    if (!ui->tbTemp->hasFocus())
        ui->tbTemp->setValue(temperatur());
    if (!ui->tbDauer->hasFocus())
        ui->tbDauer->setValue(dauer());
    ui->btnNachOben->setEnabled(mIndex > 0);
    ui->btnNachUnten->setEnabled(mIndex < bh->sud()->modelRasten()->rowCount() - 1);
}

void WdgRast::remove()
{
    bh->sud()->modelRasten()->removeRow(mIndex);
}

void WdgRast::on_cbRast_currentTextChanged(const QString &text)
{
    if (ui->cbRast->hasFocus())
        setData(ModelRasten::ColName, text);
}

void WdgRast::on_cbRast_currentIndexChanged(int index)
{
    if (ui->cbRast->hasFocus())
    {
        setData(ModelRasten::ColName, ui->cbRast->itemText(index));
        switch (index)
        {
        case 0: //Gummirast
            setData(ModelRasten::ColTemp, 38);
            setData(ModelRasten::ColDauer, 60);
            break;
        case 1: //Weizenrast
            setData(ModelRasten::ColTemp, 45);
            setData(ModelRasten::ColDauer, 15);
            break;
        case 2: //Eiweissrast
            setData(ModelRasten::ColTemp, 57);
            setData(ModelRasten::ColDauer, 10);
            break;
        case 3: //Maltoserast
            setData(ModelRasten::ColTemp, 63);
            setData(ModelRasten::ColDauer, 35);
            break;
        case 4: //Kombirast
            setData(ModelRasten::ColTemp, 67);
            setData(ModelRasten::ColDauer, 60);
            break;
        case 5: //Verzuckerung
            setData(ModelRasten::ColTemp, 72);
            setData(ModelRasten::ColDauer, 20);
            break;
        case 6: //Abmaischen
            setData(ModelRasten::ColTemp, 78);
            setData(ModelRasten::ColDauer, 0);
        }
    }
}

void WdgRast::on_tbTemp_valueChanged(int value)
{
    if (ui->tbTemp->hasFocus())
        setData(ModelRasten::ColTemp, value);
}

void WdgRast::on_tbDauer_valueChanged(int value)
{
    if (ui->tbDauer->hasFocus())
        setData(ModelRasten::ColDauer, value);
}

void WdgRast::on_btnLoeschen_clicked()
{
    remove();
}

void WdgRast::on_btnNachUnten_clicked()
{
    if (mIndex < bh->sud()->modelRasten()->rowCount() - 1)
    {
        int row1 = bh->sud()->modelRasten()->mapRowToSource(mIndex);
        QMap<int, QVariant> values1 = bh->modelRasten()->copyValues(row1);
        int row2 = bh->sud()->modelRasten()->mapRowToSource(mIndex + 1);
        QMap<int, QVariant> values2 = bh->modelRasten()->copyValues(row2);
        bh->modelRasten()->setData(row1, values2);
        bh->modelRasten()->setData(row2, values1);
    }
}

void WdgRast::on_btnNachOben_clicked()
{
    if (mIndex > 0)
    {
        int row1 = bh->sud()->modelRasten()->mapRowToSource(mIndex);
        QMap<int, QVariant> values1 = bh->modelRasten()->copyValues(row1);
        int row2 = bh->sud()->modelRasten()->mapRowToSource(mIndex - 1);
        QMap<int, QVariant> values2 = bh->modelRasten()->copyValues(row2);
        bh->modelRasten()->setData(row1, values2);
        bh->modelRasten()->setData(row2, values1);
    }
}
